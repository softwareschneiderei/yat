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
// Copyright (C) 2006-2016 The Tango Community
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
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat/utils/StringTemplate.h>
#include <yat/system/SysUtils.h>
#include <iostream>

namespace yat
{

//=============================================================================
// StringTemplate
//=============================================================================
//----------------------------------------------------------------------------
// StringTemplate::add_symbol_interpreter
//----------------------------------------------------------------------------
void StringTemplate::add_symbol_interpreter(ISymbolInterpreter *pInterpreter)
{
  m_lstInterpreter.push_back(pInterpreter);
}

//----------------------------------------------------------------------------
// StringTemplate::remove_symbol_interpreter
//----------------------------------------------------------------------------
void StringTemplate::remove_symbol_interpreter(ISymbolInterpreter *pInterpreter)
{
  std::list<ISymbolInterpreter *>::iterator itInterpreter = m_lstInterpreter.begin();

  while( m_lstInterpreter.end() != itInterpreter )
  {
    if( *itInterpreter == pInterpreter )
    {
      m_lstInterpreter.erase(itInterpreter);
      break;
    }
    itInterpreter++;
  }
}

//----------------------------------------------------------------------------
// StringTemplate::substitute
//----------------------------------------------------------------------------
bool StringTemplate::substitute(std::string *pstrTemplate, std::vector<std::string> *not_found_p)
{
  std::string strEval, strTmp;
  std::string strTmpl = *pstrTemplate;
  std::string strVar, strValue;
  bool return_value = true;

  while( strTmpl.size() > 0 )
  {
    // Search for a variable
    std::string::size_type uiFirstPos = strTmpl.find("$(");
    if( std::string::npos != uiFirstPos )
    {
      // Search for matching ')'. Take care of nested variables
      std::string::size_type uiMatchPos = strTmpl.find_first_of(')', uiFirstPos + 2);

      if( std::string::npos != uiMatchPos )
      {
        // complete result string
        strEval += strTmpl.substr(0, uiFirstPos);

        // Delete up to '$(' characters
        strTmpl.erase(0, uiFirstPos + 2);

        // Extract variable content
        strVar = strTmpl.substr(0, uiMatchPos - uiFirstPos - 2);
        // Delete up to matching end parenthesis
        strTmpl.erase(0, uiMatchPos - uiFirstPos - 1);

        std::string var_before = strVar;
        // Variable evaluation
        bool rc = value(&strVar);
        if( return_value )
          return_value = rc;

        if( !rc && not_found_p )
          not_found_p->push_back(var_before);
        strEval += strVar;
      }
      else
      {
        // Missing close bracket
        // Copying up to the end of template string
        strEval += strTmpl;
        strTmpl.erase();
      }
    }
    else
    {
      // Copying up to the end of template string
      strEval += strTmpl;
      strTmpl.erase();
    }
  }

  (*pstrTemplate) = strEval;
  return return_value;
}

//----------------------------------------------------------------------------
// StringTemplate::substitute
//----------------------------------------------------------------------------
bool StringTemplate::substitute(String *pstrTemplate)
{
  return substitute( (std::string*)pstrTemplate );
}

//----------------------------------------------------------------------------
// StringTemplate::value
//----------------------------------------------------------------------------
bool StringTemplate::value(std::string *pstrVar)
{
  std::list<ISymbolInterpreter *>::iterator itInterpreter = m_lstInterpreter.begin();

  while( m_lstInterpreter.end() != itInterpreter )
  {
    if( (*itInterpreter)->value(pstrVar) )
        return true;
    itInterpreter++;
  }

  switch( m_eNotFoundReplacement )
  {
    case EMPTY_STRING:
      *pstrVar = "";
      break;
    case SYMBOL_NAME:
      break;
    case UNCHANGE_STRING:
      *pstrVar = std::string("$(") + *pstrVar + ')';
      break;
  }
  return false;
}

//----------------------------------------------------------------------------
// StringTemplate::value
//----------------------------------------------------------------------------
bool StringTemplate::value(String *pstrVar)
{
  return value( (std::string*)pstrVar );
}

//=============================================================================
// EnvVariableInterpreter
//=============================================================================
//----------------------------------------------------------------------------
// EnvVariableInterpreter::value
//----------------------------------------------------------------------------
bool EnvVariableInterpreter::value(std::string *pstrVar)
{
  return SysUtils::get_env(*pstrVar, pstrVar);
}

} // namespace