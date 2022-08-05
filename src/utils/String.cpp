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

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <yat/threading/Mutex.h>
#include <yat/utils/String.h>

// stricmp function
#ifdef YAT_LINUX
int stricmp(const char * s1, const char * s2)
{
  while( *s1 )
  {
    if( *s2 == 0 )
      return 1;

    if( toupper(*s1) < toupper(*s2) )
      return -1;
    if( toupper(*s1) > toupper(*s2) )
      return 1;
    s1++;
    s2++;
}

  if( *s2 == 0 )
    return 0;
  return -1;
}

int strnicmp(const char * s1, const char * s2, int maxlen)
{
  int i = 0;
  while( i<maxlen )
  {
    if( *s2 == 0 || *s1 == 0 )
      return 1;

    if( toupper(*s1) < toupper(*s2) )
      return -1;
    if( toupper(*s1) > toupper(*s2) )
      return 1;
    s1++;
    s2++;
    i++;
  }
  return 0;
}
#endif

namespace yat
{

//=============================================================================
// String
//=============================================================================
const std::string StringUtil::empty = "";
#define BUF_LEN 262144 // 256Ko buffer

//---------------------------------------------------------------------------
// StringUtil::extract_token
//---------------------------------------------------------------------------
StringUtil::ExtractTokenRes StringUtil::extract_token(std::string* str_p, char c,
                                                      std::string *pstrToken,
                                                      char escape_char)
{
  // Cannot extract a substring a put it in the same string !
  if( str_p == pstrToken )
    return EMPTY_STRING;

  int iSrcLength = (*str_p).length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return EMPTY_STRING;
  }

  int iPos = str_p->find(c, 0);
  while( escape_char != '\0' && iPos > 0 && escape_char == (*str_p)[iPos-1] )
  {
    iPos = str_p->find(c, iPos + 1);
  }

  StringUtil::ExtractTokenRes ret_val = SEP_FOUND;
  // Search for separator
  if( iPos < 0 )
  {
    // Not found
    *pstrToken = (*str_p);
    (*str_p).clear();
    ret_val = SEP_NOT_FOUND;
  }
  else
  {
    // Separator found
    *pstrToken = str_p->substr(0, iPos);
    str_p->erase(0, iPos+1);
  }

  while( escape_char != '\0' )
  {
    std::size_t esc = pstrToken->find(escape_char);

    if( esc != std::string::npos && esc < pstrToken->length() - 1 &&
        (*pstrToken)[esc+1] == c )
    {
      *pstrToken = pstrToken->substr(0, esc) + pstrToken->substr(esc + 1);
    }
    else
      break;
  }

  return ret_val;
}

//---------------------------------------------------------------------------
// deprecated
//---------------------------------------------------------------------------
StringUtil::ExtractTokenRes StringUtil::extract_token(std::string* str_p, char c,
                                                      std::string *pstrToken)
{
  return extract_token(str_p, c, pstrToken, '\0');
}
StringUtil::ExtractTokenRes StringUtil::extract_token(std::string* str_p, char c,
                                                      std::string *pstrToken,
                                                      bool apply_escape)
{
  return extract_token(str_p, c, pstrToken, apply_escape ? '\\' : '\0');
}

//---------------------------------------------------------------------------
// StringUtil::split
//---------------------------------------------------------------------------
void StringUtil::split(std::string* str_p, char c, std::string *pstrLeft,
                       std::string *pstrRight, bool trim)
{
  if( SEP_NOT_FOUND == extract_token_right(str_p, c, pstrRight, true) )
  {
    *str_p = *pstrRight; // push back original string content
    pstrRight->clear();
  }
  else
    (*pstrLeft) = (*str_p);

  if( trim )
  {
    StringUtil::trim(pstrLeft);
    StringUtil::trim(pstrRight);
  }
}

//---------------------------------------------------------------------------
// StringUtil::split
//---------------------------------------------------------------------------
void StringUtil::split(const std::string& str, char c, std::string *pstrLeft,
                       std::string *pstrRight, bool trim)
{
  std::string tmp(str);
  if( SEP_NOT_FOUND == extract_token_right(&tmp, c, pstrRight, true) )
  {
    pstrRight->clear();
  }
  else
    (*pstrLeft) = tmp;

  if( trim )
  {
    StringUtil::trim(pstrLeft);
    StringUtil::trim(pstrRight);
  }
}

//---------------------------------------------------------------------------
// StringUtil::extract_token_right
//---------------------------------------------------------------------------
StringUtil::ExtractTokenRes StringUtil::extract_token_right(std::string* str_p, char c,
                                                            std::string *pstrToken,
                                                            char escape_char)
{
  // Cannot extract a substring a put it in the same string !
  if( str_p == pstrToken )
    return EMPTY_STRING;

  int iSrcLength = str_p->length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return EMPTY_STRING;
  }

  // Search for separator
  int iPos = str_p->rfind(c, str_p->length() - 1);
  while( escape_char != '\0' && iPos > 0 && escape_char == (*str_p)[iPos-1] )
  {
    iPos = str_p->rfind(c, iPos - 1);
  }

  StringUtil::ExtractTokenRes ret_val = SEP_FOUND;

  if( iPos < 0 )
  {
    // Not found
    *pstrToken = *str_p;
    str_p->clear();
    ret_val = SEP_NOT_FOUND;
  }
  else
  {
    // Separator found
    *pstrToken = str_p->substr(iPos+1);
    str_p->erase(iPos);
  }

  while( escape_char != '\0' )
  {
    std::size_t esc = pstrToken->find(escape_char);

    if( esc != std::string::npos && esc < pstrToken->length() - 1 &&
        (*pstrToken)[esc+1] == c )
    {
      *pstrToken = pstrToken->substr(0, esc) + pstrToken->substr(esc + 1);
    }
    else
      break;
  }

  return ret_val;
}

//---------------------------------------------------------------------------
// deprecated
//---------------------------------------------------------------------------
StringUtil::ExtractTokenRes StringUtil::extract_token_right(std::string* str_p, char c,
                                                            std::string *pstrToken,
                                                            bool apply_escape)
{
  return extract_token_right(str_p, c, pstrToken, apply_escape ? '\\' : '\0');
}
StringUtil::ExtractTokenRes StringUtil::extract_token_right(std::string* str_p, char c,
                                                            std::string *pstrToken)
{
  return extract_token_right(str_p, c, pstrToken, '\0');
}

//---------------------------------------------------------------------------
// StringUtil::extract_token
//---------------------------------------------------------------------------
StringUtil::ExtractTokenRes StringUtil::extract_token(std::string* str_p, char cLeft,
                                                      char cRight, std::string *pstrToken,
                                                      char escape_char)
{
  // Cannot extract a substring a put it in the same string !
  if( str_p == pstrToken )
    return EMPTY_STRING;

  int iSrcLength = str_p->length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return EMPTY_STRING;
  }

  // Search for enclosing characters
  int iLeftPos = str_p->find(cLeft);
  while( iLeftPos > 0 && escape_char == (*str_p)[iLeftPos-1] )
    iLeftPos = str_p->find(cLeft, iLeftPos + 1);

  int iRightPos = str_p->find(cRight, iLeftPos + 1);
  while( iRightPos > 0 && escape_char == (*str_p)[iRightPos-1] )
    iRightPos = str_p->find(cRight, iRightPos + 1);

  if( iLeftPos < 0 || iRightPos < 0 || iRightPos < iLeftPos )
  {
    // Not found
    pstrToken->clear();
    return SEP_NOT_FOUND;
  }

  // Enclosing characters found
  *pstrToken = str_p->substr(iLeftPos + 1, iRightPos - iLeftPos - 1);

  while( escape_char != '\0' )
  {
    bool b = false;
    std::size_t esc;
    esc = pstrToken->find(escape_char);
    if( pstrToken->find(cLeft) == esc + 1 )
    {
        b = true;
        pstrToken->erase(esc, 1);
    }
    esc = pstrToken->find(escape_char);
    if( pstrToken->find(cRight) == esc + 1 )
    {
        b = true;
        pstrToken->erase(esc, 1);
    }

   if( !b )
      break;
  }

  str_p->erase(0, iRightPos - iLeftPos + 1);
  return SEP_FOUND;
}

//---------------------------------------------------------------------------
// deprecated
//---------------------------------------------------------------------------
StringUtil::ExtractTokenRes StringUtil::extract_token(std::string* str_p, char cLeft,
                                                      char cRight, std::string *pstrToken,
                                                      bool apply_escape)
{
  return extract_token(str_p, cLeft, cRight, pstrToken, apply_escape ? '\\' : '\0');
}
StringUtil::ExtractTokenRes StringUtil::extract_token(std::string* str_p, char cLeft,
                                                      char cRight, std::string *pstrToken)
{
  return extract_token(str_p, cLeft, cRight, pstrToken, '\0');
}

//---------------------------------------------------------------------------
// StringUtil::extract_token_right
//---------------------------------------------------------------------------
StringUtil::ExtractTokenRes StringUtil::extract_token_right(std::string* str_p, char cLeft,
                                                            char cRight, std::string *pstrToken,
                                                            char escape_char)
{
  // Cannot extract a substring a put it in the same string !
  if( str_p == pstrToken )
    return EMPTY_STRING;

  int iSrcLength = str_p->length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return EMPTY_STRING;
  }

  // Search for enclosing characters
  int iRightPos = str_p->rfind(cRight);
  while( escape_char != '\0' && iRightPos > 0 && escape_char == (*str_p)[iRightPos-1] )
    iRightPos = str_p->rfind(cRight, iRightPos - 1);

  int iLeftPos = iRightPos > 0 ? (int)str_p->rfind(cLeft, iRightPos - 1) : -1;
  while( escape_char != '\0' && iLeftPos > 0 && escape_char == (*str_p)[iLeftPos-1] )
    iLeftPos = str_p->rfind(cLeft, iLeftPos - 1);

  if( iLeftPos < 0 || iRightPos < 0 || iRightPos < iLeftPos )
  {
    // Not found
    *pstrToken = empty;
    return SEP_NOT_FOUND;
  }

  // Enclosing characters found
  *pstrToken = str_p->substr(iLeftPos+1, iRightPos - iLeftPos - 1);

  while( escape_char != '\0' )
  {
    bool b = false;
    std::size_t esc;
    esc = pstrToken->find(escape_char);
    if( pstrToken->find(cLeft) == esc + 1 )
    {
        b = true;
        pstrToken->erase(esc, 1);
    }
    esc = pstrToken->find(escape_char);
    if( pstrToken->find(cRight) == esc + 1 )
    {
        b = true;
        pstrToken->erase(esc, 1);
    }

   if( !b )
      break;
  }

  str_p->erase(iLeftPos);
  return SEP_FOUND;
}

//---------------------------------------------------------------------------
// deprecated
//---------------------------------------------------------------------------
StringUtil::ExtractTokenRes StringUtil::extract_token_right(std::string* str_p, char cLeft,
                                                            char cRight, std::string *pstrToken,
                                                            bool apply_escape)
{
  return extract_token_right(str_p, cLeft, cRight, pstrToken, apply_escape ? '\\' : '\0');
}
StringUtil::ExtractTokenRes StringUtil::extract_token_right(std::string* str_p, char cLeft,
                                                            char cRight, std::string *pstrToken)
{
  return extract_token_right(str_p, cLeft, cRight, pstrToken, '\0');
}

//---------------------------------------------------------------------------
// StringUtil::remove_enclosure
//---------------------------------------------------------------------------
bool StringUtil::remove_enclosure(std::string* str_p, psz pszLeft, psz pszRight)
{
  // pcszLeft & pcszRight must have the same length
  if( strlen(pszLeft) != strlen(pszRight) )
    return false;

  for( uint32 ui = 0; ui < strlen(pszLeft); ui++ )
  {
    std::string strMask;
    strMask += pszLeft[ui];
    strMask += '*';
    strMask += pszRight[ui];
    if( match(*str_p, strMask) )
    {
      (*str_p) = str_p->substr(strlen(pszLeft), (*str_p).size() - (strlen(pszLeft) + strlen(pszRight)));
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------------------
// StringUtil::remove_enclosure
//---------------------------------------------------------------------------
bool StringUtil::remove_enclosure(std::string* str_p, char cLeft, char cRight)
{
  std::string strMask;
  strMask += cLeft;
  strMask += '*';
  strMask += cRight;
  if( match(*str_p, strMask) )
  {
    (*str_p) = str_p->substr(1, (*str_p).size() - 2);
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
// StringUtil::is_equal
//---------------------------------------------------------------------------
bool StringUtil::is_equal(const std::string& str, const std::string& other)
{
  return (str == other);
}

//---------------------------------------------------------------------------
// StringUtil::is_equal_no_case
//---------------------------------------------------------------------------
bool StringUtil::is_equal_no_case(const std::string& str, const std::string& other)
{
  return (!stricmp(other.c_str(), str.c_str()));
}

//---------------------------------------------------------------------------
// StringUtil::starts_with
//---------------------------------------------------------------------------
bool StringUtil::starts_with(const std::string& str, char c)
{
  if( str.size() == 0 )
    return false;

  if( c == str[0] )
    return true;

  return false;
}

//---------------------------------------------------------------------------
// StringUtil::starts_with
//---------------------------------------------------------------------------
bool StringUtil::starts_with(const std::string& str, pcsz pcszStart, bool bNoCase)
{
  if( str.size() < strlen(pcszStart) )
    return false;

  if( bNoCase )
  {
    return (!strnicmp(str.c_str(), pcszStart, strlen(pcszStart)));
  }
  else
  {
    return (!strncmp(str.c_str(), pcszStart, strlen(pcszStart)));
  }
}

//---------------------------------------------------------------------------
// StringUtil::ends_with
//---------------------------------------------------------------------------
bool StringUtil::ends_with(const std::string& str, pcsz pcszEnd, bool bNoCase)
{
  if( str.size() < strlen(pcszEnd) )
    return false;

  if( bNoCase )
  {
    return (!strnicmp(str.c_str() + strlen(str.c_str())-strlen(pcszEnd), pcszEnd, strlen(pcszEnd)));
  }
  else
  {
    return (!strncmp(str.c_str() + strlen(str.c_str())-strlen(pcszEnd), pcszEnd, strlen(pcszEnd)));
  }
}

//---------------------------------------------------------------------------
// StringUtil::ends_with
//---------------------------------------------------------------------------
bool StringUtil::ends_with(const std::string& str, char c)
{
  if( str.size() == 0 )
    return false;

  if( c == str[str.size()-1] )
    return true;

  return false;
}

//---------------------------------------------------------------------------
// Internal utility function
// Look for occurence for a string in another
// Take care of '?' that match any character
//---------------------------------------------------------------------------
static pcsz find_sub_str_with_joker(pcsz pszSrc, pcsz pMask, uint32 uiLenMask, std::vector<std::string>* tokens_p)
{
  if (strlen(pszSrc) < uiLenMask)
    return NULL; // No hope

  // while mask len < string len
  while( *(pszSrc + uiLenMask - 1) )
  {
    uint32 uiOffSrc = 0; // starting offset in mask and sub-string

    // Tant qu'on n'est pas au bout du masque
    while (uiOffSrc < uiLenMask)
    {
      char cMask = pMask[uiOffSrc];

      if (cMask != '?') // In case of '?' it always match
      {
        if (pszSrc[uiOffSrc] != cMask)
          break;
      }
      else if( tokens_p )
      {
        tokens_p->push_back(std::string(1, pszSrc[uiOffSrc]));
      }

      // Next char
      uiOffSrc++;
    }

    // std::string matched !
    if (uiOffSrc == uiLenMask)
      return pszSrc + uiLenMask;

    // Next sub-string
    pszSrc++;
  }

  // Not found
  return NULL;
}

//---------------------------------------------------------------------------
// StringUtil::match
//---------------------------------------------------------------------------
bool StringUtil::match(const std::string& str, const std::string &strMask,
                       std::vector<std::string>* tokens_p)
{
  return match(str, PSZ(strMask), tokens_p);
}

//---------------------------------------------------------------------------
// StringUtil::match
//---------------------------------------------------------------------------
bool StringUtil::match(const std::string& str, pcsz pszMask,
                       std::vector<std::string>* tokens_p)
{
  pcsz pszTxt = str.c_str();
  while (*pszMask)
  {
    switch (*pszMask)
    {

      case '\\':
        // escape next special mask char (e.g. '?' or '*')
        pszMask++;
        if( *pszMask )
        {
          if( *(pszMask++) != *(pszTxt++) )
            return false;
        }
        break;

      case '?': // joker at one position
        if (!*pszTxt)
          return true; // no match

        if( tokens_p )
          tokens_p->push_back( std::string(1, *pszTxt) );

        pszTxt++;
        pszMask++;
        break;

      case '*': // joker on one or more positions
        {
          // Pass through others useless joker characters
          while (*pszMask == '*' || *pszMask == '?')
            pszMask++;

          if( tokens_p )
          {
            // get matching token for joker
            if (!*pszMask )
              tokens_p->push_back(pszTxt);
            else
            {
              std::size_t offset = 1;
              while( pszTxt[offset] != 0 && pszMask[0] != pszTxt[offset] )
                ++offset;
              tokens_p->push_back(std::string(pszTxt, offset));
            }
          }

          if (!*pszMask)
            return true; // Fin

          // end of mask
          uint32 uiLenMask;
          const char *pEndMask = strchr(pszMask, '*');

          if (pEndMask)
            // other jokers characters => look for bloc between the two jokers in source string
            uiLenMask = static_cast<yat::uint32>(pEndMask - pszMask);
          else
            // string must be end with mask
            return (NULL != find_sub_str_with_joker( pszTxt + strlen(pszTxt)-strlen(pszMask), pszMask, strlen(pszMask), tokens_p ) ) ? true : false;

          // Search first uiLenMask characters from mask in text
          pcsz pEnd = find_sub_str_with_joker(pszTxt, pszMask, uiLenMask, tokens_p);

          if (!pEnd)
            // Mask not found
            return false;

          pszTxt = pEnd;
          pszMask += uiLenMask;
        }
        break;

      default:
        if( *(pszMask++) != *(pszTxt++) )
          return false;
        break;
    }
  }

  if( *pszTxt )
    // End of string not reached
    return false;

  return true;
}

//---------------------------------------------------------------------------
// StringUtil::trim
//---------------------------------------------------------------------------
void StringUtil::trim(std::string* str_p)
{
  if( str_p->size() > 0 )
  {
    int iFirstNoWhite = 0;
    int iLastNoWhite = (*str_p).size()-1;

    // Search for first non-white character
    for( ; iFirstNoWhite <= iLastNoWhite; iFirstNoWhite++ )
    {
      if( !isspace((*str_p)[iFirstNoWhite]) )
        break;
    }

    // Search for last non-white character
    for( ; iLastNoWhite > iFirstNoWhite; iLastNoWhite-- )
    {
      if( !isspace((*str_p)[iLastNoWhite]) )
        break;
    }

    // Extract sub-string
    (*str_p) = str_p->substr(iFirstNoWhite, iLastNoWhite - iFirstNoWhite + 1);
  }
}

//---------------------------------------------------------------------------
// StringUtil::trim_right
//---------------------------------------------------------------------------
void StringUtil::trim_right(std::string* str_p)
{
  if( str_p->size() > 0 )
  {
    int iLastNoWhite = str_p->size()-1;

    // Search for last non-white character
    for( ; iLastNoWhite > 0; iLastNoWhite-- )
    {
      if( !isspace((*str_p)[iLastNoWhite]) )
        break;
    }

    // Extract sub-string
    (*str_p) = str_p->substr(0, iLastNoWhite + 1);
  }
}

//---------------------------------------------------------------------------
// StringUtil::trim_left
//---------------------------------------------------------------------------
void StringUtil::trim_left(std::string* str_p)
{
  if( (*str_p).size() > 0 )
  {
    std::size_t iFirstNoWhite = 0;

    // Search for first non-white character
    for( ; iFirstNoWhite <= (*str_p).size(); ++iFirstNoWhite )
    {
      if( !isspace((*str_p)[iFirstNoWhite]) )
        break;
    }

    // Extract sub-string
    (*str_p) = (*str_p).substr(iFirstNoWhite);
  }
}

//---------------------------------------------------------------------------
// StringUtil::trim
//---------------------------------------------------------------------------
void StringUtil::trim( std::vector<std::string>* vec_p )
{
  for( std::size_t i = 0; i < vec_p->size(); ++i )
    trim( & (*vec_p)[i] );
}

//---------------------------------------------------------------------------
// StringUtil::trim_left
//---------------------------------------------------------------------------
void StringUtil::trim_left( std::vector<std::string>* vec_p )
{
  for( std::size_t i = 0; i < vec_p->size(); ++i )
    trim_left( & (*vec_p)[i] );
}

//---------------------------------------------------------------------------
// StringUtil::trim_right
//---------------------------------------------------------------------------
void StringUtil::trim_right( std::vector<std::string>* vec_p )
{
  for( std::size_t i = 0; i < vec_p->size(); ++i )
    trim_right( & (*vec_p)[i] );
}

//---------------------------------------------------------------------------
// StringUtil::trim
//---------------------------------------------------------------------------
void StringUtil::trim( std::vector<yat::String>* vec_p )
{
  for( std::size_t i = 0; i < vec_p->size(); ++i )
    (*vec_p)[i].trim();
}

//---------------------------------------------------------------------------
// StringUtil::trim_left
//---------------------------------------------------------------------------
void StringUtil::trim_left( std::vector<yat::String>* vec_p )
{
  for( std::size_t i = 0; i < vec_p->size(); ++i )
    (*vec_p)[i].trim_left();
}

//---------------------------------------------------------------------------
// StringUtil::trim_right
//---------------------------------------------------------------------------
void StringUtil::trim_right( std::vector<yat::String>* vec_p )
{
  for( std::size_t i = 0; i < vec_p->size(); ++i )
    (*vec_p)[i].trim_right();
}

#define _SPLIT_TO_COLLECTION_IMPL_(s_p, coll_p)                                \
do                                                                             \
{                                                                              \
  if( clear_coll )                                                             \
    coll_p->clear();                                                           \
  std::string strToken;                                                        \
  bool sep_found = false;                                                      \
  while( !(*s_p).empty() )                                                     \
  {                                                                            \
    sep_found = extract_token(s_p, c, &strToken, true) == StringUtil::SEP_FOUND; \
    coll_p->push_back(strToken);                                               \
  }                                                                            \
  if( sep_found )                                                              \
    coll_p->push_back("");                                                     \
} while (0)

//---------------------------------------------------------------------------
// StringUtil::split
//---------------------------------------------------------------------------
void StringUtil::split(std::string* str_p, char c,
                       std::vector<std::string> *vec_p, bool clear_coll)
{
  _SPLIT_TO_COLLECTION_IMPL_(str_p, vec_p);
}

//---------------------------------------------------------------------------
// StringUtil::split
//---------------------------------------------------------------------------
void StringUtil::split(std::string* str_p, char c, std::vector<String> *vec_p,
                       bool clear_coll)
{
  _SPLIT_TO_COLLECTION_IMPL_(str_p, vec_p);
}

//---------------------------------------------------------------------------
// StringUtil::split (const version)
//---------------------------------------------------------------------------
void StringUtil::split(const std::string& str, char c,
                       std::vector<std::string> *vec_p, bool clear_coll)
{
  std::string tmp(str);
  std::string* tmp_p = &tmp;
  _SPLIT_TO_COLLECTION_IMPL_(tmp_p, vec_p);
}

//---------------------------------------------------------------------------
// StringUtil::split (const version)
//---------------------------------------------------------------------------
void StringUtil::split(const std::string& str, char c,
                       std::vector<String> *vec_p, bool clear_coll)
{
  std::string tmp(str);
  std::string* tmp_p = &tmp;
  _SPLIT_TO_COLLECTION_IMPL_(tmp_p, vec_p);
}

//---------------------------------------------------------------------------
// StringUtil::split
//---------------------------------------------------------------------------
void StringUtil::split(std::string* str_p, char c,
                       std::deque<std::string> *deque_p, bool clear_coll)
{
  _SPLIT_TO_COLLECTION_IMPL_(str_p, deque_p);
}

//---------------------------------------------------------------------------
// StringUtil::split
//---------------------------------------------------------------------------
void StringUtil::split(std::string* str_p, char c, std::deque<String> *deque_p,
                       bool clear_coll)
{
  _SPLIT_TO_COLLECTION_IMPL_(str_p, deque_p);
}

//---------------------------------------------------------------------------
// StringUtil::split (const version)
//---------------------------------------------------------------------------
void StringUtil::split(const std::string& str, char c,
                       std::deque<std::string> *deque_p, bool clear_coll)
{
  std::string tmp(str);
  std::string* tmp_p = &tmp;
  _SPLIT_TO_COLLECTION_IMPL_(tmp_p, deque_p);
}

//---------------------------------------------------------------------------
// StringUtil::split (const version)
//---------------------------------------------------------------------------
void StringUtil::split(const std::string& str, char c,
                       std::deque<String> *deque_p, bool clear_coll)
{
  std::string tmp(str);
  std::string* tmp_p = &tmp;
  _SPLIT_TO_COLLECTION_IMPL_(tmp_p, deque_p);
}

//---------------------------------------------------------------------------
// StringUtil::join
//---------------------------------------------------------------------------
void StringUtil::join(std::string* str_p, const std::vector<std::string> &vecStr, char cSep)
{
  (*str_p).erase();
  for( uint32 ui=0; ui < vecStr.size(); ui++ )
  {
    if( 0 < ui )
      (*str_p) += cSep;
    (*str_p) += vecStr[ui];
  }
}

//---------------------------------------------------------------------------
// StringUtil::join
//---------------------------------------------------------------------------
void StringUtil::join(std::string* str_p, const std::vector<String> &vecStr, char cSep)
{
  (*str_p).erase();
  for( uint32 ui=0; ui < vecStr.size(); ui++ )
  {
    if( 0 < ui )
      (*str_p) += cSep;
    (*str_p) += vecStr[ui].str();
  }
}

//---------------------------------------------------------------------------
// StringUtil::join
//---------------------------------------------------------------------------
std::string StringUtil::join(const std::vector<std::string> &vecStr, char cSep)
{
  std::string s;
  join( &s, vecStr, cSep);
  return s;
}

//---------------------------------------------------------------------------
// StringUtil::join
//---------------------------------------------------------------------------
std::string StringUtil::join(const std::vector<String> &vecStr, char cSep)
{
  std::string s;
  join( &s, vecStr, cSep);
  return s;
}

//---------------------------------------------------------------------------
// StringUtil::join
//---------------------------------------------------------------------------
void StringUtil::join(std::string* str_p, const std::deque<std::string> &deque, char cSep)
{
  (*str_p).erase();
  for( uint32 ui=0; ui < deque.size(); ui++ )
  {
    if( 0 < ui )
      (*str_p) += cSep;
    (*str_p) += deque[ui];
  }
}

//---------------------------------------------------------------------------
// StringUtil::join
//---------------------------------------------------------------------------
std::string StringUtil::join(const std::deque<std::string> &deque, char cSep)
{
  std::string s;
  join( &s, deque, cSep);
  return s;
}

//---------------------------------------------------------------------------
// StringUtil::remove_item
//---------------------------------------------------------------------------
bool StringUtil::remove_item(std::string* str_p, const std::string &strItem, char cSep)
{
  std::string::size_type uiPos = (*str_p).find(strItem);
  if( uiPos == std::string::npos )
    return false;

  if( (*str_p) == strItem )
  {
    (*str_p).erase();
    return true;
  }

  std::vector<std::string> vecstr;
  split(str_p, cSep, &vecstr);
  for( std::vector<std::string>::iterator it = vecstr.begin(); it != vecstr.end(); it++ )
  {
    if( *it == strItem )
    {
      vecstr.erase(it);
      join(str_p, vecstr, cSep);
      return true;
    }
  }
  join(str_p, vecstr, cSep);
  return false;
}

//---------------------------------------------------------------------------
// StringUtil::lower
//---------------------------------------------------------------------------
void StringUtil::to_lower(std::string* str_p)
{
  std::transform(str_p->begin(), str_p->end(), str_p->begin(), (int(*)(int))std::tolower);
}

//---------------------------------------------------------------------------
// StringUtil::upper
//---------------------------------------------------------------------------
void StringUtil::to_upper(std::string* str_p)
{
  std::transform(str_p->begin(), str_p->end(), str_p->begin(), (int(*)(int))std::toupper);
}

//---------------------------------------------------------------------------
// StringUtil::replace
//---------------------------------------------------------------------------
void StringUtil::replace(std::string* str_p, pcsz pszSrc, pcsz pszDst)
{
  pcsz pBeg = NULL, pFind = NULL;

  int iLenSrc = strlen(pszSrc);
  if( iLenSrc == 0 )
    // Avoid infinite loop
    return;

  int iLenDst = strlen(pszDst);
  int iOffset = 0;
  for(;;)
  {
    pBeg = (*str_p).c_str() + iOffset;
    pFind = strstr(pBeg, pszSrc);

    if( !pFind )
      return;

    // Do replace
    (*str_p).replace(pFind - pBeg + iOffset, iLenSrc, pszDst);
    // replace again after replacement
    iOffset += static_cast<int>(pFind - pBeg + iLenDst);
  }
}

//---------------------------------------------------------------------------
// StringUtil::replace
//---------------------------------------------------------------------------
void StringUtil::replace(std::string* str_p, char cSrc, char cDst)
{
  if ( !strchr((*str_p).c_str(), cSrc) )
    return; // Nothing to do

  for( uint32 uiPos = 0; uiPos < (*str_p).size(); uiPos++ )
    if( cSrc == (*str_p)[uiPos] )
      (*str_p).replace(uiPos, 1, 1, cDst);
}

//---------------------------------------------------------------------------
// StringUtil::substitute
//---------------------------------------------------------------------------
void StringUtil::substitute(std::string* str_p, pcsz pszCharSet, char cReplacement)
{
  for( std::size_t pos = str_p->find_first_of(pszCharSet);
       pos != std::string::npos;
       pos = str_p->find_first_of(pszCharSet)
     )
  {
    str_p->replace(pos, 1, 1, cReplacement);
  }
}

//---------------------------------------------------------------------------
// StringUtil::remove
//---------------------------------------------------------------------------
void StringUtil::remove(std::string* str_p, pcsz pszCharSet)
{
  for( std::size_t pos = str_p->find_first_of(pszCharSet);
       pos != std::string::npos;
       pos = str_p->find_first_of(pszCharSet)
     )
  {
    str_p->replace(pos, 1, "");
  }
}

//---------------------------------------------------------------------------
// StringUtil::hash
//---------------------------------------------------------------------------
h32_t StringUtil::hash(const std::string& str)
{
  // Very basic implementation !
  int64 modulo = (int64(2) << 31) - 1;
  int64 hash64 = 0;
  uint32 length = str.size();
  for( uint32 i = 0; i < length; i++ )
    hash64 = (31 * hash64 + str[i]) % modulo;

  return (h32_t)(hash64);
}

//---------------------------------------------------------------------------
// StringUtil::hash64
//---------------------------------------------------------------------------
h64_t StringUtil::hash64(const std::string& str)
{
  // Implementation of the FNV-1a algorithm (http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function)
  h64_t hash64 = uint64(14695981039346656037ULL);
  uint32 length = str.size();
  for( uint32 i = 0; i < length; i++ )
    hash64 = (hash64 ^ str[i]) * uint64(1099511628211ULL);

  return hash64;
}

//=============================================================================
// String
//=============================================================================
const String String::nil = "";

//=============================================================================
// Format::Cache
//=============================================================================
Format::Cache& Format::Cache::instance()
{
  static Format::Cache the_unique_instance;
  return the_unique_instance;
}

//---------------------------------------------------------------------------
// Format::Cache::insert
//---------------------------------------------------------------------------
void Format::Cache::insert(h64_t h, CompiledFormatPtr& entry_ptr)
{
  AutoMutex<> __lock(instance().m_mtx);
  std::map<h64_t, Format::CompiledFormatPtr>::iterator it = instance().m_cache.find(h);
  if( it == instance().m_cache.end() )
    instance().m_cache[h] = entry_ptr;
}

//---------------------------------------------------------------------------
// Format::Cache::get
//---------------------------------------------------------------------------
bool Format::Cache::get(const yat::String& fmt, h64_t&  h, CompiledFormatPtr& entry_ptr)
{
  h = fmt.hash64();

  {
    AutoMutex<> __lock(instance().m_mtx);
    std::map<h64_t, Format::CompiledFormatPtr>::iterator it = instance().m_cache.find(h);
    if( it != instance().m_cache.end() )
    {
      entry_ptr = it->second;
      return true;
    }
  }
  entry_ptr = new CompiledFormat;
  return false;
}

//=============================================================================
// Format
//=============================================================================

inline void rep_double_char(std::string& str, char c)
{
  std::size_t p1 = 0, p2 = 0;
  do
  {
    do
    {
      p1 = str.find(c, p2);
      if( std::string::npos == p1 )
        break;
      if( p1 < str.size() - 1 && c == str[p1+1] )
      {
        p2 = std::string::npos;
        break;
      }
      p2 = p1 + 1;
    }
    while( true );

    if( p2 == std::string::npos )
    {
      str.replace(p1, 2, 1, c);
      p2 = p1 + 1;
    }
    else
      break;
  }
  while(true);
}

//---------------------------------------------------------------------------
// Format::parse_field
//---------------------------------------------------------------------------
Format::FieldSection* Format::parse_field(yat::String& field, int* arg_index_p)
{
  FieldSection* section_p = new FieldSection;

  field.trim();

  // string value before substitution is '{fmt}'
  section_p->fc.field.append(1, '{');
  section_p->fc.field.append(field);
  section_p->fc.field.append(1, '}');

  // first look for an element index (2 digits max)
  if( (field.size() > 1 && isdigit(field[0]) && field[1] == ':') ||
      (field.size() > 2 && isdigit(field[0]) && isdigit(field[1]) && field[2] == ':') )
  {
    *arg_index_p = -1;
    *arg_index_p = field[0] - '0';
    if( field[2] == ':' )
    {
      *arg_index_p = 10 * (*arg_index_p) + field[1] - '0';
      field = field.substr(3);
    }
    else // fmt[1] == ':'
      field = field.substr(2);
  }
  field.trim();
  section_p->fc.fmt_spec = field;

  yat::String &fmt = field;
  if( fmt.empty() )
  { // no format specified
    return section_p;
  }
  section_p->fc.empty_fmt = false;

  // look for a format custom part extension
  std::size_t fmt_ext_pos = fmt.find('/');
  if( fmt_ext_pos != std::string::npos )
  {
    // custom format can't start with either '<' or '>'
    if( !(fmt_ext_pos < fmt.size() - 1
        && ( fmt[fmt_ext_pos + 1] == '<' || fmt[fmt_ext_pos + 1] == '>')) )
    {
      if( fmt_ext_pos < fmt.size() - 1 )
        section_p->fc.fmt_ext = fmt.substr(fmt_ext_pos + 1);
      fmt = fmt.substr(0, fmt_ext_pos);
      fmt.trim();
    }
  }

  if( fmt.size() == 1 && isalpha(fmt[0]) )
  {
    // type only
    section_p->fc.type = fmt[0];
    section_p->fc.empty_fmt = false;
    section_p->fc.type_only_fmt = true;
    return section_p;
  }

  bool zero_padding = false;
  std::size_t search_start = 0;
  // search for align characters
  std::size_t align_pos = fmt.find_first_of("<>");
  if( align_pos > 1 && align_pos != std::string::npos )
    throw yat::Exception("BAD_FORMAT", "Invalid format string", "yat::Format::prepare_format");
  if( align_pos != std::string::npos )
  {
    switch( fmt[align_pos] )
    {
      case '<':
        section_p->fc.align_left = true;
        break;
      case '>':
        section_p->fc.align_right = true;
        break;
      case '=':
        section_p->fc.align_sign = true;
        break;
      default:
        break;
    }
    if( align_pos > 0 )
      section_p->fc.fill = fmt[0];
    search_start = align_pos;
  }

  std::size_t sign_pos = fmt.find_first_of("+-", search_start);
  if( sign_pos != std::string::npos )
  {
    if( fmt[sign_pos] == '+' )
      section_p->fc.show_sign = true;
    search_start = sign_pos;
  }
  std::size_t sharp_pos = fmt.find('#', search_start);
  if( sharp_pos != std::string::npos )
  {
    section_p->fc.show_base = true;
    search_start = sharp_pos;
  }

  std::size_t decimal_point_pos = fmt.find('.', search_start);
  std::size_t precision_pos = std::string::npos;
  if( decimal_point_pos != std::string::npos )
    precision_pos = decimal_point_pos + 1;

  std::size_t width_pos = fmt.find_first_of("123456789", search_start);
  if( width_pos == precision_pos )
    width_pos = std::string::npos;

  std::size_t zero_pad_pos = fmt.find('0', search_start);
  if( !section_p->fc.align_right && !section_p->fc.align_left
     && zero_pad_pos != std::string::npos && ( zero_pad_pos == 0
          || (zero_pad_pos > 0 && !std::isdigit(fmt[zero_pad_pos - 1]) &&
                                         '.' != fmt[zero_pad_pos - 1])))
  {
    section_p->fc.zero_padding = true;
    section_p->fc.fill = '0';
  }

  std::size_t type_pos = std::string::npos;
  if( fmt.size() > 0 && (std::isalpha(fmt[fmt.size() - 1]) || fmt[fmt.size() - 1] == '%') )
  {
    type_pos = fmt.size() - 1;
    section_p->fc.type = fmt[type_pos];
  }

  if( decimal_point_pos != std::string::npos && width_pos != std::string::npos )
    section_p->fc.width = StringUtil::to_num<std::size_t>(fmt.substr(width_pos, decimal_point_pos - width_pos));
  else if( width_pos != std::string::npos )
    section_p->fc.width = StringUtil::to_num<std::size_t>(fmt.substr(width_pos, type_pos - width_pos));

  if( precision_pos != std::string::npos )
    section_p->fc.precision = StringUtil::to_num<std::size_t>(fmt.substr(precision_pos, type_pos - precision_pos));

  return section_p;
}

//---------------------------------------------------------------------------
// Format::prepare_type
//---------------------------------------------------------------------------
inline void Format::prepare_type(std::ios_base::fmtflags& ff, std::ostream &oss, const char& type)
{
  switch( type )
  {
    case 'p':
      ff |= std::ios::showbase;
      oss.setf(ff);
      oss << std::hex;
      break;

    case 'b':
      ff |= std::ios::boolalpha;
      oss.setf(ff);
      break;

    case 'f':
    case 'F':
    case '%':
      ff |= std::ios::fixed;
      oss.setf(ff);
      break;

    case 'E':
      ff |= std::ios::uppercase;
    case 'e':
      ff |= std::ios::scientific;
      oss.setf(ff);
      break;

    case 'G':
      ff |= std::ios::uppercase;
      oss.setf(ff);
      break;

    default:
      oss.setf(ff);
      break;
  }
}

//---------------------------------------------------------------------------
// Format::parse
//---------------------------------------------------------------------------
void Format::parse()
{
  h64_t h;
  if( Cache::get(m_str, h, m_compiled_fmt_ptr) )
    return;

  std::size_t start_pos, end_pos, start_str = 0, end_str = std::string::npos;
  std::size_t start_search = 0, arg_auto_index = 0, n_formats = 0;
  yat::String field;
  bool manual_positioning = false, first_field = true;

  while( true )
  {
    start_pos = m_str.find('{', start_search);
    end_pos = m_str.find('}', start_search);

    bool double_end = false, double_start = false;

    if( start_pos != std::string::npos )
    {
      if( start_pos < m_str.size()-1 && m_str[start_pos+1] == '{' )
        // '{{' detected
        double_start = true;
      else
        end_str = start_pos;
    }

    if( end_pos != std::string::npos && end_pos < m_str.size()-1
                                     && m_str[end_pos+1] == '}' )
    { // '}}' detected
      double_end = true;
    }

    if( double_start )
    {
      end_str = start_pos + 1;
      start_search = end_str + 1;
      continue;
    }
    else if( double_end && start_pos > end_pos)
    {
      end_str = end_pos + 1;
      start_search = end_str + 1;
      continue;
    }

    if( end_pos != std::string::npos && start_pos == std::string::npos  )
      throw yat::Exception("BAD_FORMAT", "Single '}' found", "yat::Format::parse");

    else if( end_pos == std::string::npos && start_pos != std::string::npos  )
      throw yat::Exception("BAD_FORMAT", "Single '{' found", "yat::Format::parse");

    else if( end_pos != std::string::npos && start_pos != std::string::npos )
    {
      if( end_pos < start_pos )
        throw yat::Exception("BAD_FORMAT", "Single '}' found", "yat::Format::parse");

      field = m_str.substr(start_pos + 1, end_pos - start_pos - 1);
      if( field.find('{') != std::string::npos )
        throw yat::Exception("BAD_FORMAT", "Unexpected '{' found inside format", "yat::Format::parse");

      // ok, continue
      if( start_str != end_str )
      {
        StringSection* p = new StringSection;
        p->str = m_str.substr(start_str, end_str - start_str);
        rep_double_char(p->str, '{');
        rep_double_char(p->str, '}');
        m_compiled_fmt_ptr->sections.push_back(SectionPtr(p));
      }
      start_str = end_pos + 1;
      end_str = end_pos + 1;
      int arg_index = -1;
      FieldSection* field_p = parse_field(field, &arg_index);
      ++n_formats;
      SectionPtr ptr(field_p);

      // { positioning
      if( !first_field )
      {
        if( manual_positioning && arg_index == -1 )
          throw yat::Exception("BAD_FORMAT", "Can't switch from manual argument positioning to automatic positioning", "yat::Format::parse");
        if( !manual_positioning && arg_index != -1 )
          throw yat::Exception("BAD_FORMAT", "Can't switch from automatic argument positioning to manual positioning", "yat::Format::parse");
      }
      else
        first_field = false;

      if( arg_index != -1 )
      { // manual positioning: {0} {1}...
        m_compiled_fmt_ptr->arg_sections_map[std::size_t(arg_index)].push_back(ptr);
        manual_positioning = true;
      }
      else // automatic positioning: {} {}...
        m_compiled_fmt_ptr->arg_sections_map[arg_auto_index++].push_back(ptr);
      // } positioning

      m_compiled_fmt_ptr->sections.push_back(ptr);
      start_search = end_pos + 1;
    }
    else
    {
      // no more format spec
      if( start_str < m_str.size() )
      {
        StringSection* p = new StringSection;
        p->str = m_str.substr(start_str);
        rep_double_char(p->str, '{');
        rep_double_char(p->str, '}');
        m_compiled_fmt_ptr->sections.push_back(SectionPtr(p));
      }
      break;
    }
  }

  // The map size is logically the expected arguments count
  m_compiled_fmt_ptr->expected_arg_count = m_compiled_fmt_ptr->arg_sections_map.size();
  // check the position indexes. it must start at 0
  for( std::size_t i = 0; i < m_compiled_fmt_ptr->expected_arg_count; ++i )
  {
    if( m_compiled_fmt_ptr->arg_sections_map.find(i) == m_compiled_fmt_ptr->arg_sections_map.end() )
      throw yat::Exception("BAD_FORMAT", "Bad argument numbering. Must start at '0'", "yat::Format::parse");
  }

  // insert into cache
  Cache::insert(h, m_compiled_fmt_ptr);
}

//---------------------------------------------------------------------------
// Format::get_next_format
//---------------------------------------------------------------------------
bool Format::get_next_format(Format::FieldSection** pp_fmt)
{
  if( m_arg_index >= m_compiled_fmt_ptr->expected_arg_count )
    throw yat::Exception("BAD_ARG", "Format argument not expected", "yat::Format::get_next_format");

  if( m_section_by_arg_idx < m_compiled_fmt_ptr->arg_sections_map[m_arg_index].size() )
  {
    *pp_fmt = dynamic_cast<FieldSection*>(m_compiled_fmt_ptr->arg_sections_map[m_arg_index][m_section_by_arg_idx++].get());
    // re-init internal output stream
    m_oss.copyfmt(m_null_fmt);
    m_oss.str("");
    return true;
  }

  *pp_fmt = 0;
  m_section_by_arg_idx = 0;
  ++m_arg_index;
  return false;
}

//---------------------------------------------------------------------------
// Format::prepare_format
//---------------------------------------------------------------------------
void Format::prepare_format(std::ostream &oss,
                            const Format::Context& fc, char default_type,
                            bool force_type)
{
  char type = default_type;

  if( fc.empty_fmt )
  {
    if( 0 != type )
    {
      std::ios_base::fmtflags ff = oss.flags();
      prepare_type(ff, oss, type);
    }
    // no defined spec
    return;
  }

  std::ios_base::fmtflags ff = oss.flags();
  if( 0 != fc.type && !force_type )
    type = fc.type;

  if( fc.type_only_fmt )
  {
    prepare_type(ff, oss, type);
    return;
  }

  if( fc.show_base )   ff |= std::ios::showbase;
  if( fc.show_sign )   ff |= std::ios::showpos;
  if( fc.zero_padding )ff |= std::ios::internal;

  prepare_type(ff, oss, type);

  if( fc.fill )            oss << std::setfill(fc.fill);
  if( fc.width > 0 && !('p' == type && fc.zero_padding) ) oss << std::setw(fc.width);
  if( fc.precision > 0 )   oss << std::setprecision(fc.precision);

  if( 's' == type || 'b' == type )
    oss << std::left;

  if( fc.align_right )     oss << std::right;
  if( fc.align_left )      oss << std::left;
  if( fc.align_sign )      oss << std::left;

  if( fc.zero_padding && 'p' == type )
  {
    oss << std::setfill('0');
    oss << std::setw(2 * YAT_PTR_BYTES_LEN + 2);
  }
}

//---------------------------------------------------------------------------
// Format::get
//---------------------------------------------------------------------------
String& Format::get() const
{
  m_str.clear();

  if( m_compiled_fmt_ptr )
  {
    for( std::size_t i = 0; i < m_compiled_fmt_ptr->sections.size(); ++i )
    {
      if( m_compiled_fmt_ptr->sections[i]->type() == replacement_field )
      {
        FieldSection* section_p = dynamic_cast<FieldSection*>(m_compiled_fmt_ptr->sections[i].get());
        ReplacementMap::const_iterator it = m_replacements.find(PTR2INT(section_p));
        if( it != m_replacements.end() )
          // used format
          m_str.append(it->second);
        else
          // unused format => write replacement field on output string
          m_str.append(section_p->fc.field);
      }
      else // string
      {
        m_str.append(dynamic_cast<StringSection*>(m_compiled_fmt_ptr->sections[i].get())->str);
      }
    }
  }
  return m_str;
}

//---------------------------------------------------------------------------
// Format::output_ptr_value
//---------------------------------------------------------------------------
void Format::output_ptr_value(uintptr p, const Format::Context& fc)
{
  if( p )
  {
    prepare_format(m_oss, fc, 'p');
    m_oss << p;
  }
  else
  {
    prepare_format(m_oss, fc, 's', true);
    if( fc.zero_padding )
      m_oss << std::setw(2 * YAT_PTR_BYTES_LEN + 2);
    m_oss << std::setfill(' ') << "null";
  }
}

//---------------------------------------------------------------------------
// Format::arg(const char *s)
//---------------------------------------------------------------------------
Format& Format::arg(const char *s)
{
  FieldSection *section_p = 0;
  while( get_next_format(&section_p) )
  {
    if( 'p' == section_p->fc.type || !s )
      output_ptr_value(reinterpret_cast<uintptr>(s), section_p->fc);
    else
    {
      prepare_format(m_oss, section_p->fc, 's', true);
      m_oss << s;
    }
    m_replacements[PTR2INT(section_p)] = m_oss.str();
  }
  return *this;
}

//---------------------------------------------------------------------------
// Format::Formatter<std::string>
//---------------------------------------------------------------------------
void Format::Formatter<std::string>::operator()(const std::string& v, std::ostream &oss, const Format::Context& fc)
{
  prepare_format(oss, fc, 's', true);
  oss << v;
}

//---------------------------------------------------------------------------
// Format::Formatter<String>
//---------------------------------------------------------------------------
void Format::Formatter<String>::operator()(const String& v, std::ostream &oss, const Format::Context& fc)
{
  prepare_format(oss, fc, 's', true);
  oss << v;
}

//---------------------------------------------------------------------------
// Format::Formatter<char>
//---------------------------------------------------------------------------
void Format::Formatter<char>::operator()(const char& v, std::ostream &oss, const Format::Context& fc)
{
  prepare_format(oss, fc);
  if( 'B' == fc.type )
    output_binary_value(v, oss, fc);
  else if( 'o' == fc.type )
    output_octal_value(v, oss, fc);
  else if( 'X' == fc.type || 'x' == fc.type )
    output_hex_value(v, oss, fc);
  else if( 'd' == fc.type )
    oss << int(v);
  else
    oss << v;
}

#ifdef YAT_LINUX
//---------------------------------------------------------------------------
// Format::Formatter<int8>
//---------------------------------------------------------------------------
void Format::Formatter<int8>::operator()(const int8& v, std::ostream &oss, const Format::Context& fc)
{
  prepare_format(oss, fc);
  if( 'B' == fc.type )
    output_binary_value(v, oss, fc);
  else if( 'o' == fc.type )
    output_octal_value(v, oss, fc);
  else if( 'X' == fc.type || 'x' == fc.type )
    output_hex_value(v, oss, fc);
  else if( 'd' == fc.type )
    oss << int(v);
  else
    oss << v;
}
#endif

//---------------------------------------------------------------------------
// Format::Formatter<unsigned char>
//---------------------------------------------------------------------------
void Format::Formatter<unsigned char>::operator()(const unsigned char& v, std::ostream &oss, const Format::Context& fc)
{
  prepare_format(oss, fc);
  if( 'B' == fc.type )
    output_binary_value(v, oss, fc);
  else if( 'o' == fc.type )
    output_octal_value(v, oss, fc);
  else if( 'X' == fc.type || 'x' == fc.type )
    output_hex_value(v, oss, fc);
  else if( 'd' == fc.type )
    oss << int(v);
  else
    oss << v;
}

//---------------------------------------------------------------------------
// Format::Formatter<bool>
//---------------------------------------------------------------------------
void Format::Formatter<bool>::operator()(const bool& v, std::ostream &oss, const Format::Context& fc)
{
  prepare_format(oss, fc, 'b');
  oss << v;
}

//---------------------------------------------------------------------------
// Format::Formatter<double>
//---------------------------------------------------------------------------
void Format::Formatter<double>::operator()(const double& v, std::ostream &oss,
                                           const Format::Context& fc)
{
  prepare_format(oss, fc);

  if( '%' == fc.type )
    output_percent_value(v, oss, fc);
  else
    oss << v;
}

//---------------------------------------------------------------------------
// Format::Formatter<float>
//---------------------------------------------------------------------------
void Format::Formatter<float>::operator()(const float& v, std::ostream &oss,
                                          const Format::Context& fc)
{
  prepare_format(oss, fc);

  if( '%' == fc.type )
    output_percent_value(v, oss, fc);
  else
    oss << v;
}

//---------------------------------------------------------------------------
// operators with left & right args
//---------------------------------------------------------------------------
std::string operator+(const String& s1, const String& s2)
{ std::string s; s = s1.str() + s2.str(); return s; }
std::string operator+(const String& s1, const std::string& s2)
{ std::string s; s = s1.str() + s2; return s; }
std::string operator+(const std::string& s1, const String& s2)
{ std::string s; s = s1 + s2.str(); return s; }
std::string operator+(const char* psz, const String& str)
{ std::string s; s = psz + str.str(); return s; }
std::string operator+(char c, const String& str)
{ std::string s; s = c + str.str(); return s; }
std::string operator+(const String& str, const char* psz)
{ std::string s; s = str.str() + psz; return s; }
std::string operator+(const String& str, char c)
{ std::string s; s = str.str() + c; return s; }
bool operator==(const String& s1, const String& s2)
{ return s1.str() == s2.str(); }
bool operator==(const std::string& s1, const String& s2)
{ return s1 == s2.str(); }
bool operator==(const String& s1, const std::string& s2)
{ return s1.str() == s2; }
bool operator!=(const String& s1, const String& s2)
{ return s1.str() != s2.str(); }
bool operator!=(const String& s1, const std::string& s2)
{ return s1.str() != s2; }
bool operator!=(const std::string& s1, const String& s2)
{ return s1 != s2.str(); }
bool operator<(const String& s1, const String& s2)
{ return s1.str() < s2.str(); }
bool operator<(const String& s1, const std::string& s2)
{ return s1.str() < s2; }
bool operator<(const std::string& s1, const String& s2)
{ return s1 < s2.str(); }
bool operator<=(const String& s1, const std::string& s2)
{ return s1.str() <= s2; }
bool operator<=(const String& s1, const String& s2)
{ return s1.str() <= s2.str(); }
bool operator<=(const std::string& s1, const String& s2)
{ return s1 <= s2.str(); }
bool operator>(const String& s1, const String& s2)
{ return s1.str() > s2.str(); }
bool operator>(const String& s1, const std::string& s2)
{ return s1.str() > s2; }
bool operator>(const std::string& s1, const String& s2)
{ return s1 > s2.str(); }
bool operator>=(const String& s1, const std::string& s2)
{ return s1.str() >= s2; }
bool operator>=(const String& s1, const String& s2)
{ return s1.str() >= s2.str(); }
bool operator>=(const std::string& s1, const String& s2)
{ return s1 >= s2.str(); }
bool operator==(const String& s1, const char* s2)
{ return s1.str() == s2; }
bool operator==(const char* s1, const String& s2)
{ return s1 == s2.str(); }
bool operator!=(const String& s1, const char* s2)
{ return s1.str() != s2; }
bool operator!=(const char* s1, const String& s2)
{ return s1 != s2.str(); }
bool operator<(const String& s1, const char* s2)
{ return s1.str() < s2; }
bool operator<(const char* s1, const String& s2)
{ return s1 < s2.str(); }
bool operator<=(const String& s1, const char* s2)
{ return s1.str() <= s2; }
bool operator<=(const char* s1, const String& s2)
{ return s1 <= s2.str(); }
bool operator>(const String& s1, const char* s2)
{ return s1.str() > s2; }
bool operator>(const char* s1, const String& s2)
{ return s1 > s2.str(); }
bool operator>=(const String& s1, const char* s2)
{ return s1.str() >= s2; }
bool operator>=(const char* s1, const String& s2)
{ return s1 >= s2.str(); }

std::ostream& operator<<(std::ostream& os, const String& s)
{ return operator<<(os, s.str()); }
std::istream& operator>>(std::istream& is, const String& s)
{ return operator>>(is, s.str()); }

std::string StringUtil::str_format(pcsz pszFormat, ...)
{
  static char buf[BUF_LEN];
  static Mutex mtx;
  AutoMutex<> lock(mtx);
  va_list argptr;
  va_start(argptr, pszFormat);
  VSNPRINTF(buf, BUF_LEN, pszFormat, argptr);
  va_end(argptr);

  return std::string(buf);
}
yat::String String::str_format(pcsz pszFormat, ...)
{
  static char buf[BUF_LEN];
  static Mutex mtx;
  AutoMutex<> lock(mtx);
  va_list argptr;
  va_start(argptr, pszFormat);
  VSNPRINTF(buf, BUF_LEN, pszFormat, argptr);
  va_end(argptr);

  return yat::String(buf);
}
int String::printf(pcsz pszFormat, ...)
{
  static char buf[BUF_LEN];
  static Mutex mtx;
  AutoMutex<> lock(mtx);
  va_list argptr;
  va_start(argptr, pszFormat);
  VSNPRINTF(buf, BUF_LEN, pszFormat, argptr);
  va_end(argptr);
  m_str = buf;
  return m_str.size();
}
int StringUtil::printf(std::string* str_p, pcsz pszFormat, ...)
{
  static char buf[BUF_LEN];
  static Mutex mtx;
  AutoMutex<> lock(mtx);
  va_list argptr;
  va_start(argptr, pszFormat);
  VSNPRINTF(buf, BUF_LEN, pszFormat, argptr);
  va_end(argptr);

  (*str_p) = buf;
  return (*str_p).size();
}

} // namespace
