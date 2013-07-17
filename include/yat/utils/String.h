//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2012  The Tango Community
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
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */


#ifndef __YAT_STRING_H__
#define __YAT_STRING_H__

#include <yat/CommonHeader.h>

// STL headers
#include <string>
#include <vector>

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
  #define PSZ_FMT(...) yat::String::str_format(__VA_ARGS__).c_str()
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

// ============================================================================
//! \class StringWork
//! \brief Manipulations on std::string class object.
//! This class contains a set of string high-level manipulations algorithms. 
//! It uses a reference on an existing std::string object.
//! This class should replace the deprecated yat::String
//! \verbatim
//! std::string str =" foo-bar ";
//! yat::StringWork strw(str);
//! strw.trim();
//! std::out << str;
//! \endverbatim
// ============================================================================
class YAT_DECL StringWork
{
public:

  //! \brief Empty string - useful when need a const string &.
  static const std::string empty;

  //! \brief Constructor from std::string object
  //!        get a reference on it
  //! \param a_string the string object
  //! \warning the std::string object must not be deleted before this objet dies
  StringWork(std::string& a_string) : the_string_(a_string) 
  { }

  //! \brief get access to the std::std::string object
  //!        in order to use natives methods
  std::string& get() { return the_string_; }
  
  //! \brief conversion operator
  operator std::string&() { return the_string_; }
  
  //! \brief assignement operator
  StringWork& operator=(const StringWork& other);
  
  //! \brief assignement operator
  StringWork& operator=(const std::string& other);
  
  //! \brief Builds a std::string with a C-std::string like format.
  //! \param pszFormat The std::string format.
  //! \param ... The std::string.
  static std::string str_format(pcsz pszFormat, ...);

  //! \brief Compares std::strings (i.e. operator==).
  //!
  //! Returns true if std::strings are equal, false otherwise.
  //! \param str The source std::string.
  bool is_equal(const std::string &str) const;

  //! \brief Compares std::string in a no case sensitive way.
  //!
  //! Returns true if std::strings are equal, false otherwise.
  //! \param str The source std::string.
  bool is_equal_no_case(const std::string &str) const;

  //! \brief Tests the first character.
  //!
  //! Returns true if the std::string starts with this character, false otherwise.
  //! \param c The character.
  bool start_with(char c) const;
  
  //! \brief Tests the first character with that of another std::string.
  //!
  //! Returns true if the std::strings start with the same character, false otherwise.
  //! \param pcszStart The source std::string.
  //! \param bNoCase If set to true, the test is not case sensitive.
  bool start_with(pcsz pcszStart, bool bNoCase=false) const;
  
  //! \brief Tests the last character.
  //!
  //! Returns true if the std::string ends with this character, false otherwise.
  //! \param c The character.
  bool end_with(char c) const;
  
  //! \brief Tests the last character with that of another std::string.
  //!
  //! Returns true if the std::strings end with the same character, false otherwise.
  //! \param pcszEnd The source std::string.
  //! \param bNoCase If set to true, the test is not case sensitive.
  bool end_with(pcsz pcszEnd, bool bNoCase=false) const;

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
  ExtractTokenRes extract_token(char c, std::string *pstrToken);
  
  //! \brief Looks for a token, from right to left.
  //! 
  //! Returns the extraction status.\n
  //! The function looks for the first separator occurrence. The extracted token is removed from the std::string.
  //! \param c Separator.
  //! \param[out] pstrToken Extracted token.
  ExtractTokenRes extract_token_right(char c, std::string *pstrToken);
  
  //! \brief Looks for enclosed token, from left to right.
  //! 
  //! Returns the extraction status.\n
  //! The function looks for the first left separator occurrence. The extracted token is removed from the std::string.
  //! \param cLeft Left separator.
  //! \param cRight Right separator.
  //! \param[out] pstrToken Extracted token.
  ExtractTokenRes extract_token(char cLeft, char cRight, std::string *pstrToken);
  
  //! \brief Looks for enclosed token, from right to left.
  //! 
  //! Returns the extraction status.\n
  //! The function looks for the first left separator occurrence. The extracted token is removed from the std::string.
  //! \param cLeft Left separator.
  //! \param cRight Right separator.
  //! \param[out] pstrToken Extracted token.
  ExtractTokenRes extract_token_right(char cLeft, char cRight, std::string *pstrToken);
  
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
  bool remove_enclosure(psz pszLeft, psz pszRight);

  //! \brief Removes characters that enclose std::string: quotes, parenthesis, etc...
  //!
  //! Returns true if enclosure was removed.
  //! ex: RemoveEnclosure("'", "'") -\> removes quotes in a std::string like 'std::string'.
  //! \param cLeft Left enclosure character.
  //! \param cRight Right enclosure character.
  bool remove_enclosure(char cLeft, char cRight);

  //! \brief Tests if std::string matches with mask containing '*' and '?' jokers.
  //!
  //! Returns true if std::string matches the mask, false otherwise. \n
  //! The mask can contain:
  //! - '*': match any number of characters.
  //! - '?': match one character.
  //! \param pszMask The mask.
  bool match(pcsz pszMask) const;

  //! \brief Tests if std::string matches with mask containing '*' and '?' jokers.
  //!
  //! Returns true if std::string matches the mask, false otherwise. \n
  //! The mask can contain:
  //! - '*': match any number of characters.
  //! - '?': match one character.
  //! \param strMask The mask.
  bool match(const std::string &strMask) const;

  //! \brief Removes white spaces at beginning and end of std::string.
  void trim();

  //! \brief Builds a std::string with the specified format.
  //!
  //! Returns the std::string size.
  //! \param pszFormat The format.
  //! \param ... The std::string.
  int printf(pcsz pszFormat, ...);

  //! \brief Splits the std::string.
  //!
  //! The std::string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the std::string vector, while *this* std::string is
  //! preserved.
  //! \param c Separator.
  //! \param[out] pvecstr Pointer to a vector of std::strings.
  //! \param bClearVector If set to true, the vector is cleared.
  void split(char c, std::vector<std::string> *pvecstr, bool bClearVector=true);

  //! \brief Splits the std::string.
  //!
  //! The std::string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the std::string vector, while *this* std::string is
  //! preserved.
  //! \param c Separator.
  //! \param[out] pvecstr Pointer to a vector of std::strings.
  //! \param bClearVector If set to true, the vector is cleared.
  void split(char c, std::vector<std::string> *pvecstr, bool bClearVector=true) const;

  //! \brief Splits the std::string.
  //!
  //! The std::string is split in 2 tokens separated with the specified separator.
  //!
  //! \param c Separator.
  //! \param[out] pstrLeft Left part of the split std::string.
  //! \param[out] pstrRight Right part of the split std::string.
  //! \param bPreserve If set to true, *this* std::string is preserved, else, *this* is
  //! the left part of the split std::string.
  void split(char c, std::string *pstrLeft, std::string *pstrRight, bool bPreserve=false);

  //! \brief Joins std::strings from a std::string vector, using specified separator.
  //!
  //! Replaces *this* std::string with the result.
  //! For instance: join (\<str1, str2\>, ";") gives: str1;str2 
  //! \param vecStr The source vector.
  //! \param cSep %std::string separator.
  void join(const std::vector<std::string> &vecStr, char cSep=',');

  //! \brief Removes items separated by a specific separator.
  //!
  //! For instance: std::string like "item1,item2,item3,...". \n
  //! Returns true if any item was found, false otherwise.
  //! \param strItem Item to find and remove.
  //! \param cSep Items separator.
  bool remove_item(const std::string &strItem, char cSep=',');

  //! \brief Converts std::string to lowercase.
  void to_lower();

  //! \brief Converts std::string to uppercase.
  void to_upper();

  //! \brief Finds and replaces a std::string.
  //! \param pszSrc %std::string to replace.
  //! \param pszDst Substitution std::string.
  void replace(pcsz pszSrc, pcsz pszDst);

  //! \brief Finds and replaces one character.
  //!
  //! \param cSrc Character to replace.
  //! \param cDst Substitution character.
  void replace(char cSrc, char cDst);

  //! \brief Returns a 32 bits hash code.
  uint32 hash() const;
  
  //! \brief Returns the hash code using the FNV-1a algorithm.
  //!
  //! Calculates a 64 bits hash code. See details of the algorithm 
  //! on http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function.
  uint64 hash64() const;

  //! \brief Converts string content to numeric type \<_T\>.
  //! 
  //! Returns the converted string in numeric type \<_T\>.
  //! Should also work for any "istringstream::operator>>" supported type.
  //! \param _s The string to convert.
  //! \param _throw If set to true, throws an exception if conversion fails.
  //! \exception SOFTWARE_ERROR Thrown if conversion fails.
  template <class T>
  T to_num(bool _throw = true)
  {
    ISStream iss(the_string_.c_str());

    T num_val;

    if( (iss >> num_val) == false )
    {
      if( _throw )
      {
        OSStream desc;
        desc << "conversion from string to num failed [" 
             << the_string_
             << "]"
             << std::ends;
        THROW_YAT_ERROR ("SOFTWARE_ERROR",
                         desc.str().c_str(),
                         "StringWork::to_num");
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
  void from_num(const T& number, bool _throw = true)
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
                         "StringWork::from_num");
      }
    }

    the_string_ = oss.str();
  } 
  
private:
  std::string& the_string_;
};

// ============================================================================
//! \class String 
//! \brief Extended string class.
//! \deprecated use yat::StringWork instead
//!
//! This class is an extension of the std::string class: it provides additional
//! string manipulation functions, such as token extraction, enclosure deletion, 
//! find and replace function, ...
//!
//! Inherits from std::string class.
// ============================================================================
class YAT_DECL String : public std::string
{
public:

  //! \brief Empty string - useful when need a const string &.
  static const String nil;

  //! \name Constructors
  //@{ 

  //! \brief Default constructor.
  String() : std::string()
  {}

  //! \brief Constructor from a char pointer.
  //! \param psz Char pointer.
  String(const char *psz)
  {
    if( NULL == psz )
      erase();
    else
      append(psz);
  }

  //! \brief Constructor from char buffer.
  //! \param psz Char pointer.
  //! \param iSize %Buffer size.
  String(const char *psz, int iSize)
  {
    if( NULL == psz )
      erase();
    else
      append(psz, iSize);
  }

  //! \brief Copy Constructor.
  //! \param str The source string.
  String(const String &str) : std::string(str) {}

  //! \brief Constructor from std::string.
  //! \param str The string.
  String(const std::string &str) : std::string(str) {}
  //@}

  //! \brief Builds a string with a C-string like format.
  //! \param pszFormat The string format.
  //! \param ... The string.
  static String str_format(pcsz pszFormat, ...);

  //! \brief Compares strings (i.e. operator==).
  //!
  //! Returns true if strings are equal, false otherwise.
  //! \param str The source string.
  bool is_equal(const String &str) const;

  //! \brief Compares string in a no case sensitive way.
  //!
  //! Returns true if strings are equal, false otherwise.
  //! \param str The source string.
  bool is_equal_no_case(const String &str) const;

  //! \brief Tests the first character.
  //!
  //! Returns true if the string starts with this character, false otherwise.
  //! \param c The character.
  bool start_with(char c) const;
  
  //! \brief Tests the first character with that of another string.
  //!
  //! Returns true if the strings start with the same character, false otherwise.
  //! \param pcszStart The source string.
  //! \param bNoCase If set to true, the test is not case sensitive.
  bool start_with(pcsz pcszStart, bool bNoCase=false) const;
  
  //! \brief Tests the last character.
  //!
  //! Returns true if the string ends with this character, false otherwise.
  //! \param c The character.
  bool end_with(char c) const;
  
  //! \brief Tests the last character with that of another string.
  //!
  //! Returns true if the strings end with the same character, false otherwise.
  //! \param pcszEnd The source string.
  //! \param bNoCase If set to true, the test is not case sensitive.
  bool end_with(pcsz pcszEnd, bool bNoCase=false) const;

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
  ExtractTokenRes extract_token(char c, String *pstrToken);
  
  //! \brief Looks for a token, from right to left.
  //! 
  //! Returns the extraction status.\n
  //! The function looks for the first separator occurrence. The extracted token is removed from the string.
  //! \param c Separator.
  //! \param[out] pstrToken Extracted token.
  ExtractTokenRes extract_token_right(char c, String *pstrToken);
  
  //! \brief Looks for enclosed token, from left to right.
  //! 
  //! Returns the extraction status.\n
  //! The function looks for the first left separator occurrence. The extracted token is removed from the string.
  //! \param cLeft Left separator.
  //! \param cRight Right separator.
  //! \param[out] pstrToken Extracted token.
  ExtractTokenRes extract_token(char cLeft, char cRight, String *pstrToken);
  
  //! \brief Looks for enclosed token, from right to left.
  //! 
  //! Returns the extraction status.\n
  //! The function looks for the first left separator occurrence. The extracted token is removed from the string.
  //! \param cLeft Left separator.
  //! \param cRight Right separator.
  //! \param[out] pstrToken Extracted token.
  ExtractTokenRes extract_token_right(char cLeft, char cRight, String *pstrToken);
  
  //@}

  //! \brief Removes characters that enclose string: quotes, parenthesis, etc...
  //!
  //! Returns true if enclosure was removed.
  //! ex: RemoveEnclosure("'", "'") -\> removes quotes in a string like 'string'.
  //! ex: RemoveEnclosure("(\[", ")\]") -\> removes parenthesis in a string like (string) or \[string\]
  //! but not in string like (string\].
  //! \param pszLeft List of possible left enclosure characters.
  //! \param pszRight List of possible right enclosure characters.
  //! \remark \<pszLeft\> and \<pszRight\> must have the same length.
  bool remove_enclosure(psz pszLeft, psz pszRight);

  //! \brief Removes characters that enclose string: quotes, parenthesis, etc...
  //!
  //! Returns true if enclosure was removed.
  //! ex: RemoveEnclosure("'", "'") -\> removes quotes in a string like 'string'.
  //! \param cLeft Left enclosure character.
  //! \param cRight Right enclosure character.
  bool remove_enclosure(char cLeft, char cRight);

  //! \brief Tests if string matches with mask containing '*' and '?' jokers.
  //!
  //! Returns true if string matches the mask, false otherwise. \n
  //! The mask can contain:
  //! - '*': match any number of characters.
  //! - '?': match one character.
  //! \param pszMask The mask.
  bool match(pcsz pszMask) const;

  //! \brief Tests if string matches with mask containing '*' and '?' jokers.
  //!
  //! Returns true if string matches the mask, false otherwise. \n
  //! The mask can contain:
  //! - '*': match any number of characters.
  //! - '?': match one character.
  //! \param strMask The mask.
  bool match(const String &strMask) const;

  //! \brief Removes white spaces at beginning and end of string.
  void trim();

  //! \brief Builds a string with the specified format.
  //!
  //! Returns the string size.
  //! \param pszFormat The format.
  //! \param ... The string.
  int printf(pcsz pszFormat, ...);

  //! \brief Splits the string.
  //!
  //! The string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the string vector, while *this* string is
  //! preserved.
  //! \param c Separator.
  //! \param[out] pvecstr Pointer to a vector of strings.
  //! \param bClearVector If set to true, the vector is cleared.
  void split(char c, std::vector<String> *pvecstr, bool bClearVector=true);

  //! \brief Splits the string.
  //!
  //! The string is split in tokens separated with the specified separator.
  //! The extracted tokens are put in the string vector, while *this* string is
  //! preserved.
  //! \param c Separator.
  //! \param[out] pvecstr Pointer to a vector of strings.
  //! \param bClearVector If set to true, the vector is cleared.
  void split(char c, std::vector<String> *pvecstr, bool bClearVector=true) const;

  //! \brief Splits the string.
  //!
  //! The string is split in 2 tokens separated with the specified separator.
  //!
  //! \param c Separator.
  //! \param[out] pstrLeft Left part of the split string.
  //! \param[out] pstrRight Right part of the split string.
  //! \param bPreserve If set to true, *this* string is preserved, else, *this* is
  //! the left part of the split string.
  void split(char c, String *pstrLeft, String *pstrRight, bool bPreserve=false);

  //! \brief Joins strings from a string vector, using specified separator.
  //!
  //! Replaces *this* string with the result.
  //! For instance: join (\<str1, str2\>, ";") gives: str1;str2 
  //! \param vecStr The source vector.
  //! \param cSep %String separator.
  void join(const std::vector<String> &vecStr, char cSep=',');

  //! \brief Removes items separated by a specific separator.
  //!
  //! For instance: string like "item1,item2,item3,...". \n
  //! Returns true if any item was found, false otherwise.
  //! \param strItem Item to find and remove.
  //! \param cSep Items separator.
  bool remove_item(const String &strItem, char cSep=',');

  //! \brief Converts string to lowercase.
  void to_lower();

  //! \brief Converts string to uppercase.
  void to_upper();

  //! \brief Finds and replaces a string.
  //! \param pszSrc %String to replace.
  //! \param pszDst Substitution string.
  void replace(pcsz pszSrc, pcsz pszDst);

  //! \brief Finds and replaces one character.
  //!
  //! \param cSrc Character to replace.
  //! \param cDst Substitution character.
  void replace(char cSrc, char cDst);

  //! \brief Returns a 32 bits hash code.
  uint32 hash() const;
  
  //! \brief Returns the hash code using the FNV-1a algorithm.
  //!
  //! Calculates a 64 bits hash code. See details of the algorithm 
  //! on http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function.
  uint64 hash64() const;
};

// Simple scratch buffer (not thread safe)
extern const int g_iScratchLen;
extern char g_acScratchBuf[];

} // namespace

#endif