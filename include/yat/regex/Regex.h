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

#include <vector>
#include <yat/memory/SharedPtr.h>
#include <yat/utils/String.h>

#if defined YAT_HAS_GNUREGEX
  #include <regex.h>
#else
  #include <yat/regex/impl/gnuregex.h>
#endif


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

  //! c-tor
  Regex(const yat::String& regex, CompFlags flags = extended);
  Regex(const Regex& src);

  //! d-tor
  ~Regex();

  //! Returns the pattern
  const yat::String& pattern() const { return m_pattern; }

  //! Check if whole string match the regex
  //! \param str string to match
  //! \param match_p result object, may be null ptr if one not interested on sub-matchs
  bool match(const yat::String& str, Match* match_p = NULL, MatchFlags mflags = match_default);

  //! Check if whole string match the regex
  //! \param str string to match
  //! \param match_p result object, may be null ptr if one not interested on sub-matchs
  //! \return sharedptr on a match result object
  MatchPtr match(const yat::String& str, MatchFlags mflags = match_default);

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

  //! Compile the Regex, this is not required to manually call this method since
  //! compiling is automatically performed when needed
  void compile();

private:
  bool exec(const yat::String& str, Match* match_p, std::size_t start_pos,
            Regex::MatchFlags mflags, std::size_t req_len);

  yat::String m_pattern;
  CompFlags   m_flags;
  bool        m_compiled;
  ::regex_t   m_regex;
};

typedef SharedPtr<Regex> RegexPtr;

} // namespace

#endif
