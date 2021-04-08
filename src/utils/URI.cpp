//----------------------------------------------------------------------------
// Copyright (c) 2004-2015 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2012  N.Leclercq & The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB.
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// See AUTHORS file
//
// The YAT library is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
//
// The YAT library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// See COPYING file for license details
//
// Contact:
//      Stephane Poirier
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <sstream>
#include <yat/threading/Mutex.h>
#include <yat/utils/Logging.h>
#include <yat/utils/URI.h>

namespace yat
{

const std::string uri_syntax_error = "BAD_URI_SYNTAX";
Mutex URI::s_regex_mtx;

const std::string _PCT_ENCODED   = R"(%[[:xdigit:]][[:xdigit:]])";
const std::string _GEN_DELIMS    = R"([]:/?@#[])";
const std::string _SUB_DELIMS    = R"([!$&'()*+,;=])";
const std::string _UNRESERVED    = R"([-.~_[:alnum:]])";
const std::string _PCHAR         = "(" + _UNRESERVED  + "|" + _PCT_ENCODED  + "|"  + _SUB_DELIMS  + "|:|@)";
const std::string _SEGMENT       = _PCHAR + "*";
const std::string _SEGMENT_NZ    = _PCHAR + "+";
const std::string _SEGMENT_NZ_NC = "(" + _UNRESERVED + "|" + _PCT_ENCODED + "|" + _SUB_DELIMS + "|@)+";
const std::string _PATH_ABEMPTY  = "((/" + _SEGMENT + ")*)";
const std::string _PATH_ABSOLUTE = "(/(" + _SEGMENT_NZ + "(/" + _SEGMENT + ")*)*)";
const std::string _PATH_NOSCHEME = "(" + _SEGMENT_NZ_NC + "(/" + _SEGMENT + ")*)";
const std::string _PATH_ROOTLESS = "(" + _SEGMENT_NZ + "(/" + _SEGMENT + ")*)";
const std::string _PATH_EMPTY    = _PCHAR + "{0}";
const std::string _PATH          = "(" + _PATH_ABEMPTY + "|" + _PATH_ABSOLUTE + "|" + _PATH_ROOTLESS + "|" + _PATH_EMPTY + ")";
const std::string _SCHEME        = "([[:alpha:]][-+.[:alnum:]]*)";
const std::string _QUERY         = "((" + _PCHAR + R"(|/|\?)*))";
const std::string _FRAGMENT      = "((" + _PCHAR + R"(|/|\?)*))";
const std::string _REG_NAME      = "(" + _UNRESERVED + "|" + _PCT_ENCODED + "|" + _SUB_DELIMS + ")*";
const std::string _USERINFO      = "(" + _UNRESERVED + "|" + _PCT_ENCODED + "|" + _GEN_DELIMS + "|:)*";
const std::string _DEC_OCTET     = "([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])";
const std::string _PORT          = "([0-9]*)";
const std::string _IPV4          = _DEC_OCTET + R"(\.)" + _DEC_OCTET + R"(\.)" + _DEC_OCTET + R"(\.)" + _DEC_OCTET;
const std::string _IPV4FORM      = R"([[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+)";
const std::string _H16           = "[[:xdigit:]]{1,4}";
const std::string _LS32          = "(" + _H16 + ":" + _H16 + "|" + _IPV4 + ")";
const std::string _IPV6_1        =                                      "(" + _H16 + ":){6}" + _LS32;
const std::string _IPV6_2        =                                    "::("  +_H16 + ":){5}" + _LS32;
const std::string _IPV6_3        =                     "("  +_H16 + ")?::(" + _H16 + ":){4}" + _LS32;
const std::string _IPV6_4        = "((" + _H16 + ":){0,1}" + _H16 + ")?::(" + _H16 + ":){3}" + _LS32;
const std::string _IPV6_5        = "((" + _H16 + ":){0,2}" + _H16 + ")?::(" + _H16 + ":){2}" + _LS32;
const std::string _IPV6_6        = "((" + _H16 + ":){0,3}" + _H16 + ")?::(" + _H16 + ":){1}" + _LS32;
const std::string _IPV6_7        = "((" + _H16 + ":){0,4}" + _H16 + ")?::" + _LS32;
const std::string _IPV6_8        = "((" + _H16 + ":){0,5}" + _H16 + ")?::" + _H16;
const std::string _IPV6_9        = "((" + _H16 + ":){0,6}" + _H16 + ")?::";
const std::string _IPV6          = "(" + _IPV6_1 + "|" + _IPV6_2 + "|" + _IPV6_3 + "|" + _IPV6_4 + "|" + _IPV6_5 \
                                 + "|" + _IPV6_6 + "|" + _IPV6_7 + "|" + _IPV6_8 + "|" + _IPV6_9 + ")";
const std::string _IPVFUTURE     = R"(([vV][[:xdigit:]]+\.()" + _UNRESERVED + "|" + _SUB_DELIMS + "|:))";
const std::string _IP_LITERAL    = R"((\[()" + _IPV6 + "|" + _IPVFUTURE + R"()\]))";
const std::string _HOST          = "(" + _IP_LITERAL + "|" + _IPV4 + "|" + _REG_NAME + ")";
const std::string _AUTHORITY     = "(((" + _USERINFO + ")@)?" + _HOST + "(:" + _PORT + ")?)";
const std::string _HIER_PART     = "((//" + _AUTHORITY + _PATH_ABEMPTY + ")|" + _PATH_ABSOLUTE \
                                 + "|" + _PATH_ROOTLESS + "|" + _PATH_EMPTY + ")";
const std::string  _URI          = _SCHEME + ":" + _HIER_PART + R"((\?)" + _QUERY + ")?(#" + _FRAGMENT + ")?";
Regex URI::s_re_scheme(_SCHEME, Regex::extended | Regex::nosubs);
Regex URI::s_re_userinfo(_USERINFO, Regex::extended | Regex::nosubs);
Regex URI::s_re_path(_PATH, Regex::extended | Regex::nosubs);
Regex URI::s_re_host(_HOST, Regex::extended | Regex::nosubs);
Regex URI::s_re_port(_PORT, Regex::extended | Regex::nosubs);
Regex URI::s_re_query(_QUERY, Regex::extended | Regex::nosubs);
Regex URI::s_re_fragment(_FRAGMENT, Regex::extended | Regex::nosubs);
Regex URI::s_re_authority(_AUTHORITY, Regex::extended);
Regex URI::s_re_full(_URI, Regex::extended);
Regex URI::s_re_ipv4form(_IPV4FORM, Regex::extended | Regex::nosubs);
Regex URI::s_re_ipv4(_IPV4, Regex::extended | Regex::nosubs);

// just for the fun!
yat::String URI::get_full_pattern() { return _URI; }

//----------------------------------------------------------------------------
// URI::re_match
//----------------------------------------------------------------------------
bool URI::re_match(Regex& re, const String& str, Regex::Match* match_p)
{
  AutoMutex<> _lock(s_regex_mtx);
  return re.match(str, match_p);
}

//----------------------------------------------------------------------------
// URI::URI (const std::string& uri_string)
//----------------------------------------------------------------------------
URI::URI(const std::string& uri_string)
{
  parse(uri_string);
}

//----------------------------------------------------------------------------
// URI::URI (const URI::Fields& fields)
//----------------------------------------------------------------------------
URI::URI(const URI::Fields& fields)
{
  if( check(URI::SCHEME, fields.scheme) &&
      check(URI::USERINFO, fields.userinfo) &&
      check(URI::HOST, fields.host) &&
      check(URI::PORT, fields.port) &&
      check(URI::PATH, fields.path) &&
      check(URI::QUERY, fields.query) &&
      check(URI::FRAGMENT, fields.fragment) )
  {
    m_part[URI::SCHEME]    = fields.scheme;
    m_part[URI::USERINFO]  = fields.userinfo;
    m_part[URI::HOST]      = fields.host;
    m_part[URI::PORT]      = fields.port;
    m_part[URI::PATH]      = fields.path;
    m_part[URI::QUERY]     = fields.query;
    m_part[URI::FRAGMENT]  = fields.fragment;
  }
}

//----------------------------------------------------------------------------
// URI::check_value
//----------------------------------------------------------------------------
bool URI::check_value(const std::string& part, Regex &re,
                         const std::string& part_name, bool throw_exception)
{
  if( !re_match(re, part) )
  {
    if( throw_exception )
      THROW_YAT_ERROR(uri_syntax_error,
                      "Bad '" << part_name << "' syntax: " << part,
                      "yat::URI::check_value" );
    YAT_WARNING << "Bad '" << part_name << "' syntax: " << part << std::endl;
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
// URI::check
//----------------------------------------------------------------------------
bool URI::check(URI::Part part, const std::string& value, bool throw_exception)
{
  switch ( part )
  {
    case URI::SCHEME:
      return check_value(value, s_re_scheme, "scheme", throw_exception);

    case URI::AUTHORITY:
      return check_value(value, s_re_authority, "authority", throw_exception);

    case URI::USERINFO:
      return check_value(value, s_re_userinfo, "userinfo", throw_exception);

    case URI::HOST:
      return check_host(value, throw_exception);

    case URI::PORT:
      return check_value(value, s_re_port, "port", throw_exception);

    case URI::PATH:
      return check_value(value, s_re_path, "path", throw_exception);

    case URI::QUERY:
      return check_value(value, s_re_query, "query", throw_exception);

    case URI::FRAGMENT:
      return check_value(value, s_re_fragment, "fragment", throw_exception);

    default:
      throw Exception("ERROR", "invalid part specified", "yat::URI::check");
  }

  if( throw_exception )
    throw Exception(uri_syntax_error, "invalid value for specified uri part", "yat::URI::check");

  return false;
}

//----------------------------------------------------------------------------
// URI::split_authority
//----------------------------------------------------------------------------
void URI::split_authority(const std::string& authority, std::string* userinfo_ptr,
                          std::string* host_ptr, std::string* port_ptr)
{
  if( !authority.empty() )
  {
    std::string value = authority;

    // Look for user info
    std::string userinfo;
    if( value.find('@') != std::string::npos )
      yat::StringUtil::extract_token(&value, '@', userinfo_ptr);

    // Look for port number
    std::string port;
    if( value.find(':') != std::string::npos )
      yat::StringUtil::extract_token_right(&value, ':', port_ptr);

    // The remaining part is the host name
    *host_ptr = value;
  }
}

//----------------------------------------------------------------------------
// URI::check_authority
//----------------------------------------------------------------------------
bool URI::check_authority(const std::string& authority, URI::Fields* fields_ptr,
                          bool throw_exception)
{
  if( !authority.empty() )
  {
    Regex::Match m;
    if( !re_match(s_re_authority, authority, &m) )
    {
      if( throw_exception )
        throw Exception(uri_syntax_error,
                        StringFormat("'{}' is not a valid authority!").format(authority),
                        "yat::URI::check_authority");
      return false;
    }

    fields_ptr->userinfo = m.str(3);
    fields_ptr->host = m.str(5);
    fields_ptr->port = m.str(71);
  }
  return true;
}

//----------------------------------------------------------------------------
// URI::check_host
//----------------------------------------------------------------------------
bool URI::check_host(const std::string& host, bool throw_exception)
{
  Regex::Match m;
  if( !re_match(s_re_host, host, &m) )
  {
    if( throw_exception )
      throw Exception(uri_syntax_error,
                      StringFormat("'{}' is not a valid host name!").format(host),
                      "yat::URI::check_host");
    YAT_WARNING << "Bad host name: '" << host << std::endl;
    return false;
  }

  if( s_re_ipv4form.match(host) && !s_re_ipv4.match(host) )
    THROW_YAT_ERROR(uri_syntax_error,
                    StringFormat("'{}' is not a valid host name").format(host),
                   "yat::URI::check_host");

  return true;
}

//----------------------------------------------------------------------------
// URI::parse_ex
//----------------------------------------------------------------------------
void URI::parse(const std::string& uri)
{
  AutoMutex<> _lock(s_regex_mtx);

  Regex::Match m;
  if( !s_re_full.match(uri, &m) )
  {
    THROW_YAT_ERROR(uri_syntax_error,
                    StringFormat("'{}' is not a valid uri").format(uri),
                   "yat::URI::parse");
  }

  // The difference between ipv4 address and reg-name is ambiguous
  // We considere a host name like [digit]+.[digit]+.[digit]+.[digit]+ should be
  // a ipv4 address. If it's the case then check if it's a valid one
  std::string host = m.str(8);

  if( s_re_ipv4form.match(host) && !s_re_ipv4.match(host) )
    THROW_YAT_ERROR(uri_syntax_error,
                    StringFormat("'{}' is not a valid uri").format(uri),
                   "yat::URI::parse");

  m_part[SCHEME]     = m.str(1);
  m_part[AUTHORITY]  = m.str(4);
  m_part[USERINFO]   = m.str(6);
  m_part[HOST]       = m.str(8);
  m_part[PORT]       = m.str(74);
  m_part[PATH]       = m.str(75).empty() ? m.str(83) : m.str(75);
  m_part[QUERY]      = m.str(89);
  m_part[FRAGMENT]   = m.str(93);
}

//----------------------------------------------------------------------------
// URL::empty
//----------------------------------------------------------------------------
bool URI::empty() const
{
  if( get().empty() )
    return true;

  return false;
}

//----------------------------------------------------------------------------
// URL::clear
//----------------------------------------------------------------------------
void URI::clear()
{
  m_part.clear();
}

//----------------------------------------------------------------------------
// URL::get
//----------------------------------------------------------------------------
std::string URI::get() const
{
  std::ostringstream s;

  std::string scheme = value(URI::SCHEME);
  if( !scheme.empty() )
    s << value(URI::SCHEME) << ':';

  std::string authority = get(URI::AUTHORITY);
  if( !authority.empty() )
    s << "//" << authority;

  std::string path = get(URI::PATH);
  if( !path.empty() )
    s << path;

  std::string query = get(URI::QUERY);
  if( !query.empty() )
    s << '?' << query;

  std::string fragment = get(URI::FRAGMENT);
  if( !fragment.empty() )
    s << '#' << fragment;

  return s.str();
}

//----------------------------------------------------------------------------
// URI::value
//----------------------------------------------------------------------------
std::string URI::value(URI::Part part) const
{
  std::map<Part, std::string>::const_iterator cit = m_part.find(part);
  if( cit != m_part.end() )
    return cit->second;
  return "";
}

//----------------------------------------------------------------------------
// URI::get
//----------------------------------------------------------------------------
std::string URI::get(URI::Part part) const
{
  std::string v;

  if( URI::AUTHORITY == part )
  {
    std::ostringstream s;
    if( !value(URI::USERINFO).empty() )
      s <<  value(URI::USERINFO) << '@';
    if( !value(URI::HOST).empty() )
      s <<  value(URI::HOST);
    if( !value(URI::PORT).empty() )
      s <<  ':' << value(URI::PORT);

    v = s.str();
  }
  else
  {
    v = value(part);
  }
  return v;
}

//----------------------------------------------------------------------------
// URI::set
//----------------------------------------------------------------------------
void URI::set(URI::Part part, const std::string &v)
{
  std::string val = v;

  if( URI::AUTHORITY == part )
  {
    URI::Fields fields;
    check_authority(val, &fields, true);
    m_part[URI::USERINFO] = fields.userinfo;
    m_part[URI::HOST] = fields.host;
    m_part[URI::PORT] = fields.port;
  }
  else if( URI::HOST == part )
  {
    URI::Fields fields;
    check_host(val, true);
    m_part[URI::HOST] = val;
  }
  else
  {
    check(part, val, true);
    m_part[part] = val;
  }
}

//----------------------------------------------------------------------------
// URI::set
//----------------------------------------------------------------------------
void URI::set(const std::string &value)
{
  parse(value);
}

//----------------------------------------------------------------------------
// URI::pct_encode
//----------------------------------------------------------------------------
void URI::pct_encode(std::string* to_encode, const std::string& reserved)
{
  std::ostringstream encoded;
  for( std::size_t i = 0; i < (*to_encode).size(); ++i )
  {
    if( reserved.find((*to_encode)[i]) == std::string::npos )
      encoded << (*to_encode)[i];
    else
    {
      std::ostringstream oss;
      oss << std::hex << std::uppercase << static_cast<int>((*to_encode)[i]);
      encoded << '%' << oss.str();
    }
  }
  *to_encode = encoded.str();
}

//----------------------------------------------------------------------------
// URI::pct_decode
//----------------------------------------------------------------------------
 void URI::pct_decode(std::string* to_decode)
 {
    std::ostringstream decoded;
    std::size_t size = (*to_decode).size();

    for( std::size_t i = 0; i < size; ++i )
    {
      if( '%' != (*to_decode)[i] )
      {
        decoded << (*to_decode)[i];
      }
      else
      {
        if( i > size - 2 )
          throw yat::Exception("ERROR", "Bad URI string", "yat::URI::pct_decode");

        int n;
        std::istringstream( (*to_decode).substr(i+1, 2) ) >> std::hex >> n;
        decoded << char(n);
        i += 2;
      }
    }
    *to_decode = decoded.str();
 }

} //- namespace yat

