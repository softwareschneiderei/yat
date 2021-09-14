//----------------------------------------------------------------------------
// Copyright (c) 2004-2020 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2020 The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB.
// see http://omniorb.sourceforge.net/ for more about omniORB.
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


#ifndef YAT_DEBUG_REGEX
  #undef YAT_ENABLE_TRACE
  #undef YAT_ENABLE_LOG
#endif

#include <algorithm>
#include <yat/utils/Singleton.h>
#include <yat/threading/Mutex.h>
#include <yat/regex/Regex.h>
#include <cctype>

#if defined YAT_HAS_GNUREGEX
  #include <regex.h>
#else
  #include <yat/regex/impl/gnuregex.h>
#endif


namespace yat
{

typedef ::regex_t CompiledRegex;
typedef YAT_SHARED_PTR(::regex_t) CompiledRegexPtr;
typedef YAT_WEAK_PTR(::regex_t) CompiledRegexWPtr;

#if ! defined YAT_HAS_GNUREGEX
  Mutex  Regex::s_mtx;
#endif

// ============================================================================
// class RegexCache
// Simple FIFO cache
// ============================================================================
class RegexCache : public Singleton<RegexCache>
{
public:
  typedef std::map<h64_t, CompiledRegexPtr> re_map_t;
  typedef std::pair<CompiledRegexWPtr, h64_t> re_pair_t;

  //---------------------------------------------------------------------------
  static std::pair<CompiledRegexWPtr, h64_t> get(const String& pattern, int cflags, h64_t h = 0)
  {
    YAT_TRACE_STATIC("RegexCache::get");
    if( 0 == h )
      h = yat::Format("{}{}").arg(pattern).arg(cflags).get().hash64();
    YAT_LOG_STATIC("pattern: " << pattern);
    YAT_LOG_STATIC("hash: " << h);
    AutoMutex<> _lock(instance().m_mtx);
    re_map_t::iterator it = instance().m_regex_map.find(h);
    if( it != instance().m_regex_map.end() )
    {
      YAT_LOG_STATIC("Found in cache");
      return std::make_pair(it->second, h);
    }

    while( instance().m_regex_map.size() >= instance().m_cache_size )
    {
      // Cache is full
      YAT_LOG_STATIC("Cache is full");
      h64_t h = instance().m_keys_list.back();
      YAT_LOG_STATIC("Remove " << h_LRU);
      ::regfree(instance().m_regex_map[h].get());
      instance().m_regex_map.erase(h);
      instance().m_keys_list.pop_back();
    }

    CompiledRegexPtr re_ptr(new CompiledRegex);
    int rc = ::regcomp(re_ptr.get(), pattern.c_str(), cflags);
    switch( rc )
    {
      case 0:
        break;
      case REG_BADBR:
        throw yat::Exception("REGEX_ERROR", "Invalid use of back reference operator.", "yat::Regex::compile");
      case REG_BADPAT:
        throw yat::Exception("REGEX_ERROR", "Invalid use of pattern operators such as group or list.", "yat::Regex::compile");
      case REG_BADRPT:
        throw yat::Exception("REGEX_ERROR", "Invalid use of repetition operators such as using '*' as the first character.", "yat::Regex::compile");
      case REG_EBRACE:
        throw yat::Exception("REGEX_ERROR", "Un-matched brace interval operators.", "yat::Regex::compile");
      case REG_EBRACK:
        throw yat::Exception("REGEX_ERROR", "Un-matched bracket list operators.", "yat::Regex::compile");
      case REG_ECOLLATE:
        throw yat::Exception("REGEX_ERROR", "Invalid collating element.", "yat::Regex::compile");
      case REG_ECTYPE:
        throw yat::Exception("REGEX_ERROR", "Unknown character class name.", "yat::Regex::compile");
      case REG_EEND:
        throw yat::Exception("REGEX_ERROR", "Nonspecific error. This is not defined by POSIX.2.", "yat::Regex::compile");
      case REG_EESCAPE:
        throw yat::Exception("REGEX_ERROR", "Trailing backslash.", "yat::Regex::compile");
      case REG_EPAREN:
        throw yat::Exception("REGEX_ERROR", "Un-matched parenthesis group operators.", "yat::Regex::compile");
      case REG_ERANGE:
        throw yat::Exception("REGEX_ERROR", "Invalid use of the range operator; for example, the ending point of the range occurs prior to the starting point.", "yat::Regex::compile");
      case REG_ESIZE:
        throw yat::Exception("REGEX_ERROR", "Compiled pattern too big.", "yat::Regex::compile");
      case REG_ESPACE:
        throw yat::Exception("REGEX_ERROR", "The regex routines ran out of memory.", "yat::Regex::compile");
      case REG_ESUBREG:
        throw yat::Exception("REGEX_ERROR", "Invalid back reference to a subexpression.", "yat::Regex::compile");
      default:
        throw yat::Exception("REGEX_ERROR", "Unknown error.", "yat::Regex::compile");
    }
    instance().m_regex_map[h] = re_ptr;
    instance().m_keys_list.push_front(h);
    YAT_LOG_STATIC("Compiled pattern added. Cache size: " << instance().m_regex_map.size());
    return std::make_pair(CompiledRegexWPtr(re_ptr), h);
  }

  //---------------------------------------------------------------------------
  static void set_cache_size(std::size_t s)
  {
    AutoMutex<> _lock(instance().m_mtx);
    if( s < 2)
      instance().m_cache_size = 1;
    else
      instance().m_cache_size = s;
    while( instance().m_regex_map.size() >= instance().m_cache_size )
    {
      h64_t h = instance().m_keys_list.back();
      ::regfree(instance().m_regex_map[h].get());
      instance().m_regex_map.erase(h);
      instance().m_keys_list.pop_back();
    }
  }

  //---------------------------------------------------------------------------
  RegexCache() : Singleton<RegexCache>()
  {
    m_cache_size = 100;
  }

private:
  std::list<h64_t> m_keys_list; // keys list for fifo implementation
  std::size_t    m_cache_size;
  re_map_t       m_regex_map;
  Mutex          m_mtx;
};

// ============================================================================
// class Regex
// ============================================================================
typedef GenericContainer<CompiledRegexWPtr> CompiledRegexWPtrContainer;
//---------------------------------------------------------------------------
// Regex::Regex(const yat::String&, yat::Regex::Flags)
//---------------------------------------------------------------------------
Regex::Regex(const yat::String& pattern, int flags)
: m_pattern(pattern), m_flags(flags), m_cflags(0), m_re_hash(0)
{
  if( m_flags & yat::Regex::extended )
    m_cflags |= REG_EXTENDED;
  if( m_flags & yat::Regex::nosubs )
  {
    m_cflags |= REG_NOSUB;
    // check for '^' & '$'
    if( m_pattern[0] != '^' )
      m_pattern.insert(0, 1, '^');
    if( m_pattern[m_pattern.size() - 1] != '$' )
      m_pattern.append(1, '$');
  }
  if( m_flags & yat::Regex::icase )
    m_cflags |= REG_ICASE;

  priv_init();
}

//---------------------------------------------------------------------------
// Regex::Regex(const Regex& src)
//---------------------------------------------------------------------------
Regex::Regex(const Regex& src) : m_pattern(src.m_pattern), m_flags(src.m_flags),
                                 m_cflags(src.m_cflags)
{
  priv_init();
}

//---------------------------------------------------------------------------
// Regex::opetator=
//---------------------------------------------------------------------------
Regex& Regex::operator=(const Regex& src)
{
  m_pattern = src.m_pattern;
  m_flags = src.m_flags;
  m_cflags = src.m_cflags;
  priv_init();
  return *this;
}

//---------------------------------------------------------------------------
// Regex::priv_init
//---------------------------------------------------------------------------
void Regex::priv_init()
{
  // Allocate a container to hold the requested weak ptr
  m_data_uptr.reset(new CompiledRegexWPtrContainer);

  // get the weak ptr on compiled pattern & pattern hash
  RegexCache::re_pair_t wptr_hash = RegexCache::get(m_pattern, m_cflags);
  CompiledRegexWPtrContainer* container_p = dynamic_cast<CompiledRegexWPtrContainer*>(m_data_uptr.get());
  container_p->set(wptr_hash.first);
  m_re_hash = wptr_hash.second;
}

//---------------------------------------------------------------------------
// Regex::~Regex
//---------------------------------------------------------------------------
Regex::~Regex()
{
}

//---------------------------------------------------------------------------
// Regex::search
//---------------------------------------------------------------------------
bool Regex::search(const yat::String& str, yat::Regex::Match* match_p,
                   Regex::MatchFlags mflags)
{
  if( (match_p && match_p->empty()) || !match_p )
  {
    return exec(str, match_p, 0, mflags, 0);
                                       // No minimum size for a search
  }
  if( str.is_equal(match_p->m_string) )
  {
    // walking on the same string
    return exec(str, match_p, match_p->position() + match_p->length(), mflags, 0);
  }
  else
  {
    // walking on a new string
    return exec(str, match_p, 0, mflags, 0);
  }
}

//---------------------------------------------------------------------------
// Regex::match
//---------------------------------------------------------------------------
bool Regex::match(const yat::String& str, yat::Regex::Match* match_p,
                  MatchFlags mflags)
{
  return exec(str, match_p, 0, mflags, str.size());
                                // The full match must be the entire string
}

//---------------------------------------------------------------------------
// Regex::match2
//---------------------------------------------------------------------------
Regex::MatchPtr Regex::match2(const yat::String& str, MatchFlags mflags)
{
  Regex::MatchPtr match_ptr = new Regex::Match;
  exec(str, match_ptr.get(), 0, mflags, str.size());
  return match_ptr;
}

//---------------------------------------------------------------------------
// Regex::exec
//---------------------------------------------------------------------------
bool Regex::exec(const yat::String& str, yat::Regex::Match* match_p,
                 std::size_t start_pos, Regex::MatchFlags mflags,
                 std::size_t req_len)
{
#if ! defined YAT_HAS_GNUREGEX
   // Original gnu regex code (used on Windows platform) is not threadsafe
  AutoMutex<> _lock(s_mtx);
#endif

  CompiledRegexPtr re_ptr;
  CompiledRegexWPtrContainer* container_p = dynamic_cast<CompiledRegexWPtrContainer*>(m_data_uptr.get());
  re_ptr = (*container_p)->lock();
  while( !re_ptr )
  {
    container_p->set(RegexCache::get(m_pattern, m_cflags, m_re_hash).first);
    re_ptr = (*container_p)->lock();
  }

  bool res = false;
  std::size_t nmatch = 0;
  regmatch_t* regmatch_p = NULL;
  if( !(m_flags & yat::Regex::nosubs) )
  {
    nmatch = re_ptr->re_nsub + 1;
    regmatch_p = new regmatch_t[nmatch];
  }

  int eflags = 0;
  if( mflags & match_not_bol)
    eflags += REG_NOTBOL;
  if( mflags & match_not_eol)
    eflags += REG_NOTEOL;

  int rc = ::regexec(re_ptr.get(), str.c_str() + start_pos, nmatch, regmatch_p, eflags);
  if( !rc )
  { // success match
    if( m_flags & yat::Regex::nosubs )
      res = true;
    else if( ((std::size_t)regmatch_p[0].rm_eo - (std::size_t)regmatch_p[0].rm_so >= req_len )
      && ( !(mflags & match_continuous) || ((mflags & match_continuous) && (std::size_t)regmatch_p[0].rm_so == 0 )) )
    {
      if( match_p )
      {
        match_p->m_string = str;
        match_p->m_submatchs.clear();
        for( std::size_t i = 0; i < nmatch; ++i )
        {
          yat::Regex::SubMatch submatch;
          if( regmatch_p[i].rm_so != -1 )
          {
            submatch.m_pos = regmatch_p[i].rm_so + start_pos;
            submatch.m_length = regmatch_p[i].rm_eo - regmatch_p[i].rm_so;
            submatch.m_string = str;
          }
          match_p->m_submatchs.push_back(submatch);
        }
      }
      res = true;
    }
    else
      res = false;
  }
  if( regmatch_p )
    delete [] regmatch_p;
  if( match_p )
    match_p->m_ready = true;
  return res;
}

//---------------------------------------------------------------------------
// Regex::search
//---------------------------------------------------------------------------
bool Regex::search(const yat::String& str, std::vector<Match>* matchs_p,
                   MatchFlags mflags)
{
  bool is_match = false;
  std::size_t start_pos = 0;
  matchs_p->clear();
  while( true )
  {
    Regex::Match match;
    if( exec(str, &match, start_pos, mflags, 0) )
    {
      matchs_p->push_back(match);
      start_pos = match.position() + match.length();
      is_match = true;
    }
    else
      break;
  }

  return is_match;
}

//---------------------------------------------------------------------------
// Regex::search
//---------------------------------------------------------------------------
yat::String Regex::replace(const yat::String& input, const yat::String& replacement,
                           MatchFlags mflags)
{
  std::vector<Regex::Match> vmrep;
  if( replacement.find('$') != std::string::npos )
  {
    // analyse the replacement string by searching $x forms
    Regex re_rep(R"(\$[0-9&`'$])");
    re_rep.search(replacement, &vmrep);
  }

  yat::String out, suffix;
  Regex::Match m;
  std::size_t start_pos = 0;
  // $ replacements are done left-to-right
  while( search(input, &m) )
  {
    if( !(mflags & format_no_copy) )
      out.append(input.substr(start_pos, m.position() - start_pos));

    start_pos = m.position() + m.length();
    if( vmrep.size() == 0 )
      out.append(replacement);
    else
    {
      std::size_t vm_start_pos = 0;
      for( std::size_t i = 0; i < vmrep.size(); ++i )
      {
        out.append(replacement.substr(vm_start_pos, vmrep[i].position() - vm_start_pos));
        vm_start_pos = vmrep[i].position() + vmrep[i].length();
        char c = vmrep[i].str()[1];
        if( std::isdigit(c) )
        {
          std::size_t x = c - '0';
          try
          {
            out.append(m.str(x));
          }
          catch(...) {}
        }
        else
        {
          switch( c )
          {
            case '&':
              out.append(m.str());
              break;
            case '$':
              out.append("$");
              break;
            case '`':
              out.append(m.suffix());
              break;
            case '\'':
              out.append(m.prefix());
              break;
          }
        }
      }
      if( !(mflags & format_no_copy) )
        out.append(vmrep.back().suffix());
    }
    suffix = m.suffix();

    if( mflags & format_first_only )
      break;
  }
  if( !(mflags & format_no_copy) )
    out.append(suffix);

  return out;
}

// ============================================================================
// class Regex::Match
// ============================================================================
//---------------------------------------------------------------------------
// Regex::Match::operator[]
//---------------------------------------------------------------------------
const Regex::SubMatch& Regex::Match::operator[](std::size_t i) const
{
  if( i < m_submatchs.size() )
    return m_submatchs[i];
  throw yat::Exception("REGEX_ERROR", "Submatch index out of bounds.", "yat::Regex::Match::operator[]");
}

//---------------------------------------------------------------------------
// Regex::Match::first
//---------------------------------------------------------------------------
const Regex::SubMatch& Regex::Match::first() const
{
  if( 0 < m_submatchs.size() )
    return m_submatchs[0];
  throw yat::Exception("REGEX_ERROR", "No match found.", "yat::Regex::Match::str");
}

//---------------------------------------------------------------------------
// Regex::Match::last
//---------------------------------------------------------------------------
const Regex::SubMatch& Regex::Match::last() const
{
  if( 0 < m_submatchs.size() )
    return m_submatchs[m_submatchs.size() - 1];
  throw yat::Exception("REGEX_ERROR", "No match found.", "yat::Regex::Match::str");
}

//---------------------------------------------------------------------------
// Regex::Match::begin
//---------------------------------------------------------------------------
Regex::Match::iterator Regex::Match::begin() const
{
  Regex::Match::iterator it;
  it.m_submatchs_p = &m_submatchs;
  return it;
}

//---------------------------------------------------------------------------
// Regex::Match::end
//---------------------------------------------------------------------------
Regex::Match::iterator Regex::Match::end() const
{
  Regex::Match::iterator it;
  it.m_submatchs_p = &m_submatchs;
  if( it.m_submatchs_p )
    it.m_idx = m_submatchs.size();
  return it;
}

//---------------------------------------------------------------------------
// Regex::Match::operator[]
//---------------------------------------------------------------------------
yat::String Regex::Match::str(std::size_t i) const
{
  if( i < m_submatchs.size() )
    return m_submatchs[i].str();
  throw yat::Exception("REGEX_ERROR", "submatch index out of bounds.", "yat::Regex::Match::str");
}

//---------------------------------------------------------------------------
// Regex::Match::position
//---------------------------------------------------------------------------
std::size_t Regex::Match::position(std::size_t i) const
{
  if( i < m_submatchs.size() )
    return m_submatchs[i].position();
  throw yat::Exception("REGEX_ERROR", "Submatch index out of bounds.", "yat::Regex::Match::position");
}

//---------------------------------------------------------------------------
// Regex::Match::length
//---------------------------------------------------------------------------
std::size_t Regex::Match::length(std::size_t i) const
{
  if( i < m_submatchs.size() )
    return m_submatchs[i].length();
  throw yat::Exception("REGEX_ERROR", "Submatch index out of bounds.", "yat::Regex::Match::length");
}

//---------------------------------------------------------------------------
// Regex::Match::prefix
//---------------------------------------------------------------------------
yat::String Regex::Match::prefix() const
{
  if( 0 < m_submatchs.size() )
    return m_string.substr(0, m_submatchs[0].position());
  throw yat::Exception("REGEX_ERROR", "No match found.", "yat::Regex::Match::prefix");
}

//---------------------------------------------------------------------------
// Regex::Match::suffix
//---------------------------------------------------------------------------
yat::String Regex::Match::suffix() const
{
  if( 0 < m_submatchs.size() )
    return m_string.substr(m_submatchs[0].position() + m_submatchs[0].length());
  throw yat::Exception("REGEX_ERROR", "No match found.", "yat::Regex::Match::prefix");
}

//---------------------------------------------------------------------------
// Regex::Match::iterator::operator*
//---------------------------------------------------------------------------
const Regex::SubMatch& Regex::Match::iterator::operator*() const
{
  if( m_submatchs_p )
    return (*m_submatchs_p)[m_idx];
  throw yat::Exception("OUT_OF_BOUNDS", "No such item", "yat::Regex::Match::iterator::operator*");
}

//---------------------------------------------------------------------------
// Regex::Match::iterator::operator->
//---------------------------------------------------------------------------
const Regex::SubMatch* Regex::Match::iterator::operator->() const
{
  if( m_submatchs_p )
    return &(*m_submatchs_p)[m_idx];
  throw yat::Exception("OUT_OF_BOUNDS", "No such item", "yat::Regex::Match::iterator::operator*");
}

//---------------------------------------------------------------------------
// Regex::Match::iterator::operator++
//---------------------------------------------------------------------------
Regex::Match::iterator& Regex::Match::iterator::operator++()
{
  if( m_submatchs_p->size() > m_idx )
    ++m_idx;
  return *this;
}

//---------------------------------------------------------------------------
// Regex::Match::iterator::operator++(int)
//---------------------------------------------------------------------------
Regex::Match::iterator Regex::Match::iterator::operator++(int)
{
  Regex::Match::iterator it = *this;
  ++(*this);
  return it;
}

//---------------------------------------------------------------------------
// operator!=(const Regex::Match::iterator&, const Regex::Match::iterator&)
//---------------------------------------------------------------------------
bool Regex::Match::iterator::operator!=(const Regex::Match::iterator& other) const
{
  return (m_idx != other.m_idx || m_submatchs_p != other.m_submatchs_p);
}

//---------------------------------------------------------------------------
// operator==(const Regex::Match::iterator&, const Regex::Match::iterator&)
//---------------------------------------------------------------------------
bool Regex::Match::iterator::operator==(const Regex::Match::iterator& other) const
{
  return (m_idx == other.m_idx && m_submatchs_p == other.m_submatchs_p);
}

//---------------------------------------------------------------------------
// Regex::set_cache_size
//---------------------------------------------------------------------------
void Regex::set_cache_size(std::size_t s)
{
  if( s < 2 )
    RegexCache::set_cache_size(1);
  else
    RegexCache::set_cache_size(s);
}

}
