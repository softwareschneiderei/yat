//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2011  The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// Ramon Sune (ALBA) for the yat::Signal class 
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
 * \author S.Poirier - Synchrotron SOLEIL
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

//=============================================================================
/// Extended string class
///
//=============================================================================
class YAT_DECL String : public std::string
{
public:

  /// Empty string - useful when need a const string &
  static const String nil;

  //@{ Constructors
  String() : std::string()
  {}

  String(const char *psz)
  {
    if( NULL == psz )
      erase();
    else
      append(psz);
  }

  String(const char *psz, int iSize)
  {
    if( NULL == psz )
      erase();
    else
      append(psz, iSize);
  }

  String(const String &str) : std::string(str) {}

  String(const std::string &str) : std::string(str) {}
  //@}

  /// Build a string with a C-string like format
  /// @param pszFormat The format
  static String str_format(pcsz pszFormat, ...);

  /// Compare strings (eq. '==' operator)
  bool is_equal(const String &str) const;

  /// Compare string in a no case sensitive way
  bool is_equal_no_case(const String &str) const;

  /// Test first character
  bool start_with(char c) const;
  
  /// Test first characters with other string
  bool start_with(pcsz pcszStart, bool bNoCase=false) const;
  
  /// Test last character
  bool end_with(char c) const;
  
  /// Test last character
  bool end_with(pcsz pcszEnd, bool bNoCase=false) const;

  //@{ Tokens Extraction

  /// extract_token methods family results
  enum ExtractTokenRes
  {
    EMPTY_STRING=0, /// nothing extracted
    SEP_FOUND,      /// string extracted and separator found
    SEP_NOT_FOUND   /// string extracted and separator not found
  };
	
  /// Search token from left to right
  /// @param c Separator
  /// @param pstrToken String object receiving the extracted token
  ExtractTokenRes extract_token(char c, String *pstrToken);
  
  /// Search token from right to left
  /// @param c Separator
  /// @param pstrToken String object receiving the extracted token
  ExtractTokenRes extract_token_right(char c, String *pstrToken);
  
  /// Search enclosed token from left to right
  /// @param cLeft Left separator
  /// @param cRight Right separator
  /// @param pstrToken String object receiving the extracted token
  ExtractTokenRes extract_token(char cLeft, char cRight, String *pstrToken);
  
  /// Search enclosed token from right to left
  /// @param cLeft Left separator
  /// @param cRight Right separator
  /// @param pstrToken String object receiving the extracted token
  ExtractTokenRes extract_token_right(char cLeft, char cRight, String *pstrToken);
  
  //@}

  /// Remove characters that enclose string: quotes, paranthesis, etc...
  /// ex: RemoveEnclosure("'", "'") -> removes quotes in a string like 'string'
  /// ex: RemoveEnclosure("([", ")]") -> removes paranthesis in a string like (string) or [string]
  ///                                    but not in string like (string]
  /// @param pcszLeft list of possible left enclosure chars
  /// @param pcszRight list of possible right enclosure chars
  /// @return true if enclosure was removed
  bool remove_enclosure(psz pszLeft, psz pszRight);
  bool remove_enclosure(char cLeft, char cRight);

  /// Match string with mask containing '*' and '?' jokers
  /// '*' match any number of characters
  /// '?' match one character
  bool match(pcsz pszMask) const;
  bool match(const String &strMask) const;

  /// Remove white space and begining and end of string
  void trim();

  /// Build a string with format
  int printf(pcsz pszFormat, ...);

  /// Split string
  ///
  /// @param c Separator
  /// @param pvecstr pointer to a vector of strings
  ///
  void split(char c, std::vector<String> *pvecstr, bool bClearVector=true);
  void split(char c, std::vector<String> *pvecstr, bool bClearVector=true) const;
  void split(char c, String *pstrLeft, String *pstrRight, bool bPreserve=false);

  /// Join strings from string vector
  ///
  /// @param cSep Items separator
  void join(const std::vector<String> &vecStr, char cSep=',');

  /// Remove item in a string like "item1,item2,item3,..."
  /// @param cSep Items separator
  /// @return true if the item was found, otherwise false
  bool remove_item(const String &strItem, char cSep=',');

  /// Convert characters to lowercase
  void to_lower();

  /// Convert string to uppercase
  void to_upper();

  /// Find and replace
  /// @param pszSrc Substring to replace
  /// @param pszDst Substitution string
  void replace(pcsz pszSrc, pcsz pszDst);

  /// Find and replace one character
  ///
  /// @param cSrc Character to replace
  /// @param cDst Substitution Character
  void replace(char cSrc, char cDst);

  /// Returns a hash code
  uint32 hash() const;
  
  /// Returns the hash code using the FNV-1a algorithm (http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function)
  uint64 hash64() const;
};

// Simple scratch buffer (not thread safe)
extern const int g_iScratchLen;
extern char g_acScratchBuf[];

} // namespace

#endif