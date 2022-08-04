//----------------------------------------------------------------------------
// Copyright (c) 2004-2021 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2021  The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB.
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// see AUTHORS file
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


#ifndef __YAT_URI_H__
#define __YAT_URI_H__

#include <map>
#include <yat/utils/String.h>
#include <yat/threading/Mutex.h>
#include <yat/regex/Regex.h>

namespace yat
{

const std::string URI_RESERVED = " !$&'()*+,;=:?#[]@%";
const std::string URI_RESERVED_AUTHORITY = " !$&'()*+,;=?#%";
const std::string URI_PCT_ENCODED = " %!$&'()*+,;=?#";

// ============================================================================
//! \class URI
//! \brief Unified Resource Identifier class.
//!
//! This class is aimed to allow parsing and building URLs.
//! \par The URI
//! A URI is a compact sequence of characters that identifies an abstract or physical
//! resource.
//! \par
//! URI syntax is defined by RFC 3986.
//! \par
//! The generic URI syntax consists of a hierarchical sequence of
//!  components referred to as the scheme, authority, path, query, and fragment.
//! \par
//! The basic syntax is:
//! \verbatim  URI = scheme ":" hier-part \[ "?" query \] \[ "\#" fragment \] \endverbatim
//!
// ============================================================================
class YAT_DECL URI
{
public:
  //! \brief URI component types.
  enum Part
  {
    //! Scheme.
    SCHEME = 0,
    //! Authority.
    AUTHORITY,
    //! User information.
    USERINFO,
    //! Host.
    HOST,
    //! Port.
    PORT,
    //! Path.
    PATH,
    //! Query.
    QUERY,
    //! Fragment.
    FRAGMENT
  };

  //! \brief URI components structure.
  struct Fields
  {
    yat::String scheme;
    yat::String userinfo;
    yat::String host;
    yat::String port;
    yat::String path;
    yat::String query;
    yat::String fragment;
  };

private:

  RegexUPtr m_re_full_uptr;
  RegexUPtr m_re_scheme_uptr;
  RegexUPtr m_re_authority_uptr;
  RegexUPtr m_re_userinfo_uptr;
  RegexUPtr m_re_host_uptr;
  RegexUPtr m_re_port_uptr;
  RegexUPtr m_re_path_uptr;
  RegexUPtr m_re_query_uptr;
  RegexUPtr m_re_fragment_uptr;
  RegexUPtr m_re_ipv4form_uptr;
  RegexUPtr m_re_ipv4_uptr;

  std::map<Part, yat::String> m_part;

  yat::String value(Part part) const;
  void split_authority(const std::string& authority, std::string* userinfo_ptr,
                              std::string* host_ptr, std::string* port_ptr);
  bool check_authority(const std::string& authority, URI::Fields* fields_ptr,
                              bool throw_exception);
  bool check_host(const std::string& host, bool throw_exception);

  void parse(const std::string& URI);
  bool check_value(const std::string& value, Regex* re,
                          const std::string& error_desc, bool throw_exception);
  bool re_match(Regex* re, const yat::String& str, Regex::Match* m=NULL);

public:
  //! \brief Constructor from URI.
  //!
  //! Creates an URI object from the regular representation of the URL: a string.
  //! \param uri_string The URI value.
  //! \exception BAD_URI_SYNTAX Thrown if URI syntax is not correct.
  URI(const std::string& uri_string);

  //! \brief Constructor from explicit values.
  //!
  //! Creates an URI object from explicit URI fields.
  //! \param fields URI components structure.
  //! \exception BAD_URI_SYNTAX Thrown if URI syntax is not correct.
  URI(const URI::Fields& fields);

  //! \brief Default constructor.
  URI();

  //! \brief Copy constructor.
  URI(const URI& src);

  //! \brief operator=
  URI& operator=(const URI& src);

  //! \brief Clear all parts
  void clear();

  //! \brief Gets the URI as a string.
  yat::String get() const;

  //! \brief Gets a specific part of the URI as a string.
  //!
  //! \param part The URI part to retrieve.
  yat::String get(Part part) const;

  //! \brief Sets a specific part of the URI.
  //!
  //! \param part The URI part to set.
  //! \param value Value.
  //! \exception BAD_URI_SYNTAX Thrown if the value syntax is not correct.
  void set(Part part, const std::string &value);

  //! \brief Sets the URI.
  //!
  //! \param value The URI value.
  //! \exception BAD_URI_SYNTAX Thrown if the URI syntax is not correct.
  void set(const std::string &value);

  //! \brief return true if all fields are empty.
  //!
  bool empty() const;

  //! \brief Checks the syntax of the value of an URI part.
  //!
  //! Returns true if check is OK, false otherwise.
  //! \param part The URI part to check.
  //! \param value The value to check.
  //! \param throw_exception If set to true, throws an exception if an error occurs.
  //! \exception BAD_URI_SYNTAX Thrown if the value syntax is not correct.
  bool check(Part part, const std::string &value, bool throw_exception=false);

  //! \brief Just the the fun ;)
  yat::String get_full_pattern();

  //! \brief percent encoding for any string that have to be part of an uri
  static void pct_encode(std::string& to_encode, const std::string& reserved=URI_RESERVED);

  //! \brief percent decoding for any string that have to be part of an uri
  static void pct_decode(std::string& to_decode);

  //! \name utility methods
  //@{

  //! return true if the given host is a valid ipv4 address
  static bool is_ipv4(const std::string& host);

  //@}

  //! \name deprecated
  //@{
  static void pct_encode(std::string* x, const std::string& y=URI_RESERVED) { pct_encode(*x, y); }
  static void pct_decode(std::string* x) { pct_decode (*x); }
  //@}

};

//==================================================================================================
//! \struct Format::Formatter<URI>
//! \brief for using with yat::Format class
//!
//! Extended format specification:
//! [<field name>|<short field name>]\n
//! \code{.unparsed}
//! field name       ::= "scheme" | "authority" | "path" | "userinfo" | "host" |
//!                      "port" | "query" | "fragment"
//! short field name ::= "s" | "a" | "p" | "u" | "h" | "r" | "q" | "f"
//! \endcode
//! If no field name or short name is provided then full URI string is printed
//!
//! \par Usage:
//! \code{.cpp}
//! yat::URI uri(<some valid URI>);
//! yat::fcout("{}").arg(uri);            // whole uri
//! yat::fcout("{/scheme}").arg(uri);     // scheme part
//! yat::fcout("{/path}").arg(uri);       // path part
//! yat::fcout("{0:/a} {0:/p}").arg(uri); // authority and path parts
//! \endcode
//==================================================================================================
template<>
struct Format::Formatter<URI>
{
  void operator()(const URI& uri, std::ostream &oss, const Format::Context& fc)
  {
    if( fc.fmt_ext.empty() )
      oss << uri.get();
    else if( fc.fmt_ext.is_equal_no_case("scheme") || fc.fmt_ext.is_equal_no_case("s") )
      oss << uri.get(URI::SCHEME);
    else if( fc.fmt_ext.is_equal_no_case("authority") || fc.fmt_ext.is_equal_no_case("a") )
      oss << uri.get(URI::AUTHORITY);
    else if( fc.fmt_ext.is_equal_no_case("path") || fc.fmt_ext.is_equal_no_case("p") )
      oss << uri.get(URI::PATH);
    else if( fc.fmt_ext.is_equal_no_case("userinfo") || fc.fmt_ext.is_equal_no_case("u") )
      oss << uri.get(URI::USERINFO);
    else if( fc.fmt_ext.is_equal_no_case("host") || fc.fmt_ext.is_equal_no_case("h") )
      oss << uri.get(URI::HOST);
    else if( fc.fmt_ext.is_equal_no_case("port") || fc.fmt_ext.is_equal_no_case("r") )
      oss << uri.get(URI::PORT);
    else if( fc.fmt_ext.is_equal_no_case("query") || fc.fmt_ext.is_equal_no_case("q") )
      oss << uri.get(URI::QUERY);
    else if( fc.fmt_ext.is_equal_no_case("fragment") || fc.fmt_ext.is_equal_no_case("f") )
      oss << uri.get(URI::FRAGMENT);
  }
};

} // namespace

#endif