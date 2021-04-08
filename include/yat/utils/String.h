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
// Copyright (C) 2006-2021 The Tango Community
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

#ifndef __YAT_STRING_H__
#define __YAT_STRING_H__

#include <sstream>
#include <iostream>
#include <cctype>
#include <yat/CommonHeader.h>
#include <yat/utils/NonCopyable.h>
#include <yat/Exception.h>

// STL headers
#include <string>
#include <vector>
#include <deque>
#include <set>
#include <list>
#include <algorithm>
#include <iomanip>
#include <bitset>

#if defined YAT_WIN32 && _MSC_VER > 1200
    // Deprecated POSX definitions with VC8+
    #define stricmp   _stricmp
    #define strnicmp  _strnicmp
    #define mkdir     _mkdir
#endif

/// Cast a string object to a const char *
#define PSZ(s) (s).c_str()

#if defined YAT_LINUX || defined YAT_WIN32 && _MSC_VER > 1200
  // Variadic macro supported starting in C99
  #define PSZ_FMT(...) yat::StringUtil::str_format(__VA_ARGS__).c_str()
  #define YAT_STR_FMT(...) yat::StringUtil::str_format(__VA_ARGS__)
#endif

// vsnprintf function
#if defined YAT_WIN32
  #define VSNPRINTF(buf, len, fmt, arg) _vsnprintf_s(buf, len, _TRUNCATE, fmt, arg)
#else
  #define VSNPRINTF vsnprintf
#endif


#if defined YAT_LINUX
  int stricmp(const char *s1, const char *s2);
  int strnicmp(const char *s1, const char *s2, int maxlen);
#endif

namespace yat
{

typedef const char *   pcsz;
typedef char *         psz;

class String;

// ============================================================================
//! \class StringUtil
//! \brief Manipulations on a std::string objects.
//! \verbatim
//! std::string str =" foo-bar ";
//! yat::StringUtil::trim(&str);
//! std::out << str;
//! \endverbatim
// ============================================================================
class YAT_DECL StringUtil : public yat::NonCopyable
{
public:

  //! \brief Empty string - useful when need a const string &.
  static const std::string empty;

  //! \deprecated
  static std::string str_format(pcsz pszFormat, ...);

  //! \brief Compares std::strings (i.e. operator==).
  //!
  //! Returns true if std::strings are equal, false otherwise.
  //! \param str The source std::string.
  static bool is_equal(const std::string& str, const std::string& other);

  //! \brief Compares std::string in a no case sensitive way.
  //!
  //! Returns true if std::strings are equal, false otherwise.
  //! \param str The source std::string.
  static bool is_equal_no_case(const std::string& str, const std::string& other);

  //! \brief Tests the first character.
  //!
  //! Returns true if the std::string starts with this character, false otherwise.
  //! \param c The character.
  static bool starts_with(const std::string& str, char c);

  //! \brief Tests the first character with that of another std::string.
  //!
  //! Returns true if the std::strings start with the same character, false otherwise.
  //! \param pcszStart The source std::string.
  //! \param bNoCase If set to true, the test is not case sensitive.
  static bool starts_with(const std::string& str, pcsz pcszStart, bool bNoCase=false);

  //! \brief Tests the last character.
  //!
  //! Returns true if the std::string ends with this character, false otherwise.
  //! \param c The character.
  static bool ends_with(const std::string& str, char c);

  //! \brief Tests the last character with that of another std::string.
  //!
  //! Returns true if the std::strings end with the same character, false otherwise.
  //! \param pcszEnd The source std::string.
  //! \param bNoCase If set to true, the test is not case sensitive.
  static bool ends_with(const std::string& str, pcsz pcszEnd, bool bNoCase=false);

  //! \name Token extraction
  //@{

  //! \brief Family results for token extraction methods.
  enum ExtractTokenRes
  {
    //! Nothing extracted.
    EMPTY_STRING=0,
    //! %std::string extracted and separator found.
    SEP_FOUND,
    //! %std::string extracted and separator not found.
    SEP_NOT_FOUND
  };

  //! \brief Looks for a token, from left to right.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first separator occurrence. The extracted token is removed from the std::string.
  //! \param c Separator.
  //! \param[out] pstrToken %std::string object receiving the extracted token.
  //! \param apply_escape Take care of '\' before c to escape it
  static ExtractTokenRes extract_token(std::string* str_p, char c, std::string *pstrToken,
                                       bool apply_escape = false);

  //! \brief Looks for a token, from right to left.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first separator occurrence. The extracted token is removed from the std::string.
  //! \param c Separator.
  //! \param[out] pstrToken Extracted token.
  //! \param apply_escape Take care of '\' before c to escape it
  static ExtractTokenRes extract_token_right(std::string* str_p, char c, std::string *pstrToken,
                                             bool apply_escape = false);

  //! \brief Looks for enclosed token, from left to right.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first left separator occurrence. The extracted token is removed from the std::string.
  //! \param cLeft Left separator.
  //! \param cRight Right separator.
  //! \param[out] pstrToken Extracted token.
  //! \param apply_escape Take care of '\' before cLeft/cRight to escape them
  static ExtractTokenRes extract_token(std::string* str_p, char cLeft, char cRight,
                                       std::string *pstrToken, bool apply_escape = false);

  //! \brief Looks for enclosed token, from right to left.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first left separator occurrence. The extracted token is removed from the std::string.
  //! \param cLeft Left separator.
  //! \param cRight Right separator.
  //! \param[out] pstrToken Extracted token.
  //! \param apply_escape Take care of '\' before cLeft/cRight to escape them
  static ExtractTokenRes extract_token_right(std::string* str_p, char cLeft, char cRight,
                                             std::string *pstrToken, bool apply_escape = false);

  //@}

  //! \brief Removes characters that enclose std::string: quotes, parenthesis, etc...
  //!
  //! Returns true if enclosure was removed.
  //! ex: RemoveEnclosure("'", "'") -\> removes quotes in a std::string like 'std::string'.
  //! ex: RemoveEnclosure("(\[", ")\]") -\> removes parenthesis in a std::string like (std::string) or \[std::string\]
  //! but not in std::string like (std::string\].
  //! \param pszLeft List of possible left enclosure characters.
  //! \param pszRight List of possible right enclosure characters.
  //! \remark \<pszLeft\> and \<pszRight\> must have the same length.
  static bool remove_enclosure(std::string* str_p, psz pszLeft, psz pszRight);

  //! \brief Removes characters that enclose std::string: quotes, parenthesis, etc...
  //!
  //! Returns true if enclosure was removed.
  //! ex: RemoveEnclosure("'", "'") -\> removes quotes in a std::string like 'std::string'.
  //! \param cLeft Left enclosure character.
  //! \param cRight Right enclosure character.
  static bool remove_enclosure(std::string* str_p, char cLeft, char cRight);

  //! \brief Tests if std::string matches with mask containing '*' and '?' jokers.
  //!
  //! Returns true if std::string matches the mask, false otherwise. \n
  //! The mask can contain:
  //! - '*': match any number of characters.
  //! - '?': match one character.
  //! \param pszMask The mask.
  //! \param pvectokens elements that match '*' and '?' can be pushed in a vector
  static bool match(const std::string& str, pcsz pszMask, std::vector<std::string> *pvectokens=0);

  //! \brief Tests if std::string matches with mask containing '*' and '?' jokers.
  //!
  //! Returns true if std::string matches the mask, false otherwise. \n
  //! The mask can contain:
  //! - '*': match any number of characters.
  //! - '?': match one character.
  //! \param pvectokens elements that match '*' and '?' can be pushed in a vector
  //! \param strMask The mask.
  static bool match(const std::string& str, const std::string &strMask, std::vector<std::string> *pvectokens=0);

  //! \brief Removes white spaces at beginning and end of string.
  static void trim(std::string* str_p);

  //! \brief Removes white spaces at beginning of string.
  static void trim_left(std::string* str_p);

  //! \brief Removes white spaces at end of string.
  static void trim_right(std::string* str_p);

  //! \brief Removes white spaces at beginning and end of all strings in collection.
  static void trim(std::vector<std::string>* vec_p);

  //! \brief Removes white spaces at end of all strings in collection.
  static void trim_right(std::vector<std::string>* vec_p);

  //! \brief Removes white spaces at beginning of all strings in collection.
  static void trim_left(std::vector<std::string>* vec_p);

  //! \brief Removes white spaces at beginning and end of all strings in collection.
  static void trim(std::vector<yat::String>* vec_p);

  //! \brief Removes white spaces at end of all strings in collection.
  static void trim_right(std::vector<yat::String>* vec_p);

  //! \brief Removes white spaces at beginning of all strings in collection.
  static void trim_left(std::vector<yat::String>* vec_p);

  //! \deprecated
  static int printf(std::string* str_p, pcsz pszFormat, ...);

  //! \brief Splits the std::string.
  //!
  //! The std::string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the std::string vector, while *this*
  //! std::string is preserved.
  //! \param c Separator.
  //! \param[out] pvecstr Pointer to a vector of std::strings.
  //! \param clear_coll If set to true, the vector is cleared.
  static void split(std::string* str_p, char c, std::vector<std::string> *pvecstr,
                    bool clear_coll=true);
  static void split(std::string* str_p, char c, std::vector<String> *pvecstr,
                    bool clear_coll=true);

  //! \brief Splits the std::string.
  //!
  //! The std::string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the std::string vector, while *this*
  //! std::string ispreserved.
  //! \param c Separator.
  //! \param[out] pvecstr Pointer to a vector of std::strings.
  //! \param clear_coll If set to true, the vector is cleared.
  static void split(const std::string& str, char c, std::vector<std::string> *pvecstr,
                    bool clear_coll=true);
  static void split(const std::string& str, char c, std::vector<String> *pvecstr,
                    bool clear_coll=true);

  //! \brief Splits the std::string.
  //!
  //! The std::string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the std::string deque, while *this*
  //! std::string is preserved.
  //! \param c Separator.
  //! \param[out] p_deque Pointer to a vector of std::strings.
  //! \param clear_coll If set to true, the deque is cleared first.
  static void split(std::string* str_p, char c, std::deque<std::string> *p_deque,
                    bool clear_coll=true);
  static void split(std::string* str_p, char c, std::deque<String> *p_deque,
                    bool clear_coll=true);

  //! \brief Splits the std::string.
  //!
  //! The std::string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the std::string deque, while *this*
  //! std::string is preserved.
  //! \param c Separator.
  //! \param[out] p_deque Pointer to a deque of std::strings.
  //! \param clear_coll If set to true, the deque is cleared first.
  static void split(const std::string& str, char c, std::deque<std::string> *p_deque,
                    bool clear_coll=true);
  static void split(const std::string& str, char c, std::deque<String> *p_deque,
                    bool clear_coll=true);

  //! \brief Splits the std::string.
  //!
  //! The std::string is split in 2 tokens separated with the specified separator.
  //!
  //! \param c Separator.
  //! \param[out] pstrLeft Left part of the split std::string.
  //! \param[out] pstrRight Right part of the split std::string.
  //! \param trim If set to true, the trim function is applied on left & right parts
  static void split(const std::string& str, char c, std::string *pstrLeft,
                    std::string *pstrRight, bool trim = false);

  //! \brief Splits the std::string.
  //!
  //! The std::string is split in 2 tokens separated with the specified separator.
  //!
  //! \param c Separator.
  //! \param[out] pstrLeft Left part of the split std::string.
  //! \param[out] pstrRight Right part of the split std::string.
  //! \param trim If set to true, the trim function is applied on left & right parts
  static void split(std::string* str_p, char c, std::string *pstrLeft,
                    std::string *pstrRight, bool trim = false);

  //! \brief Joins std::strings from a std::string vector, using specified separator.
  //!
  //! Replaces *this* std::string with the result.
  //! For instance: join (\<str1, str2\>, ";") gives: str1;str2
  //! \param vecStr The source vector.
  //! \param cSep %std::string separator.
  static void join(std::string* str_p, const std::vector<std::string> &vecStr,
                   char cSep=',');
  static void join(std::string* str_p, const std::vector<String> &vecStr,
                   char cSep=',');

  //! \brief Joins std::strings from a std::string vector, using specified separator.
  //!
  //! Replaces *this* std::string with the result.
  //! For instance: join (\<str1, str2\>, ";") gives: str1;str2
  //! \param vecStr The source vector.
  //! \param cSep %std::string separator.
  //! \return a std::string with the result
  static std::string join(const std::vector<std::string> &vecStr, char cSep=',');
  static std::string join(const std::vector<String> &vecStr, char cSep=',');

  //! \brief Joins std::strings from a std::string vector, using specified separator.
  //!
  //! Replaces *this* std::string with the result.
  //! For instance: join (\<str1, str2\>, ";") gives: str1;str2
  //! \param deque The source deque.
  //! \param cSep %std::string separator.
  static void join(std::string* str_p, const std::deque<std::string> &deque,
                   char cSep=',');

  //! \brief Joins std::strings from a std::string vector, using specified separator.
  //!
  //! Replaces *this* std::string with the result.
  //! For instance: join (\<str1, str2\>, ";") gives: str1;str2
  //! \param deque The source deque.
  //! \param cSep %std::string separator.
  //! \return a std::string with the result
  static std::string join(const std::deque<std::string> &deque, char cSep=',');

  //! \brief Removes items separated by a specific separator.
  //!
  //! For instance: std::string like "item1,item2,item3,...". \n
  //! Returns true if any item was found, false otherwise.
  //! \param strItem Item to find and remove.
  //! \param cSep Items separator.
  static bool remove_item(std::string* str_p, const std::string &strItem,
                          char cSep=',');

  //! \brief Converts std::string to lowercase.
  static void to_lower(std::string* str_p);

  //! \brief Converts std::string to uppercase.
  static void to_upper(std::string* str_p);

  //! \brief Finds and replaces a std::string.
  //! \param pszSrc %std::string to replace.
  //! \param pszDst Substitution std::string.
  static void replace(std::string* str_p, pcsz pszSrc, pcsz pszDst);

  //! \brief Finds and replaces one character.
  //!
  //! \param cSrc Character to replace.
  //! \param cDst Substitution character.
  static void replace(std::string* str_p, char cSrc, char cDst);

  //! \brief Substitute occurences of characters by a single one
  //!
  //! \param pszCharSet Characters set to substitute.
  //! \param cDst Substitution character.
  static void substitute(std::string* str_p, pcsz pszCharSet, char cReplacement);

  //! \brief Remove occurences of characters
  //!
  //! \param pszCharSet Characters set to remove.
  //! \param cDst Substitution character.
  static void remove(std::string* str_p, pcsz pszCharSet);

  //! \brief Returns a 32 bits hash code.
  static uint32 hash(const std::string& str);

  //! \brief Returns the hash code using the FNV-1a algorithm.
  //!
  //! Calculates a 64 bits hash code. See details of the algorithm
  //! on http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function.
  static uint64 hash64(const std::string& str);

  //! \brief Converts string content to numeric type \<_T\>.
  //!
  //! Returns the converted string in numeric type \<_T\>.
  //! Should also work for any "istringstream::operator>>" supported type.
  //! \param _s The string to convert.
  //! \param _throw If set to true, throws an exception if conversion fails.
  //! \exception SOFTWARE_ERROR Thrown if conversion fails.
  template <class T>
  static T to_num(const std::string& str, bool _throw = true)
  {
    ISStream iss(str.c_str());

    T num_val;
    iss >> num_val;

    if( iss.fail() )
    {
      if( _throw )
      {
        OSStream desc;
        desc << "conversion from string to num failed ["
             << str
             << "]"
             << std::ends;
        THROW_YAT_ERROR ("SOFTWARE_ERROR",
                         desc.str().c_str(),
                         "StringUtil::to_num");
      }
      return 0;
    }

    return num_val;
  }

  //! \brief Converts from type \<T\> to std::string.
  //!
  //! \param number The \<T\> type value to convert.
  //! \param _throw If set to true, throws an exception if conversion fails.
  //! \exception SOFTWARE_ERROR Thrown if conversion fails.
  template <class T>
  static void from_num(std::string* str_p, const T& number, bool _throw = true)
  {
    OSStream oss;

    if ( (oss << std::fixed << number) == false )
    {
      if (_throw)
      {
        OSStream desc;
        desc << "conversion from num to string failed ["
             << number
             << "]"
             << std::ends;
        THROW_YAT_ERROR ("SOFTWARE_ERROR",
                         desc.str().c_str(),
                         "StringUtil::from_num");
      }
    }

    (*str_p) = oss.str();
  }

  //! \brief Return a new string from numeric type \<T\> value.
  //!
  //! \param number The \<T\> type value to convert.
  //! \param _throw If set to true, throws an exception if conversion fails.
  //! \exception SOFTWARE_ERROR Thrown if conversion fails.
  template <class T>
  static std::string to_string(const T& number, bool _throw = true)
  {
    OSStream oss;

    if ( !(oss << std::fixed << number) )
    {
      if (_throw)
      {
        OSStream desc;
        desc << "conversion from num to string failed ["
             << number
             << "]"
             << std::ends;
        THROW_YAT_ERROR ("SOFTWARE_ERROR",
                         desc.str().c_str(),
                         "StringUtil::from_num");
      }
    }

    return oss.str();
  }

  //! \name Deprecated
  //@{
  static bool start_with(const std::string& str, char c)
  { return starts_with(str, c); }
  static bool start_with(const std::string& s, pcsz p, bool b=false)
  { return starts_with(s, p, b); }
  static bool end_with(const std::string& str, char c)
  { return ends_with(str, c); }
  static bool end_with(const std::string& s, pcsz p, bool b=false)
  { return ends_with(s, p, b); }
  //@}
};

//! Synonyms
#define extract_token_left extract_token

//! Usefull type
typedef std::vector<std::string> StringVector;

// ============================================================================
//! \class String
//! \brief Extended string class.
//!
//! This class is an extension of the std::string class: it provides additional
//! string manipulation functions, such as token extraction, enclosure deletion,
//! find and replace function, ...
// ============================================================================
class YAT_DECL String
{
public:

  //! \brief Empty string - useful when need a const string &.
  static const String nil;

  //! Return underlying object
  const std::string& str() const { return m_str; }
  std::string& str() { return m_str; }

  //! Explicit conversion
  operator std::string&() { return m_str; }
  operator const std::string&() const { return m_str; }

  //! \name std::string definitions & methods mapping
  //@{

  typedef char value_type;
  typedef std::allocator<char> Allocator;
  typedef Allocator::size_type size_type;
  typedef Allocator::reference reference;
  typedef Allocator::const_reference const_reference;
  typedef std::string::iterator iterator;
  typedef std::string::reverse_iterator reverse_iterator;
  typedef std::string::const_iterator const_iterator;
  typedef std::string::const_reverse_iterator const_reverse_iterator;

  String& operator=(pcsz sz_p) { m_str = sz_p; return *this; }
  String& operator=(char c)    { m_str = c; return *this; }
  String& assign(std::string::size_type cnt, char ch) { m_str.assign(cnt, ch); return *this; }
  String& assign(const std::string& str) { m_str.assign(str); return *this; }
  String& assign(const char* p, size_type cnt) { m_str.assign(p, cnt); return *this; }
  String& assign(const char* p) { m_str.assign(p); return *this; }
  reference at(size_type pos) { return m_str.at(pos); }
  const_reference at(size_type pos) const { return m_str.at(pos); }
  reference operator[](size_type pos) { return m_str[pos]; }
  const_reference operator[](size_type pos) const { return m_str[pos]; }
  const char* data() const { return m_str.data(); }
  const char* c_str() const { return m_str.c_str(); }
  iterator begin() { return m_str.begin(); }
  const_iterator begin() const { return m_str.begin(); }
  reverse_iterator rbegin() { return m_str.rbegin(); }
  const_reverse_iterator rbegin() const { return m_str.rbegin(); }
  iterator end() { return m_str.end(); }
  const_iterator end() const { return m_str.end(); }
  reverse_iterator rend() { return m_str.rend(); }
  const_reverse_iterator rend() const { return m_str.rend(); }
  bool empty() const { return m_str.empty(); }
  size_type size() const { return m_str.size(); }
  size_type length() const { return m_str.length(); }
  size_type max_size() const { return m_str.max_size(); }
  size_type capacity() const { return m_str.capacity(); }
  void reserve(size_type n = 0) { m_str.reserve(n); }
  void clear() { m_str.clear(); }
  String& insert(size_type idx, size_type cnt, char c) { m_str.insert(idx, cnt, c); return *this; }
  String& insert(size_type idx, const char* p) { m_str.insert(idx, p);  return *this; }
  String& insert(size_type idx, const char* p, size_type n) { m_str.insert(idx, p, n); return *this; }
  String& insert(size_type idx, const std::string& str) { m_str.insert(idx, str); return *this; }
  String& insert(size_type idx, const String& str) { m_str.insert(idx, str.m_str); return *this; }
  iterator insert(iterator pos, char c) { return m_str.insert(pos, c); }
  String& erase(size_type idx = 0, size_type cnt = std::string::npos) { m_str.erase(idx, cnt);  return *this; }
  iterator erase(iterator pos) { return m_str.erase(pos); }
  iterator erase(iterator first, iterator last) { return m_str.erase(first, last); }
  void push_back(char c) { m_str.push_back(c); }
  String& append(size_type cnt, char c) { m_str.append(cnt, c);return *this; }
  String& append(const std::string& str) { m_str.append(str); return *this; }
  String& append(const String& str) { m_str.append(str.m_str); return *this; }
  String& append(const String& str, size_type pos, size_type n) { m_str.append(str.m_str, pos, n); return *this;}
  String& append(const char*p, size_type n) { m_str.append(p, n); return *this; }
  String& append(const char*psz) { m_str.append(psz); return *this;}
  String& operator+=(const std::string& str) { m_str += str; return *this; }
  String& operator+=(const String& str) { m_str += str.m_str; return *this; }
  String& operator+=(const char* psz) { m_str += psz; return *this; }
  String& operator+=(char c) { m_str += c; return *this; }
  int compare(const std::string& str) const { return m_str.compare(str); }
  int compare(const String& str) const { return m_str.compare(str.m_str); }
  int compare(size_type p1, size_type n1, const std::string& str, size_type p2,
              size_type n2) const
                            { return m_str.compare(p1, n1, str, p2, n2); }
  int compare(size_type p1, size_type n1, const String& str, size_type p2,
              size_type n2) const
                            { return m_str.compare(p1, n1, str.m_str, p2, n2); }
  int compare(const char* psz) const { return m_str.compare(psz); }
  int compare(size_type p1, size_type n1, const char* psz) const
                                          { return m_str.compare(p1, n1, psz); }
  int compare(size_type p1, size_type n1, const char* psz, size_type n2) const
                                      { return m_str.compare(p1, n1, psz, n2); }
  String& replace(size_type pos, size_type n, const std::string& str)
                                   { m_str.replace(pos, n, str); return *this; }
  String& replace(size_type pos, size_type n, const String& str)
                             { m_str.replace(pos, n, str.m_str); return *this; }
  String& replace(size_type p1, size_type n1, const std::string& str,
                  size_type p2, size_type n2 = std::string::npos)
                     { m_str.replace(p1, n1, str, p2, n2); return *this; }
  String& replace(size_type p1, size_type n1, const String& str,
                  size_type p2, size_type n2 = std::string::npos)
                     { m_str.replace(p1, n1, str.m_str, p2, n2); return *this; }
  String& replace(size_type pos, size_type n1, const char* psz, size_type n2)
                              { m_str.replace(pos, n1, psz, n2); return *this; }
  String& replace(iterator first, iterator last, const char* psz, size_type n2)
                               { m_str.replace(first, last, psz, n2); return *this; }
  String& replace(size_type pos, size_type n, const char* psz) { m_str.replace(pos, n, psz); return *this; }
  String& replace(iterator first, iterator last, const char* psz)
                              { m_str.replace(first, last, psz); return *this;  }
  String& replace(size_type pos, size_type n, size_type n2, char c)
                                 { m_str.replace(pos, n, n2, c); return *this; }
  String& replace(iterator first, iterator last, size_type n, char c)
                             { m_str.replace(first, last, n, c); return *this; }
  String substr(size_type pos, size_type n = std::string::npos) const { return m_str.substr(pos, n); }
  size_type copy(char* dest, size_type n, size_type pos = 0) const
                                            { return m_str.copy(dest, n, pos); }
  void resize(size_type n) { m_str.resize(n); }
  void resize(size_type n, char c) { m_str.resize(n, c); }
  void swap( std::string& o) { m_str.swap(o); }
  void swap( String& o) { m_str.swap(o.m_str); }
  size_type find(const std::string& str, size_type pos = 0) const
                                                { return m_str.find(str, pos); }
  size_type find(const String& str, size_type pos = 0) const
                                         { return m_str.find(str.m_str, pos); }
  size_type find(const char *psz, size_type pos, size_type n) const
                                             { return m_str.find(psz, pos, n); }
  size_type find(const char *psz, size_type pos = 0) const
                                                { return m_str.find(psz, pos); }
  size_type find(char c, size_type pos = 0) const { return m_str.find(c, pos); }
  size_type rfind(const std::string& str, size_type pos = std::string::npos) const
                                                { return m_str.rfind(str, pos); }
  size_type rfind(const String& str, size_type pos = std::string::npos) const
                                           { return m_str.rfind(str.m_str, pos); }
  size_type rfind(const char *psz, size_type pos, size_type n) const
                                             { return m_str.rfind(psz, pos, n); }
  size_type rfind(const char *psz, size_type pos = std::string::npos) const
                                                { return m_str.rfind(psz, pos); }
  size_type rfind(char c, size_type pos = std::string::npos) const { return m_str.rfind(c, pos); }
  size_type find_first_of(const std::string& str, size_type pos = 0) const
                                             { return m_str.find_first_of(str, pos); }
  size_type find_first_of(const String& str, size_type pos = 0) const
                                       { return m_str.find_first_of(str.m_str, pos); }
  size_type find_first_of(const char* psz, size_type pos, size_type n) const
                                          { return m_str.find_first_of(psz, pos, n); }
  size_type find_first_of(const char* psz, size_type pos = 0) const
                                             { return m_str.find_first_of(psz, pos); }
  size_type find_first_of(char c, size_type pos = 0) const
                                               { return m_str.find_first_of(c, pos); }
  size_type find_first_not_of(const std::string& str, size_type pos = 0) const
                                         { return m_str.find_first_not_of(str, pos); }
  size_type find_first_not_of(const String& str, size_type pos = 0)
                                   { return m_str.find_first_not_of(str.m_str, pos); }
  size_type find_first_not_of(const char* psz, size_type pos, size_type n) const
                                      { return m_str.find_first_not_of(psz, pos, n); }
  size_type find_first_not_of(const char* psz, size_type pos = 0)
                                         { return m_str.find_first_not_of(psz, pos); }
  size_type find_first_not_of(char c, size_type pos = 0) const
                                           { return m_str.find_first_not_of(c, pos); }
  size_type find_last_of(const std::string& str, size_type pos = std::string::npos) const
                                              { return m_str.find_last_of(str, pos); }
  size_type find_last_of(const String& str, size_type pos = std::string::npos) const
                                        { return m_str.find_last_of(str.m_str, pos); }
  size_type find_last_of(const char* psz, size_type pos, size_type n) const
                                           { return m_str.find_last_of(psz, pos, n); }
  size_type find_last_of(const char* psz, size_type pos = std::string::npos) const
                                              { return m_str.find_last_of(psz, pos); }
  size_type find_last_of(char c, size_type pos = std::string::npos) const
                                                { return m_str.find_last_of(c, pos); }
  size_type find_last_not_of(const std::string& str, size_type pos = std::string::npos) const
                                          { return m_str.find_last_not_of(str, pos); }
  size_type find_last_not_of(const String& str, size_type pos = std::string::npos) const
                                    { return m_str.find_last_not_of(str.m_str, pos); }
  size_type find_last_not_of(const char* psz, size_type pos, size_type n) const
                                       { return m_str.find_last_not_of(psz, pos, n); }
  size_type find_last_not_of(const char* psz, size_type pos = std::string::npos) const
                                          { return m_str.find_last_not_of(psz, pos); }
  size_type find_last_not_of(char c, size_type pos = std::string::npos) const
                                            { return m_str.find_last_not_of(c, pos); }
  //@}

  //! \name Constructors
  //@{

  //! \brief Default constructor.
  String() {}

  //! \brief Constructor from a char pointer.
  //! \param psz Char pointer.
  String(const char *psz)
  {
    if( NULL == psz )
      m_str.clear();
    else
      m_str.append(psz);
  }

  //! \brief Constructor from char buffer.
  //! \param psz Char pointer.
  //! \param size %Buffer size.
  String(const char *psz, int size)
  {
    if( NULL == psz )
      m_str.clear();
    else
      m_str.append(psz, size);
  }

  //! \brief Copy Constructor.
  //! \param str The source string.
  String(const String &str) : m_str(str.str()) {}

  //! \brief Constructor from a ostringstream
  //! \param oss The stream
  String(const std::ostringstream &oss) : m_str(oss.str()) {}

  //! \brief Constructor from std::string.
  //! \param str The string.
  String(const std::string &str) : m_str(str) {}
  //@}

  //! \deprecated
  static String str_format(pcsz format, ...);

  //! \brief Compares strings (i.e. operator==).
  //!
  //! Returns true if strings are equal, false otherwise.
  //! \param str The source string.
  bool is_equal(const std::string &str) const
  { return StringUtil::is_equal(m_str, str); }
  bool is_equal(const String &str) const
  { return StringUtil::is_equal(m_str, str.str()); }
  bool is_equal(const char *psz) const
  { return StringUtil::is_equal(m_str, std::string(psz)); }

  //! \brief Compares string in a no case sensitive way.
  //!
  //! Returns true if strings are equal, false otherwise.
  //! \param str The source string.
  bool is_equal_no_case(const std::string &str) const
  { return StringUtil::is_equal_no_case(m_str, str); }
  bool is_equal_no_case(const String &str) const
  { return StringUtil::is_equal_no_case(m_str, str.str()); }
  bool is_equal_no_case(const char *psz) const
  { return StringUtil::is_equal_no_case(m_str, std::string(psz)); }

  //! \brief Tests the first character.
  //!
  //! Returns true if the string starts with this character, false otherwise.
  //! \param c The character.
  bool starts_with(char c) const
  { return StringUtil::starts_with(m_str, c); }

  //! \brief Tests the first characters with that of another string.
  //!
  //! Returns true if the strings start with the same character, false otherwise.
  //! \param pcszStart The source string.
  //! \param no_case If set to true, the test is not case sensitive.
  bool starts_with(pcsz pcszStart, bool no_case=false) const
  { return StringUtil::starts_with(m_str, pcszStart, no_case); }
  bool starts_with(const std::string& str, bool no_case=false) const
  { return StringUtil::starts_with(m_str, str.c_str(), no_case); }

  //! \brief Tests the last character.
  //!
  //! Returns true if the string ends with this character, false otherwise.
  //! \param c The character.
  bool ends_with(char c) const
  { return StringUtil::ends_with(m_str, c); }

  //! \brief Tests the last characters with that of another string.
  //!
  //! Returns true if the strings end with the same character, false otherwise.
  //! \param pcszEnd The source string.
  //! \param no_case If set to true, the test is not case sensitive.
  bool ends_with(pcsz pcszEnd, bool no_case=false) const
  { return StringUtil::ends_with(m_str, pcszEnd, no_case); }
  bool ends_with(const std::string& str, bool no_case=false) const
  { return StringUtil::ends_with(m_str, str.c_str(), no_case); }

  //! \name Token extraction
  //@{

  //! \brief Family results for token extraction methods.
  enum ExtractTokenRes
  {
    //! Nothing extracted.
    EMPTY_STRING=0,
    //! %String extracted and separator found.
    SEP_FOUND,
    //! %String extracted and separator not found.
    SEP_NOT_FOUND
  };

  //! \brief Looks for a token, from left to right.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first separator occurrence. The extracted token is removed from the string.
  //! \param c Separator.
  //! \param[out] pstrToken %String object receiving the extracted token.
  //! \param apply_escape Take care of '\' before c to escape it
  ExtractTokenRes extract_token(char c, String *token_p, bool apply_escape=false)
  { return static_cast<ExtractTokenRes>(StringUtil::extract_token(&m_str, c, &(token_p->str()), apply_escape)); }
  ExtractTokenRes extract_token(char c, std::string *token_p, bool apply_escape=false)
  { return static_cast<ExtractTokenRes>(StringUtil::extract_token(&m_str, c, token_p, apply_escape)); }

  //! \brief Looks for a token, from right to left.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first separator occurrence. The extracted token is removed from the string.
  //! \param c Separator.
  //! \param[out] token_p Extracted token.
  //! \param apply_escape Take care of '\' before c to escape it
  ExtractTokenRes extract_token_right(char c, String *token_p, bool apply_escape=false)
  { return static_cast<ExtractTokenRes>(StringUtil::extract_token_right(&m_str, c, &(token_p->str()), apply_escape)); }
  ExtractTokenRes extract_token_right(char c, std::string *token_p, bool apply_escape=false)
  { return static_cast<ExtractTokenRes>(StringUtil::extract_token_right(&m_str, c, token_p, apply_escape)); }

  //! \brief Looks for enclosed token, from left to right.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first left separator occurrence. The extracted token is removed from the string.
  //! \param cl Left separator.
  //! \param cr Right separator.
  //! \param[out] token_p Extracted token.
  //! \param apply_escape Take care of '\' before cl/cr to escape them
  ExtractTokenRes extract_token(char cl, char cr, String *token_p, bool apply_escape=false)
  { return static_cast<ExtractTokenRes>(StringUtil::extract_token(&m_str, cl, cr, &(token_p->str()), apply_escape)); }
  ExtractTokenRes extract_token(char cl, char cr, std::string *token_p, bool apply_escape=false)
  { return static_cast<ExtractTokenRes>(StringUtil::extract_token(&m_str, cl, cr, token_p, apply_escape)); }

  //! \brief Looks for enclosed token, from right to left.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first left separator occurrence. The extracted token is removed from the string.
  //! \param cl Left separator.
  //! \param cr Right separator.
  //! \param[out] token_p Extracted token.
  //! \param apply_escape Take care of '\' before cl/cr to escape them
  ExtractTokenRes extract_token_right(char cl, char cr, String *token_p, bool apply_escape=false)
  { return static_cast<ExtractTokenRes>(StringUtil::extract_token_right(&m_str, cl, cr, &(token_p->str()), apply_escape)); }
  ExtractTokenRes extract_token_right(char cl, char cr, std::string *token_p, bool apply_escape=false)
  { return static_cast<ExtractTokenRes>(StringUtil::extract_token_right(&m_str, cl, cr, token_p, apply_escape)); }

  //@}

  //! \brief Removes characters that enclose string: quotes, parenthesis, etc...
  //!
  //! Returns true if enclosure was removed.
  //! ex: RemoveEnclosure("'", "'") -\> removes quotes in a string like 'string'.
  //! ex: RemoveEnclosure("(\[", ")\]") -\> removes parenthesis in a string like (string) or \[string\]
  //! but not in string like (string\].
  //! \param left List of possible left enclosure characters.
  //! \param right List of possible right enclosure characters.
  //! \remark \<pszLeft\> and \<pszRight\> must have the same length.
  bool remove_enclosure(psz left, psz right)
  { return StringUtil::remove_enclosure(&m_str, left, right); }

  //! \brief Removes characters that enclose string: quotes, parenthesis, etc...
  //!
  //! Returns true if enclosure was removed.
  //! ex: RemoveEnclosure("'", "'") -\> removes quotes in a string like 'string'.
  //! \param cl Left enclosure character.
  //! \param cr Right enclosure character.
  bool remove_enclosure(char cl, char cr)
  { return StringUtil::remove_enclosure(&m_str, cl, cr); }

  //! \brief Tests if string matches with mask containing '*' and '?' jokers.
  //!
  //! Returns true if string matches the mask, false otherwise. \n
  //! The mask can contain:
  //! - '*': match any number of characters.
  //! - '?': match one character.
  //! \param mask The mask.
  //! \param tokens_p elements that match '*' and '?' can be pushed in a vector
  bool match(pcsz mask, std::vector<std::string>* tokens_p=0) const
  { return StringUtil::match(m_str, mask, tokens_p); }

  //! \brief Tests if string matches with mask containing '*' and '?' jokers.
  //!
  //! Returns true if string matches the mask, false otherwise. \n
  //! The mask can contain:
  //! - '*': match any number of characters.
  //! - '?': match one character.
  //! \param mask The mask.
  //! \param tokens_p elements that match '*' and '?' can be pushed in a vector
  bool match(const String &mask, std::vector<std::string>* tokens_p=0) const
  { return StringUtil::match(m_str, mask.str(), tokens_p); }
  bool match(const std::string &mask, std::vector<std::string>* tokens_p=0) const
  { return StringUtil::match(m_str, mask, tokens_p); }

  //! \brief Removes white spaces at beginning and end of string.
  String& trim()
  { StringUtil::trim(&m_str); return *this; }

  //! \brief Removes white spaces at beginning of string.
  String& trim_left()
  { StringUtil::trim_left(&m_str); return *this; }

  //! \brief Removes white spaces at end of string.
  String& trim_right()
  { StringUtil::trim_right(&m_str); return *this; }

  //! \deprecated
  int printf(pcsz format, ...);

  //! \brief Splits the string.
  //!
  //! The string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the string vector, while *this* string is
  //! preserved.
  //! \param c Separator.
  //! \param[out] vec_p Pointer to a vector of strings.
  //! \param clear_coll If set to true, the vector is cleared.
  void split(char c, std::vector<std::string> *vec_p, bool clear_coll=true)
  { StringUtil::split(&m_str, c, vec_p, clear_coll); }
  void split(char c, std::vector<String> *vec_p, bool clear_coll=true)
  { StringUtil::split(&m_str, c, vec_p, clear_coll); }

  //! \brief Splits the string.
  //!
  //! The string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the string vector, while *this* string is
  //! preserved.
  //! \param c Separator.
  //! \param[out] vec_p Pointer to a vector of strings.
  //! \param clear_coll If set to true, the vector is cleared.
  void split(char c, std::vector<std::string> *vec_p, bool clear_coll=true) const
  { StringUtil::split(m_str, c, vec_p, clear_coll); }
  void split(char c, std::vector<String> *vec_p, bool clear_coll=true) const
  { StringUtil::split(m_str, c, vec_p, clear_coll); }

  //! \brief Splits the string.
  //!
  //! The string is split in 2 tokens separated with the specified separator.
  //!
  //! \param c Separator.
  //! \param[out] pstrLeft Left part of the split string.
  //! \param[out] pstrRight Right part of the split string.
  //! \param trim Trim left & right parts.
  void split(char c, String *left, String *right, bool trim=false)
  { StringUtil::split(&m_str, c, &(left->m_str), &(right->m_str), trim); }
  void split(char c, std::string *left, std::string *right)
  { StringUtil::split(&m_str, c, left, right); }

  //! \brief Joins strings from a string vector, using specified separator.
  //!
  //! Replaces *this* string with the result.
  //! For instance: join (\<str1, str2\>, ";") gives: str1;str2
  //! \param vec The source vector.
  //! \param sep %String separator.
  String& join(const std::vector<String> &vec, char sep=',')
  { StringUtil::join(&m_str, vec, sep ); return *this; }
  String& join(const std::vector<std::string> &vec, char sep=',')
  { StringUtil::join(&m_str, vec, sep ); return *this; }

  //! \brief Removes items separated by a specific separator.
  //!
  //! For instance: string like "item1,item2,item3,...". \n
  //! Returns true if any item was found, false otherwise.
  //! \param item Item to find and remove.
  //! \param cep Items separator.
  bool remove_item(const std::string &item, char sep=',')
  { return StringUtil::remove_item(&m_str, item, sep); }

  //! \brief Converts string to lowercase.
  String& to_lower()
  { StringUtil::to_lower(&m_str); return *this; }

  //! \brief Converts string to uppercase.
  String& to_upper()
  { StringUtil::to_upper(&m_str); return *this; }

  //! \brief Finds and replaces a string.
  //! \param s1 %String to replace.
  //! \param s2 Substitution string.
  String& replace(pcsz s1, pcsz s2)
  { StringUtil::replace(&m_str, s1, s2); return *this; }

  //! \brief Finds and replaces one character.
  //!
  //! \param c1 Character to replace.
  //! \param c2 Substitution character.
  String& replace(char c1, char c2)
  { StringUtil::replace(&m_str, c1, c2); return *this; }


  //! \brief Returns a 32 bits hash code.
  uint32 hash() const
  { return StringUtil::hash(m_str); }

  //! \brief Returns the hash code using the FNV-1a algorithm.
  //!
  //! Calculates a 64 bits hash code. See details of the algorithm
  //! on http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function.
  uint64 hash64() const
  { return StringUtil::hash64(m_str); }

  //! \brief Converts from type \<T\>
  //!
  //! \param number The \<T\> type value to convert.
  //! \param _throw If set to true, throws an exception if conversion fails.
  //! \exception SOFTWARE_ERROR Thrown if conversion fails.
  template <class T>
  void from_num(const T& number, bool _throw = true)
  {
    StringUtil::from_num<T>(&m_str, number, _throw);
  }

  //! \brief Converts string content to numeric type \<_T\>.
  //!
  //! Returns the converted string in numeric type \<_T\>.
  //! Should also work for any "istringstream::operator>>" supported type.
  //! \param _throw If set to true, throws an exception if conversion fails.
  //! \exception SOFTWARE_ERROR Thrown if conversion fails.
  template <class T>
  T to_num(bool _throw = true)
  {
    return StringUtil::to_num<T>(m_str, _throw);
  }

#ifndef YAT_CPP11
  //! \name some methods availables with c++ 11
  //@{
  char& front() { return m_str[0]; }
  const char& front() const { return m_str[0]; }
  char& back() { return m_str[m_str.size()-1]; }
  const char& back() const { return m_str[m_str.size()-1]; }
  void pop_back() { m_str.erase(m_str.size()-1, 1); }
  //@}
#endif

  //! \name Deprecated
  //@{
  bool start_with(char c) const { return starts_with(c); }
  bool start_with(pcsz p, bool b=false) const { return starts_with(p, b); }
  bool end_with(char c) const { return ends_with(c); }
  bool end_with(pcsz p, bool b=false) const { return ends_with(p, b); }
  //@}

private:
  std::string m_str;
};

// Operators
YAT_DECL std::string operator+(const String& s1, const String& s2);
YAT_DECL std::string operator+(const String& s1, const std::string& s2);
YAT_DECL std::string operator+(const std::string& s1, const String& s2);
YAT_DECL std::string operator+(const char* psz, const String& str);
YAT_DECL std::string operator+(char c, const String& str);
YAT_DECL std::string operator+(const String& str, const char* psz);
YAT_DECL std::string operator+(const String& str, char c);
YAT_DECL bool operator==(const String& s1, const String& s2);
YAT_DECL bool operator==(const std::string& s1, const String& s2);
YAT_DECL bool operator==(const String& s1, const std::string& s2);
YAT_DECL bool operator!=(const String& s1, const String& s2);
YAT_DECL bool operator!=(const String& s1, const std::string& s2);
YAT_DECL bool operator!=(const std::string& s1, const String& s2);
YAT_DECL bool operator<(const String& s1, const String& s2);
YAT_DECL bool operator<(const String& s1, const std::string& s2);
YAT_DECL bool operator<(const std::string& s1, const String& s2);
YAT_DECL bool operator<=(const String& s1, const std::string& s2);
YAT_DECL bool operator<=(const String& s1, const String& s2);
YAT_DECL bool operator<=(const std::string& s1, const String& s2);
YAT_DECL bool operator>(const String& s1, const String& s2);
YAT_DECL bool operator>(const String& s1, const std::string& s2);
YAT_DECL bool operator>(const std::string& s1, const String& s2);
YAT_DECL bool operator>=(const String& s1, const std::string& s2);
YAT_DECL bool operator>=(const String& s1, const String& s2);
YAT_DECL bool operator>=(const std::string& s1, const String& s2);
YAT_DECL bool operator==(const String& s1, const char* s2);
YAT_DECL bool operator==(const char* s1, const String& s2);
YAT_DECL bool operator!=(const String& s1, const char* s2);
YAT_DECL bool operator!=(const char* s1, const String& s2);
YAT_DECL bool operator<(const String& s1, const char* s2);
YAT_DECL bool operator<(const char* s1, const String& s2);
YAT_DECL bool operator<=(const String& s1, const char* s2);
YAT_DECL bool operator<=(const char* s1, const String& s2);
YAT_DECL bool operator>(const String& s1, const char* s2);
YAT_DECL bool operator>(const char* s1, const String& s2);
YAT_DECL bool operator>=(const String& s1, const char* s2);
YAT_DECL bool operator>=(const char* s1, const String& s2);

YAT_DECL std::ostream& operator<<(std::ostream& os, const String& s);
YAT_DECL std::istream& operator>>(std::istream& is, const String& s);

// ============================================================================
//! \class StringFormat
//! \brief Format string using printf-like format spec
//! \par Usage:
//! \code{.cpp}
//! std::cout << yat::StringFormat("ga: {}, bu: {}, zo: {}, meu: {}").format("pomper").format(true).format(12).format(3.14) << std::endl;
//! std::cout << yat::StringFormat("shadock={<10}").format("gabu") << std::endl;
//! std::cout << yat::StringFormat("shadock={>10}").format("zomeu") << std::endl;
//! std::cout << yat::StringFormat("ga {b} zo {s}").format(true).format("meu") << std::endl;
//! std::cout << yat::StringFormat("pct {.1%}").format(0.2) << std::endl;
//! std::cout << yat::StringFormat("bu {B}").format(h) << std::endl;
//! std::cout << yat::StringFormat("bu {*>#8x}").format(12) << std::endl;
//! std::cout << yat::StringFormat("bu {X}").format(12) << std::endl;
//! std::cout << yat::StringFormat("ga {12} {8.4} {*<+30} {!>15g} {8.7E} {>12.2f}").format(5).format(65*3.47).format(42).format(42).format(4.245684451).format(42) << std::endl;
//! \endcode
// ============================================================================
class StringFormat
{
public:
  //! \brief Default constructor.
  StringFormat() {}

  //! \brief Constructor from a char pointer.
  //! \param psz Char pointer.
  StringFormat(const char *psz) : m_str(psz), m_fmt_idx(0) {}

  //! \brief Constructor from char buffer.
  //! \param psz Char pointer.
  //! \param size %Buffer size.
  StringFormat(const char *psz, int size) : m_str(psz, size), m_fmt_idx(0) {}

  //! \brief Copy Constructor.
  //! \param str The source string.
  StringFormat(const String &str) : m_str(str), m_fmt_idx(0) {}

  //! \brief Constructor from std::string.
  //! \param str The string.
  StringFormat(const std::string &str) : m_str(str), m_fmt_idx(0) {}

  //! Explicit conversion
  operator const std::string&() const { return m_str.str(); }
  operator std::string&() { return m_str; }
  operator const yat::String&() const { return m_str; }
  operator yat::String&() { return m_str; }

  //! Apply format
  //!
  //! format spec is:\n
  //! [[fill]align][sign][#][width][.precision][type] \n
  //! fill        ::=  any character\n
  //! align       ::=  "<" | ">"\n
  //! sign        ::=  "+" | "-"\n
  //! width       ::=  integer\n
  //! precision   ::=  integer\n
  //! type        ::=  "b" | "B" | "e" | "E" | "f" | "F" | "g" | "G" | "o" | "s" | "x" | "X" | "%"\n
  //! \par align:
  //! '<' Forces the field to be left-aligned within the available space (this is the default for most objects).\n
  //! '>' Forces the field to be right-aligned within the available space (this is the default for numbers).
  //! \par sign
  //! '+' indicates that a sign should be used for both positive as well as negative numbers.\n
  //! '-' indicates that a sign should be used only for negative numbers (this is the default behavior).
  //! \par '#' character
  //! only valid for integers, and only for binary, octal, or hexadecimal output. If present, it specifies that the output will be prefixed by '0b', '0o', or '0x', respectively.
  //! \par type
  //! 's' String format. This is the default type for strings and may be omitted.\n
  //! 'B' Binary format. Outputs the number in base 2.\n
  //! 'o' Octal format. Outputs the number in base 8.\n
  //! 'x' Hex format. Outputs the number in base 16, using lower- case letters for the digits above 9.\n
  //! 'X' Hex format. Outputs the number in base 16, using upper- case letters for the digits above 9.\n
  //! 'e' Exponent notation. Prints the number in scientific notation using the letter ‘e’ to indicate the exponent. The default precision is 6.\n
  //! 'E' Exponent notation. Same as 'e' except it uses an upper case ‘E’ as the separator character.\n
  //! 'f' Fixed-point notation. Displays the number as a fixed-point number. The default precision is 6.\n
  //! 'g' General format. For a given precision p >= 1, this rounds the number to p significant digits and then formats the result in either fixed-point format or in scientific notation, depending on its magnitude.\n
  //! 'G' General format. Same as 'g' except switches to 'E' if the number gets too large. The representations of infinity and NaN are uppercased, too.\n
  //! '%%' Percentage. Multiplies the number by 100 and displays in fixed ('f') format, followed by a percent sign.\n
  template<class T> StringFormat& format(const T& v)
  {
    std::ostringstream oss;
    char type = 0;
    yat::String before, after;
    prepare_format(oss, type, before, after);

    if( 'B' == type )
    {
      std::bitset<8 * sizeof(T)> bs(v);
      oss << bs;
    }
    else if( '%' == type )
    {
      oss << 100 * v << '%';
    }
    else
      oss << v;

    m_fmt_idx = oss.str().size();
    oss << after;
    m_str = oss.str();
    return *this;
  }

  StringFormat& format(const bool& v);
  StringFormat& format(const char *v);
  StringFormat& format(const std::string& v);
  StringFormat& format(const yat::String& v);

private:
  yat::String m_str;
  std::size_t m_fmt_idx;

  void prepare_format(std::ostringstream& oss, char& type, yat::String& before, yat::String& after);
};

} // namespace

#endif
