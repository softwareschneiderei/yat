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

#ifndef __YAT_REGEX_H__
#define __YAT_REGEX_H__

// ============================================================================
//! \page RegexPage Regex documentation
//! \tableofcontents
//! The Regex class is a class wrapper on top on the native GNU regex functions
//! (Linux), on the integrated gnu implementation (Windows)
//!
//! \section secRE1 yat::Regex
//! This main class manage a regular expression and allow to:
//!  - check if a string exactly match the regex ('match' method)
//!  - found each match of the regex in a given string ('search' method)
//!  - search and replace regex matches by a replacement string ('replace' method)
//! The compiling step is perform automatically the first time one of the three
//! method is called
//! Currently supported regex grammar are BRE (Basic Regular Expression) & ERE
//! Extended Regular Expression). The later is the default.
//! BRE/ERE syntax can be found here:
//! https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_04
//! or here:
//! https://en.wikibooks.org/wiki/Regular_Expressions/POSIX-Extended_Regular_Expressions
//!
//! \section secRE2 yat::Regex::Match
//! 'match' & 'search' function result is stored in a yat::Regex::Match object
//! this object contains a yat::Regex::SubMatch for each resulting submatch
//! an yat::Match object that containing no submatches means the input string
//! does not match the regular expression
//!
//! \par Match example:
//! \code{.cpp}
//!   yat::Regex re("([^/]+)/([^/]+)/([^/]+)");
//!   yat::Regex::Match m;
//!   yat::String dev_name = "my/outstanding/device";
//!   if( re.match(dev_name, &m) )
//!   {
//!     std::cout << "full match: " << m.str(0) << std::endl;
//!     std::cout << "domain: " << m.str(1) << std::endl;
//!     std::cout << "family: " << m.str(2) << std::endl;
//!     std::cout << "member: " << m.str(3) << std::endl;
//!   }
//!   else
//!     std::cout << "This is not a device name!" << std::cout;
//! \endcode
//! Should output on the terminal:
//! \code{.unparsed}
//! $ full match: my/outstanding/device
//! $ domain: my
//! $ family: outstanding
//! $ member: device
//! \endcode
//!
//! \par Search example:
//! \code{.cpp}
//!   yat::Regex re(R"(\w+)");
//!   yat::Regex::Match m;
//!   while( re.search("What a beautiful day!", &m) )
//!   {
//!     std::cout << "found word: " << m.str() << std::endl;
//!   }
//! \endcode
//! Should output on the terminal:
//! \code{.unparsed}
//! $ found word: what
//! $ found word: a
//! $ found word: beautiful
//! $ found word: day
//! \endcode
//!
//! \par Replace Example:
//! \code{.cpp}
//!   yat::Regex re(R"(\w+)");
//!   yat::String str = re.replace("ga bu zo meu", "[$0]");
//!   std::cout << str << std::endl;
//! \endcode
//! Should output on the terminal:
//! \code{.unparsed}
//! $ [ga] [bu] [zo] [meu]
//! \endcode
//!
//! \section secRE3 Regex classes
//! Links to regex classes :
//!   - yat::Regex
//!   - yat::Regex::Match
//!   - yat::Regex::SubMatch
//!   - yat::Regex::Match::iterator
// ============================================================================

#include <vector>
#include <yat/memory/SharedPtr.h>
#include <yat/memory/UniquePtr.h>
#include <yat/utils/String.h>
#include <yat/any/GenericContainer.h>

namespace yat
{

// ============================================================================
//! \class Regex
//! \brief Wrapper over the GNU Regular expression processor
//! \verbatim
//! yat::Regex re ="foo-*(bar)?";
//! yat::Regex::Match m;
//! bool match = re.match("foo--bar", &m);
//! \endverbatim
// ============================================================================
class YAT_DECL Regex
{
public:

  enum CompFlags
  {
    basic = 1,
    extended = 2,
    icase = 4,
    nosubs = 8
  };

  enum MatchFlags
  {
    match_default = 0,
    match_not_bol = 1,
    match_not_eol = 2,
    match_continuous = 4,
    format_no_copy = 8,    // Regex::replace only
    format_first_only = 16 // Regex::replace only
  };

  //! Sub-match result
  class SubMatch
  {
    friend class Regex;
  public:
    //! returns the match string
    yat::String str() const { return m_string.substr(m_pos, m_length); }

    //! returns the match starting position
    std::size_t position() const { return m_pos; }

    //! returns the match length
    std::size_t length() const { return m_length; }

    operator const yat::String() const { return str(); }
    operator const std::string() const { return str().str(); }

  private:
    SubMatch():m_pos(0), m_length(0) {} // no one is allowed to instantiate a Match object, except the Regex
    std::size_t m_pos;
    std::size_t m_length;
    yat::String m_string;
  };

  class Match
  {
    friend class Regex;
  public:

    //! forward iterator over submatchs
    class iterator
    {
      friend class Match;
    public:
      iterator& operator++();
      iterator operator++(int);
      const SubMatch& operator*() const;
      const SubMatch* operator->() const;
      bool operator!=(const iterator& other) const;
      bool operator==(const iterator& other) const;

    private:
      iterator() : m_idx(0), m_submatchs_p(0) {}
      std::size_t m_idx;
      const std::vector<SubMatch>* m_submatchs_p;
    };

    //! returns sub match
    const SubMatch& operator[](std::size_t i) const;

    //! returns first sub match
    const SubMatch& first() const;

    //! return last submatch
    const SubMatch& last() const;

    //! returns an iterator to the beginning
    iterator begin() const;

    //! returns an iterator to the end
    iterator end() const;

    //! returns sub match string
    yat::String str(std::size_t i = 0) const;

    //! returns sub match mosition
    std::size_t position(std::size_t i = 0) const;

    //! returns sub match length
    std::size_t length(std::size_t i = 0) const;

    //! returns string part before full match
    yat::String prefix() const;

    //! returns string part after full match
    yat::String suffix() const;

    //! returns sub match count, zero means no match
    std::size_t size() const { return  m_submatchs.size(); }

    //! checks whether the match was successful
    bool empty() const { return m_submatchs.size() ? false : true; }

  private:
    std::vector<SubMatch> m_submatchs;
    yat::String           m_string;
  };

  typedef SharedPtr<Match> MatchPtr;

  //! default c-tor
  Regex(const yat::String& regex, int flags = extended);

  //! copy c-tor
  Regex(const Regex& src);

  //! operator=
  Regex& operator=(const Regex& src);

  //! d-tor
  ~Regex();

  //! Returns the pattern
  const yat::String& pattern() const { return m_pattern; }

  //! Check if whole string match the regex and fill match object
  //! \param str string to match
  //! \param match_p result object, may be null ptr if one not interested on sub-matchs
  //! \param mflags match flags
  //! \return true/false
  bool match(const yat::String& str, Match* match_p = NULL, MatchFlags mflags = match_default);

  //! Check if whole string match the regex and return a match object
  //! \param str string to match
  //! \param match_p result object, may be null ptr if one not interested on sub-matchs
  //! \param mflags match flags
  //! \return sharedptr on a match result object
  MatchPtr match2(const yat::String& str, MatchFlags mflags = match_default);

  //! Search for next match in the given string
  //! \param str matching string
  //! \param result object
  //! \param start_pos starting poition of searching
  bool search(const yat::String& str, Match* match_p = NULL, MatchFlags mflags = match_default);

  //! Search for all match in the given string
  //! \param str matching string
  //! \param result_p objects
  bool search(const yat::String& str, std::vector<Match>* match_p, MatchFlags mflags = match_default);

  //! Replace pieces of text matching the regex in the input string
  //! \param input string to process
  //! \param replacement matching pieces replacement
  //! \return output string
  yat::String replace(const yat::String& input, const yat::String& replacement, MatchFlags mflags = match_default);

  //! Set the regex cache size (1 or 0 means no cache)
  //! \param s maximum number of items in cache
  static void set_cache_size(std::size_t s);

private:
  bool exec(const yat::String& str, Match* match_p, std::size_t start_pos,
            Regex::MatchFlags mflags, std::size_t req_len);

  void priv_init();

  yat::String   m_pattern;
  int           m_flags;
  int           m_cflags;
  h64_t         m_re_hash;
  ContainerUPtr m_data_uptr; // This is to avoid dependency on regex impl

#if ! defined YAT_HAS_GNUREGEX
  static Mutex  s_mtx; // Original gnu regex code is not threadsafe
#endif
};

typedef SharedPtr<Regex> RegexPtr;
typedef UniquePtr<Regex> RegexUPtr;

} // namespace

#endif
