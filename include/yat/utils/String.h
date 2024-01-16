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
#include <cstring>
#include <yat/CommonHeader.h>
#include <yat/utils/NonCopyable.h>
#include <yat/memory/SharedPtr.h>
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
  //! @deprecated
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

//! hash values types
typedef std::pair<uint64, uint64>  h128_t;
typedef uint64                     h64_t;
typedef uint32                     h32_t;

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

  //! \brief Compares strings (i.e. operator==).
  //!
  //! Returns true if strings are equal, false otherwise.
  //! \param str The source string.
  //! \param other The other string to compare with.
  static bool is_equal(const std::string& str, const std::string& other);

  //! \brief Compares std::string in a no case sensitive way.
  //!
  //! Returns true if strings are equal, false otherwise.
  //! \param str The source string.
  //! \param other The other string to compare with.
  static bool is_equal_no_case(const std::string& str, const std::string& other);

  //! \brief Tests the first character.
  //!
  //! Returns true if the string starts with this character, false otherwise.
  //! \param str The source string.
  //! \param c The character to compare with.
  static bool starts_with(const std::string& str, char c);

  //! \brief Tests the first character with that of another std::string.
  //!
  //! Returns true if the strings start with the same character, false otherwise.
  //! \param src The source string.
  //! \param pcszStart The string fragment to compare with.
  //! \param bNoCase If set to true, the test is not case sensitive.
  static bool starts_with(const std::string& str, pcsz pcszStart, bool bNoCase=false);

  //! \brief Tests the last character.
  //!
  //! Returns true if the string ends with this character, false otherwise.
  //! \param str The source string.
  //! \param c The character.
  static bool ends_with(const std::string& str, char c);

  //! \brief Tests the last character with that of another std::string.
  //!
  //! Returns true if the strings end with the same character, false otherwise.
  //! \param str The source string.
  //! \param pcszEnd The source string.
  //! \param bNoCase If set to true, the test is not case sensitive.
  static bool ends_with(const std::string& str, pcsz pcszEnd, bool bNoCase=false);

  //! \name Token extraction
  //!@{

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
  //! \param str_p The source string to extract from.
  //! \param c Separator.
  //! \param[out] pstrToken %string object receiving the extracted token.
  //! \param escape_char escape character to take care of.
  static ExtractTokenRes extract_token(std::string* str_p, char c, std::string *pstrToken,
                                       char escape_char);
  static ExtractTokenRes extract_token(std::string* str_p, char c, std::string *pstrToken);

  //! \brief Looks for a token, from right to left.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first separator occurrence. The extracted token is removed from the std::string.
  //! \param str_p The source string to extract from.
  //! \param c Separator.
  //! \param[out] pstrToken Extracted token.
  //! \param escape_char escape character to take care of.
  static ExtractTokenRes extract_token_right(std::string* str_p, char c, std::string *pstrToken,
                                             char escape_char);
  static ExtractTokenRes extract_token_right(std::string* str_p, char c, std::string *pstrToken);

  //! \brief Looks for enclosed token, from left to right.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first left separator occurrence. The extracted token is removed from the std::string.
  //! \param str_p The source string to extract from.
  //! \param cLeft Left separator.
  //! \param cRight Right separator.
  //! \param[out] pstrToken Extracted token.
  //! \param escape_char escape character to take care of.
  static ExtractTokenRes extract_token(std::string* str_p, char cLeft, char cRight,
                                       std::string *pstrToken, char escape_char);
  static ExtractTokenRes extract_token(std::string* str_p, char cLeft, char cRight,
                                       std::string *pstrToken);

  //! \brief Looks for enclosed token, from right to left.
  //!
  //! Returns the extraction status.\n
  //! The function looks for the first left separator occurrence. The extracted token is removed from the std::string.
  //! \param str_p The source string to extract from.
  //! \param cLeft Left separator.
  //! \param cRight Right separator.
  //! \param[out] pstrToken Extracted token.
  //! \param escape_char escape character to take care of.
  static ExtractTokenRes extract_token_right(std::string* str_p, char cLeft, char cRight,
                                             std::string *pstrToken, char escape_char);
  static ExtractTokenRes extract_token_right(std::string* str_p, char cLeft, char cRight,
                                             std::string *pstrToken);

  //!@}

  //! \brief Removes characters that enclose std::string: quotes, parenthesis, etc...
  //!
  //! Returns true if enclosure was removed.
  //! ex: RemoveEnclosure("'", "'") -\> removes quotes in a std::string like 'std::string'.
  //! ex: RemoveEnclosure("(\[", ")\]") -\> removes parenthesis in a std::string like (std::string) or \[std::string\]
  //! but not in std::string like (std::string\].
  //! \param str_p The source string to extract from.
  //! \param pszLeft List of possible left enclosure characters.
  //! \param pszRight List of possible right enclosure characters.
  //! \remark \<pszLeft\> and \<pszRight\> must have the same length.
  static bool remove_enclosure(std::string* str_p, psz pszLeft, psz pszRight);

  //! \brief Removes characters that enclose std::string: quotes, parenthesis, etc...
  //!
  //! Returns true if enclosure was removed.
  //! ex: RemoveEnclosure("'", "'") -\> removes quotes in a std::string like 'std::string'.
  //! \param str_p The source string to extract from.
  //! \param cLeft Left enclosure character.
  //! \param cRight Right enclosure character.
  static bool remove_enclosure(std::string* str_p, char cLeft, char cRight);

  //! \brief Tests if string matches with mask containing '*' and '?' jokers.
  //!
  //! Returns true if string matches the mask, false otherwise. \n
  //! The mask can contain:
  //! - '*': match any number of characters.
  //! - '?': match one character.
  //! \param str The source string.
  //! \param pszMask The mask.
  //! \param pvectokens elements that match '*' and '?' can be pushed in a vector
  static bool match(const std::string& str, pcsz pszMask, std::vector<std::string> *pvectokens=0);

  //! \brief Tests if string matches with mask containing '*' and '?' jokers.
  //!
  //! Returns true if string matches the mask, false otherwise. \n
  //! The mask can contain:
  //! - '*': match any number of characters.
  //! - '?': match one character.
  //! \param str The source string.
  //! \param pvectokens elements that match '*' and '?' can be pushed in a vector
  //! \param strMask The mask.
  static bool match(const std::string& str, const std::string &strMask, std::vector<std::string> *pvectokens=0);

  //! \brief Removes white spaces at beginning and end of string.
  //! \param str_p input/output string.
  static void trim(std::string* str_p);

  //! \brief Removes white spaces at beginning of string.
  //! \param str_p input/output string.
  static void trim_left(std::string* str_p);

  //! \brief Removes white spaces at end of string.
  //! \param str_p input/output string.
  static void trim_right(std::string* str_p);

  //! \brief Removes white spaces at beginning and end of all strings in collection.
  //! \param vec_p input/output strings vector.
  static void trim(std::vector<std::string>* vec_p);

  //! \brief Removes white spaces at end of all strings in collection.
  //! \param vec_p input/output strings vector.
  static void trim_right(std::vector<std::string>* vec_p);

  //! \brief Removes white spaces at beginning of all strings in collection.
  //! \param vec_p input/output strings vector.
  static void trim_left(std::vector<std::string>* vec_p);

  //! \brief Removes white spaces at beginning and end of all strings in collection.
  //! \param vec_p input/output strings vector.
  static void trim(std::vector<yat::String>* vec_p);

  //! \brief Removes white spaces at end of all strings in collection.
  //! \param vec_p input/output strings vector.
  static void trim_right(std::vector<yat::String>* vec_p);

  //! \brief Removes white spaces at beginning of all strings in collection.
  //! \param vec_p input/output strings vector.
  static void trim_left(std::vector<yat::String>* vec_p);

  //! \brief Splits the string.
  //!
  //! The string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the std::string vector, while *this*
  //! string is preserved.
  //! \param str_p input/output string.
  //! \param c Separator.
  //! \param[out] pvecstr Pointer to a vector of std::strings.
  //! \param clear_coll If set to true, the vector is cleared.
  static void split(std::string* str_p, char c, std::vector<std::string> *pvecstr,
                    bool clear_coll=true);
  static void split(std::string* str_p, char c, std::vector<String> *pvecstr,
                    bool clear_coll=true);

  //! \brief Splits the string.
  //!
  //! The string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the std::string vector, while *this*
  //! string ispreserved.
  //! \param str input string.
  //! \param c Separator.
  //! \param[out] pvecstr Pointer to a vector of std::strings.
  //! \param clear_coll If set to true, the vector is cleared.
  static void split(const std::string& str, char c, std::vector<std::string> *pvecstr,
                    bool clear_coll=true);
  static void split(const std::string& str, char c, std::vector<String> *pvecstr,
                    bool clear_coll=true);

  //! \brief Splits the string.
  //!
  //! The string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the std::string deque, while *this*
  //! string is preserved.
  //! \param str_p input/output string.
  //! \param c Separator.
  //! \param[out] p_deque Pointer to a vector of std::strings.
  //! \param clear_coll If set to true, the deque is cleared first.
  static void split(std::string* str_p, char c, std::deque<std::string> *p_deque,
                    bool clear_coll=true);
  static void split(std::string* str_p, char c, std::deque<String> *p_deque,
                    bool clear_coll=true);

  //! \brief Splits the std::string.
  //!
  //! The string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the std::string deque, while *this*
  //! string is preserved.
  //! \param str input string.
  //! \param c Separator.
  //! \param[out] p_deque Pointer to a deque of std::strings.
  //! \param clear_coll If set to true, the deque is cleared first.
  static void split(const std::string& str, char c, std::deque<std::string> *p_deque,
                    bool clear_coll=true);
  static void split(const std::string& str, char c, std::deque<String> *p_deque,
                    bool clear_coll=true);

  //! \brief Splits the string.
  //!
  //! The string is split in 2 tokens separated with the specified separator.
  //!
  //! \param str input string.
  //! \param c Separator.
  //! \param[out] pstrLeft Left part of the split std::string.
  //! \param[out] pstrRight Right part of the split std::string.
  //! \param trim If set to true, the trim function is applied on left & right parts
  static void split(const std::string& str, char c, std::string *pstrLeft,
                    std::string *pstrRight, bool trim = false);

  //! \brief Splits the string.
  //!
  //! The string is split in 2 tokens separated with the specified separator.
  //!
  //! \param str_p input/output string.
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
  //! \param str_p input/output string.
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
  //! \param str_p input/output string.
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
  //! \param str_p input/output string.
  //! \param strItem Item to find and remove.
  //! \param cSep Items separator.
  static bool remove_item(std::string* str_p, const std::string &strItem,
                          char cSep=',');

  //! \brief Converts std::string to lowercase.
  //! \param str_p input/output string.
  static void to_lower(std::string* str_p);

  //! \brief Converts std::string to uppercase.
  //! \param str_p input/output string.
  static void to_upper(std::string* str_p);

  //! \brief Finds and replaces a std::string.
  //! \param str_p input/output string.
  //! \param pszSrc %std::string to replace.
  //! \param pszDst Substitution std::string.
  static void replace(std::string* str_p, pcsz pszSrc, pcsz pszDst);

  //! \brief Finds and replaces one character.
  //!
  //! \param str_p input/output string.
  //! \param cSrc Character to replace.
  //! \param cDst Substitution character.
  static void replace(std::string* str_p, char cSrc, char cDst);

  //! \brief Substitute occurences of characters by a single one
  //!
  //! \param str_p input/output string.
  //! \param pszCharSet Characters set to substitute.
  //! \param cReplacement Substitution character.
  static void substitute(std::string* str_p, pcsz pszCharSet, char cReplacement);

  //! \brief Remove occurences of characters
  //!
  //! \param str_p input/output string.
  //! \param pszCharSet Characters set to remove.
  static void remove(std::string* str_p, pcsz pszCharSet);

  //! \brief Returns a 32 bits hash code.
  //! \param str input string.
  static h32_t hash(const std::string& str);

  //! \brief Returns the hash code using the FNV-1a algorithm.
  //!
  //! Calculates a 64 bits hash code. See details of the algorithm
  //! on http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function.
  //! \param str input string.
  static h64_t hash64(const std::string& str);

  //! \brief Returns the hash code using the FNV-1a algorithm.
  //!
  //! Calculates a 128 bits hash code. See details of the algorithm
  //! on http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function.
  //! Derived from this C# implementation: https://github.com/3F/Fnv1a128
  //! This is an high-speed implementation using LX4Cnh (https://github.com/3F/LX4Cnh)
  //!
  //! \return an h128_t value that consist of a pair of uint64 numbers
  //!         first is the high 64bits part, second is the low 64bits part
  static h128_t hash128(const std::string& str);

  //! \brief Converts string content to numeric type \<_T\>.
  //!
  //! Returns the converted string in numeric type \<_T\>.
  //! Should also work for any "istringstream::operator>>" supported type.
  //! \param str The string to convert.
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
  //! \param str_p input/output string.
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
  //! \return the number converted in string
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

  //! \name Deprecated methods
  //@{
  static std::string str_format(pcsz pszFormat, ...);
  static ExtractTokenRes extract_token(std::string*, char, std::string*, bool);
  static ExtractTokenRes extract_token_right(std::string*, char, std::string*,
                                             bool);
  static ExtractTokenRes extract_token_right(std::string*, char, char,
                                             std::string*, bool);
  static ExtractTokenRes extract_token(std::string*, char, char,
                                       std::string*, bool);
  static int printf(std::string*, pcsz, ...);
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
  //!@{

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
  //!@}

  //! \name Constructors
  //!@{

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

  //!@}

  //! \name Comparison methods
  //!@{

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

  //!@}

  //! \name Token extraction
  //!@{

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
  //! \param[out] token_p %String object receiving the extracted token.
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

  //!@}

  //! \name Basic pattern matching
  //!@{

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

  //!@}

  //! \name Triming
  //!@{

  //! \brief Removes white spaces at beginning and end of string.
  String& trim()
  { StringUtil::trim(&m_str); return *this; }

  //! \brief Removes white spaces at beginning of string.
  String& trim_left()
  { StringUtil::trim_left(&m_str); return *this; }

  //! \brief Removes white spaces at end of string.
  String& trim_right()
  { StringUtil::trim_right(&m_str); return *this; }

  //!@}

  //! \name Split & Join
  //!@{

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
  //! \param[out] left Left part of the split string.
  //! \param[out] right Right part of the split string.
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

  //!@}

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

  //! \brief Removes items separated by a specific separator.
  //!
  //! For instance: string like "item1,item2,item3,...". \n
  //! Returns true if any item was found, false otherwise.
  //! \param[in] item Item to find and remove.
  //! \param sep Items separator.
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
  h32_t hash() const
  { return StringUtil::hash(m_str); }

  //! \brief Returns the hash code using the FNV-1a algorithm.
  //!
  //! Calculates a 64 bits hash code. See details of the algorithm
  //! on http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function.
  h64_t hash64() const
  { return StringUtil::hash64(m_str); }

  //! \brief Returns the hash code using the FNV-1a algorithm.
  //!
  //! Calculates a 128 bits hash code. See details of the algorithm
  //! on http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function.
  //! Derived from this C# implementation: https://github.com/3F/Fnv1a128
  //! This is an high-speed implementation using LX4Cnh (https://github.com/3F/LX4Cnh)
  //!
  //! \return an h128_t value that consist of a pair of uint64 numbers
  //!         first is the high 64bits part, second is the low 64bits part
  h128_t hash128() const
  { return StringUtil::hash128(m_str); }

  //! \brief Converts from type \<T\>
  //!
  //! \param number The \<T\> type value to convert.
  //! \param _throw If set to true, throws an exception if conversion fails.
  //! \exception SOFTWARE_ERROR Thrown if conversion fails.
  template <class T>
  String& from_num(const T& number, bool _throw = true)
  {
    StringUtil::from_num<T>(&m_str, number, _throw);
    return *this;
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

  //! \name Pattern matching operations using regular expressions
  //!@{

  //! check if the string match the given regex pattern 
  //! \param pattern the regex pattern
  //! \return true if the string match the pattern
  bool re_match(const std::string& pattern) const;

  //! search a match for the given pattern
  //! \param pattern the regex pattern
  //! \param start_p if a match is found then *start_p will contains the position of the first matching character
  //! \param length_p if a match is found then *length_p will contains the number of matching characters
  //! \param start_pos start searching at the given given position
  //! \return true if a match is found
  bool re_search(const std::string& pattern, std::size_t* start_p = 0, std::size_t* length_p = 0, std::size_t start_pos = 0) const;

  //! search all occurences of a pattern and replace each
  //! \param search_for the searched pattern
  //! \param replace_by the replacement string
  //! \param first_only if true replace only the 1st occurence
  //! \param reject_non_matching if true  non matching parts are removed from the string
  String& re_replace(const std::string& search_for, const std::string& replace_by, bool first_only = false, bool reject_non_matching = false);
  //!@}

#ifndef YAT_CPP11
  //! \name some methods availables with c++ 11
  //!@{
  char& front() { return m_str[0]; }
  const char& front() const { return m_str[0]; }
  char& back() { return m_str[m_str.size()-1]; }
  const char& back() const { return m_str[m_str.size()-1]; }
  void pop_back() { m_str.erase(m_str.size()-1, 1); }
  //!@}
#endif

  //! \name Deprecated methods
  //!@{
  static String str_format(pcsz format, ...);
  int printf(pcsz format, ...);
  bool start_with(char c) const { return starts_with(c); }
  bool start_with(pcsz p, bool b=false) const { return starts_with(p, b); }
  bool end_with(char c) const { return ends_with(c); }
  bool end_with(pcsz p, bool b=false) const { return ends_with(p, b); }
  //!@}

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
//! \class Format
//! \brief Format string using a specification close to the
//! format-spec defined in Python\n
//! (https://docs.python.org/3/library/string.html#formatspec)
//!
//! Format strings contain “replacement fields” surrounded by curly braces {}.
//! Anything that is not contained in braces is considered literal text, which
//! is copied unchanged to the output.\n
//! If you need to include a brace character in the literal text, it can be
//! escaped by doubling: {{ and }}.
//!
//! The grammar for a replacement field is as follows:\n
//! \code{.unparsed}
//! replacement field ::= {[<argument index>:][<format spec>]}
//! argument_index    ::= integer
//! format_spec       ::= <see below>
//! \endcode
//! \remark argument_index have to be used on all fields or none in a given
//! format string, and, if used, must start at '0'.\n
//! Mixing manual (with argument indexes) and automatic field indexing is not allowed.
//!
//! \par Format specification mini-language
//! \code{.unparsed}
//! [[fill]align][sign][#][0][width][.precision][type][/format extension]
//!
//! fill             ::=  any character
//! align            ::=  "<" | ">"
//! sign             ::=  "+" | "-"
//! width            ::=  integer
//! precision        ::=  integer
//! type             ::=  "b" | "d" | "B" | "e" | "E" | "f" | "F" | "g" | "G" | "o" | "s" | "x" | "X" | "p" | "%"
//! format extension ::= specific format synax interpreted by Formatter functor (see below)
//! \endcode
//! \par align
//! '<' Forces the field to be left-aligned within the available space (this is the default for most objects).\n
//! '>' Forces the field to be right-aligned within the available space (this is the default for numbers).
//! \par sign
//! '+' indicates that a sign should be used for both positive as well as negative numbers.\n
//! '-' indicates that a sign should be used only for negative numbers (this is the default behavior).
//! \par '#' character
//! only valid for integers, and only for octal or hexadecimal output.
//! If present, it specifies that the output will be prefixed by '0' or '0x', respectively.
//! \par '0' character
//! For numeric types, a zero preceding the width field forces the padding,
//! using '0', to be placed after sign or base (if any) but before the digits.\n
//! This is used for printing fields in the form ‘+000000120’ or '0x00ff'.\n
//! With base conversion format (B, x/X/p, o) the field with may be ommitted and
//! the number of digits therefore depends of the integer type.
//! If the 0 character and an align option both appear, the 0 character is ignored.
//! \par type
//! 's' String format. This is the default type for strings and may be omitted.\n
//! 'd' Decimal format. Outputs the number in base 10.\n
//! 'B' Binary format. Outputs the number in base 2.\n
//! 'o' Octal format. Outputs the number in base 8.\n
//! 'x' Hex format. Outputs the number in base 16, using lower- case letters for the digits above 9.\n
//! 'X' Hex format. Outputs the number in base 16, using upper- case letters for the digits above 9.\n
//! 'e' Exponent notation. Prints the number in scientific notation using the letter ‘e’ to indicate the exponent. The default precision is 6.\n
//! 'E' Exponent notation. Same as 'e' except it uses an upper case ‘E’ as the separator character.\n
//! 'f' Fixed-point notation. Displays the number as a fixed-point number. The default precision is 6.\n
//! 'g' General format. For a given precision p >= 1, this rounds the number to p significant digits and then formats the result in either fixed-point format or in scientific notation, depending on its magnitude.\n
//! 'G' General format. Same as 'g' except switches to 'E' if the number gets too large. The representations of infinity and NaN are uppercased, too.\n
//! 'p' special type to be used only to output pointer value (ex: yat::Format('{p}').arg(my_pointer)). Use '{0p}' to not truncate zeros. A null pointeur will output 'null' rather than 0x0.\n
//! '%%' Percentage. Multiplies the number by 100 and displays in fixed ('f') format, followed by a percent sign.\n
//! \par format extension
//! If present, this part is decoded and interpreted by custom formatters
//!
//! \par Usage:
//! \code{.cpp}
//! yat::Format fmt("ga: {}, bu: {}, zo: {}, meu: {}");
//! fmt % "pomper" % true % 42 % 3.14;
//! yat::String str = fmt;
//! The logging api offer the ability to use the ',' operator for the arguments :
//! yat::fcout("ga: {}, bu: {}, zo: {}, meu: {}"), "pomper", true, 42, 3.14;
//! yat::fcout("{1:} {0:}"), "gabu", "zomeu";
//! yat::fcout("shadock={<10}").arg("gabu");
//! yat::fcout("shadock={>10}"), "zomeu";
//! yat::fcout("ga {b} zo {s}"), true, "meu";
//! yat::fcout("pct {.1%}"), 0.2;
//! yat::fcout("bu {B}"), 42;
//! yat::fcout("bu {*>#8x}"), 42;
//! yat::fcout("bu {X}"), 42;
//! yat::fcout("ga {12} {8.4} {*<+30} {!>15g} {8.7E} {>12.2f}"), 5, 65*3.47, 42, 42, 4.245684451, 42;
//! \endcode
//!
//! \par Format::Formatter\<T\>
//! The Format class use functors, called formatters, to deal with differents types.\n
//! Two default formatters are defined: for basic types and for C-pointers.\n
//! To declare a formatter outside of the yat namespace:
//! \code{.cpp}
//! namespace yat
//! {
//!   template<> struct Format::Formatter<MyClassType>
//!   {
//!     void operator()(const MyClassType& v, std::ostream &oss, const Format::Context& fc)
//!     {
//!       // calling the following method is optional
//!       prepare_format(oss, fc);
//!
//!       oss << v.some_method_returning_the_value_to_be_pushed_on_the_ostream();
//!     }
//!   };
//! } // namespace yat
//! \endcode
// ============================================================================
class YAT_DECL Format
{
public:
  //! \brief Default constructor.
  Format() : m_arg_index(0), m_section_by_arg_idx(0), m_null_fmt(NULL) {}

  //! \brief Constructor from a char pointer.
  //! \param psz Char pointer.
  //! \param throw_exception if 'true' then throw in case of error.
  Format(const char *psz, bool throw_exception = true) : m_str(psz), m_arg_index(0), m_section_by_arg_idx(0),
                            m_null_fmt(NULL)
  {
    parse(throw_exception);
  }

  //! \brief Constructor from char buffer.
  //! \param psz Char pointer.
  //! \param size Buffer size.
  //! \param throw_exception if 'true' then throw in case of error.
  Format(const char *psz, int size, bool throw_exception = true) : m_str(psz, size),
                                      m_arg_index(0), m_section_by_arg_idx(0), m_null_fmt(NULL)
  {
    parse(throw_exception);
  }

  //! \brief Copy Constructor.
  //! \param str The source string.
  //! \param throw_exception if 'true' then throw in case of error.
  Format(const String &str, bool throw_exception = true) : m_str(str), m_arg_index(0), m_section_by_arg_idx(0), m_null_fmt(NULL)
  {
    parse(throw_exception);
  }

  //! \brief Constructor from std::string.
  //! \param str The string.
  //! \param throw_exception if 'true' then throw in case of error.
  Format(const std::string &str, bool throw_exception = true) : m_str(str), m_arg_index(0),
                                   m_section_by_arg_idx(0), m_null_fmt(NULL)
  {
    parse(throw_exception);
  }

  //! Explicit conversion
  operator const std::string&() const { return get().str(); }
  operator std::string&() { return get(); }
  operator const yat::String&() const { return get(); }
  operator yat::String&() { return get(); }

  //! Get final string
  String& get() const;

  //! Get final string as c-string
  const char* cget() const;

  // ============================================================================
  //! \class Context
  //! \brief decoded standard format specification of a replacement field
  // ============================================================================
  class Context
  {
  public:
    bool empty_fmt;        //! this format spec is empty
    bool type_only_fmt;    //! only value type is set in this instance
    bool show_sign;        //! positive sign of numeric value will be printed
    bool align_sign;       //! eq. to left-alignement
    bool align_right;      //! field value should be right-aligned
    bool align_left;       //! field value should be left-aligned
    bool show_base;        //! numeric field should be preceded by base information
    bool zero_padding;     //! numeric value should be padded with '0'
    char type;             //! field type, if any
    char fill;             //! fill character
    std::size_t precision; //! float value precision
    std::size_t width;     //! field width
    yat::String fmt_ext;   //! format extension for custom formatter
    yat::String fmt_spec;  //! format specification
    yat::String field;     //! replacement field before substitution

    Context() : empty_fmt(true), type_only_fmt(false), show_sign(false),
                   align_sign(false), align_right(false), align_left(false),
                   show_base(false), zero_padding(false), type(0), fill(0),
                   precision(0), width(0)
                   {}
  };

  //! \brief Apply format
  //!
  //! \param v the value argument
  template<typename T> Format& arg(const T& v)
  {
    FieldSection *section_p = 0;
    while( get_next_format(&section_p) )
    {
      Formatter<T>()(v, m_oss, section_p->fc);
      m_replacements[PTR2INT(section_p)] = m_oss.str();
    }
    return *this;
  }

  //! \brief Apply format using shorter syntax
  //!
  //! \param v the value argument
  template<typename T> Format& operator%(const T& v)
  {
    return arg(v);
  }

  //! \brief Apply format for any pointers type
  //!
  //! Output pointer address\n
  //! 'p' output type may be omitted in format specification.
  //!
  //! \param p the pointeur value argument
  template<typename T> Format& arg(T* p)
  {
    FieldSection *section_p = 0;
    while( get_next_format(&section_p) )
    {
      output_ptr_value(reinterpret_cast<uintptr>(p), section_p->fc);
      m_replacements[PTR2INT(section_p)] = m_oss.str();
    }
    return *this;
  }

  //! \brief Apply format for any pointers type, shorter syntax
  //!
  //! Output pointer address\n
  //! 'p' output type may be omitted in format specification.
  //!
  //! \param p the pointeur value argument
  template<typename T> Format& operator%(T* p)
  {
    return arg(p);
  }

  //! \brief arg specialization for C-string pointer
  //!
  //! Output pointed string if the format spec is empty, instead of pointer address
  //! but output the pointer address if the 'p' type is specified ('{p}')
  //!
  //! \param s pointeur to a C-string
  Format& arg(const char *s);

  //! @deprecated
  template<class T> Format& format(const T& v) { return arg(v); }

private:

  // Format::output_percent_value
  template<typename T>
  static void output_percent_value(const T& v, std::ostream &oss, const Context& fc);

  // Format::output_binary_value
  template<typename T>
  static void output_binary_value(const T& v, std::ostream &oss, const Context& fc);

  // Format::output_octal_value
  template<typename T>
  static void output_octal_value(const T& v, std::ostream &oss, const Context& fc);

  // Format::output_hex_value
  template<typename T>
  static void output_hex_value(const T& v, std::ostream &oss, const Context& fc);

  // Default Formatter
  template<typename T>
  struct Formatter
  {
    void operator()(const T& v, std::ostream &oss, const Context& fc)
    {
      prepare_format(oss, fc);

      if( 'B' == fc.type )
        output_binary_value(v, oss, fc);
      else if( 'o' == fc.type )
        output_octal_value(v, oss, fc);
      else if( 'x' == fc.type || 'X' == fc.type )
        output_hex_value(v, oss, fc);
      else if( '%' == fc.type )
        output_percent_value(v, oss, fc);
      else
        oss << v;
    }
  };

  // The whole format string is split into 'sections':
  // - 'strings' to be output as they are
  // - 'replacement fields' to be replaced by argument values before output
  enum SectionType { replacement_field, string };

  // Generic section base class
  class Section
  {
  public:
    virtual SectionType type() = 0;
    virtual ~Section() {}
  };
  typedef SharedPtr<Section> SectionPtr;
  typedef std::vector<SectionPtr> Sections;
  typedef std::map<std::size_t, Sections> MapArgSections;
  typedef std::map<uintptr, std::string> ReplacementMap;

  class StringSection : public Section
  {
  public:
    SectionType type() { return string; }
    std::string str;
  };

  class FieldSection : public Section
  {
  public:
    SectionType type() { return replacement_field; }
    Context fc;
  };

  struct CompiledFormat
  {
    Sections sections;
    MapArgSections arg_sections_map;
    std::size_t  expected_arg_count;
    CompiledFormat() : expected_arg_count(0) {}
  };
  typedef SharedPtr<CompiledFormat> CompiledFormatPtr;

  class Cache
  {
  public:
    static bool get(const yat::String& fmt, h64_t& h, CompiledFormatPtr& entry_ptr);
    static void insert(h64_t h, CompiledFormatPtr& entry_ptr);

  private:
    static Cache& instance();
    std::map<h64_t, CompiledFormatPtr> m_cache;
    Mutex m_mtx;
  };

private:
  CompiledFormatPtr m_compiled_fmt_ptr;
  mutable yat::String m_str;
  std::size_t m_arg_index;
  std::size_t m_section_by_arg_idx;
  ReplacementMap m_replacements;
  std::ios m_null_fmt;
  std::ostringstream m_oss;

  FieldSection* parse_field(yat::String& field, int* arg_index_p);
  bool get_next_format(FieldSection** pp_fmt);
  void parse(bool throw_exception);
  void output_ptr_value(uintptr p, const Context& fc);
  static void prepare_type(std::ios_base::fmtflags& ff, std::ostream &oss, const char& type);

public:
  //! \brief setup ostream according to the format context
  //!
  //! This static method can be called by formatters
  //!
  //! \param oss the ostream
  //! \param fc  the context
  //! \param default_type proposed default type if none is specified in the context
  //!        all type may be invoked except 'B' & '%'
  //! \param force_type default_type will override the type specified in the context
  static void prepare_format(std::ostream &oss, const Context& fc,
                             char default_type = 0, bool force_type = false);
};

//==================================================================================================
//! \brief fail-safe version of the Format class
//!
//! SFormat object will not throw exception when parsing the format string
//! It's aimed to be used in logging apis for example
//==================================================================================================
class YAT_DECL SFormat : public Format
{
public:
  //! \brief Constructor from a char pointer.
  //! \param psz Char pointer.
  //! \param throw_exception if 'true' then throw in case of error.
  SFormat(const char *psz) : Format(psz, false) { }

  //! \brief Constructor from char buffer.
  //! \param psz Char pointer.
  //! \param size Buffer size.
  //! \param throw_exception if 'true' then throw in case of error.
  SFormat(const char *psz, int size) : Format(psz, size, false) { }

  //! \brief Copy Constructor.
  //! \param str The source string.
  //! \param throw_exception if 'true' then throw in case of error.
  SFormat(const String &str) : Format(str, false) { }

  //! \brief Constructor from std::string.
  //! \param str The string.
  //! \param throw_exception if 'true' then throw in case of error.
  SFormat(const std::string &str) : Format(str, false) { }
};

//==================================================================================================
//! \brief for using with yat::Format class
//!
//! No specific format syntax defined for this formatter\n
//! Output the SharedPtr instance address\n
//! To output address of the pointed objet, call arg with ptr.get()\n
//! To output the pointed objet string representation, call arg with *ptr
//==================================================================================================
template <typename T, typename L>
struct Format::Formatter<SharedPtr<T,L>>
{
  void operator()(const SharedPtr<T,L>& ptr, std::ostream &oss, const Format::Context& fc)
  {
    // setup width & alignment
    prepare_format(oss, fc, 'p');

    oss << this;
  }
};

//==================================================================================================
//! \brief for using with yat::Error class
//!
//! No specific format syntax defined for this formatter\n
//! Output the three values of an exception\n
//==================================================================================================
template <>
struct Format::Formatter<Error>
{
  void operator()(const Error& err, std::ostream &oss, const Format::Context& fc)
  {
    // setup width & alignment
    prepare_format(oss, fc, 's');

    if( !fc.fmt_ext.empty() )
    {
      if( fc.fmt_ext[0] == 'r' )
        oss << err.reason;
      else if( fc.fmt_ext[0] == 'd' )
        oss << err.desc;
      else if( fc.fmt_ext[0] == 'o' )
        oss << err.origin;
    }
    else
      oss << "what: " << err.reason << ". why: " << err.desc << ". where: " << err.origin;
  }
};

//! @deprecated original class name
typedef Format StringFormat;

} // namespace

#include <yat/utils/String.tpp>

#endif
