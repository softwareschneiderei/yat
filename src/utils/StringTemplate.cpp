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
void StringTemplate::add_symbol_interpreter(ISymbolInterpreter* interpreter_p)
{
  m_interpreters.push_back(interpreter_p);
}

//----------------------------------------------------------------------------
// StringTemplate::remove_symbol_interpreter
//----------------------------------------------------------------------------
void StringTemplate::remove_symbol_interpreter(ISymbolInterpreter* interpreter_p)
{
  std::list<ISymbolInterpreter *>::iterator itInterpreter = m_interpreters.begin();

  while( m_interpreters.end() != itInterpreter )
  {
    if( *itInterpreter == interpreter_p )
    {
      m_interpreters.erase(itInterpreter);
      break;
    }
    itInterpreter++;
  }
}

//----------------------------------------------------------------------------
// StringTemplate::substitute
//----------------------------------------------------------------------------
bool StringTemplate::substitute(std::string* template_p)
{
  return substitute_impl(template_p, NULL, true);
}

//----------------------------------------------------------------------------
// StringTemplate::substitute_ex
//----------------------------------------------------------------------------
bool StringTemplate::substitute_ex(std::string* template_p, std::vector<std::string> *not_found_p)
{
  return substitute_impl(template_p, not_found_p, false);
}

//----------------------------------------------------------------------------
// StringTemplate::substitute_impl
//----------------------------------------------------------------------------
bool StringTemplate::substitute_impl(std::string* template_p, std::vector<std::string>* not_found_p, bool old_impl)
{
  std::string result, var, pattern, template_in = *template_p;
  bool return_value = false;

  if( old_impl )
    //- in the old implementation return false if one variable failed to be substitued
    return_value = true;

  while( template_in.size() > 0 )
  {
    //- search for next variable marker
    std::string::size_type marker_pos = template_in.find('$');
    std::string::size_type end_mark, end_pos = std::string::npos;
    std::string::size_type var_pos = std::string::npos;

    if( marker_pos < template_in.size()-1 && template_in[marker_pos+1] == '(' )
    {
      //- found pattern '$(var)'
      var_pos = marker_pos + 2;
      end_pos = template_in.find(')', var_pos);
      if( std::string::npos == end_pos )
      { //- ')' is missing
        if( !old_impl )
          throw yat::Exception("ERROR", "Missing end parenthesis in string template", "yat::StringTemplate::substitution_impl");
        else
        {
          result += template_in;
          break;
        }
      }
      end_mark = end_pos + 1;
    }
    else if( marker_pos < template_in.size()-1 && template_in[marker_pos+1] == '$' )
    {
      //- escaped '$' => complete result string with characters before 1st '$'
      result += template_in.substr(0, marker_pos + 1);
      //- and remove characters from the inpout template up to the 2nd '$'
      template_in.erase(0, marker_pos + 2);
      continue;
    }
    else if( std::string::npos != marker_pos )
    {
      // found pattern '$var'
      var_pos = marker_pos + 1;
      end_pos = template_in.find_first_not_of("azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN0123456789_", var_pos);
      if( std::string::npos == end_pos )
        end_pos = template_in.size();
      end_mark = end_pos;
    }

    if( std::string::npos != marker_pos )
    {
      //- complete result string
      result += template_in.substr(0, marker_pos);

      //- memorize pattern
      pattern = template_in.substr(marker_pos, end_mark - marker_pos);

      //- delete from input template up to '$' character
      template_in.erase(0, var_pos);

      //- extract variable name or content
      var = template_in.substr(0, end_pos - var_pos);

      //- delete up to the end of variable pattern
      template_in.erase(0, end_pos - marker_pos - 1);

      //- special modifiers, for pattern like '$(uc:var_name)'
      bool to_lower = false, to_upper = false;
      if( yat::StringUtil::starts_with(var, "uc:") )
      {
        to_upper = true;
        var = var.substr(3);
      }
      if( yat::StringUtil::starts_with(var, "lc:") )
      {
        to_lower = true;
        var = var.substr(3);
      }

      //- memorize variable name
      std::string var_before = var;

      //- variable evaluation
      bool rc = value_impl(&var, pattern);
      if( old_impl )
      {
        if( return_value )
          //- if a variable is not found then return false
          return_value = rc;
      }
      else if( rc )
      {
        //- at least one variable is substitued
        return_value = true;
      }

      if( !rc && not_found_p )
      {
        //- substitution has failed
        not_found_p->push_back(var_before);
      }
      else if( rc )
      { //- apply modifiers
        if( to_upper )
          yat::StringUtil::to_upper(&var);
        else if( to_lower )
          yat::StringUtil::to_lower(&var);
      }

      result += var;
    }
    else
    {
      //- copying up to the end of template_in string
      result += template_in;
      template_in.erase();
    }
  }

  //- substitute input pattern with final result
  (*template_p) = result;
  return return_value;
}

//----------------------------------------------------------------------------
// StringTemplate::substitute
//----------------------------------------------------------------------------
bool StringTemplate::substitute(String* template_p)
{
  return substitute( (std::string*)template_p );
}

//----------------------------------------------------------------------------
// StringTemplate::value_impl
//----------------------------------------------------------------------------
bool StringTemplate::value_impl(std::string* var_p, const std::string& pattern)
{
  // Usually the default value in a variables list like:
  // $(ga|bu|zo|'meu')
  // -> return 'meu' if none of ga, bu or zo replacement variable are set
  if( yat::StringUtil::start_with(*var_p, "'") &&
      yat::StringUtil::end_with(*var_p, "'") )
  {
    if( var_p->size() > 2 )
      *var_p = var_p->substr(1, var_p->size() - 2);
    else
      var_p->clear();
    return true;
  }

  std::string alt;
  std::size_t alt_pos = var_p->find('|');
  if( alt_pos != std::string::npos )
  {
    alt = var_p->substr(alt_pos + 1);
    *var_p = var_p->substr(0, alt_pos);
  }
  std::list<ISymbolInterpreter *>::iterator itInterpreter = m_interpreters.begin();

  while( m_interpreters.end() != itInterpreter )
  {
    if( (*itInterpreter)->value(var_p) )
        return true;
    itInterpreter++;
  }

  if( !alt.empty() )
  {
    *var_p = alt;
    //- an alternative is available
    return value_impl(var_p, pattern);
  }

  switch( m_not_found_replacement )
  {
    case EMPTY_STRING:
      *var_p = "";
      break;
    case SYMBOL_NAME:
      break;
    case UNCHANGE_STRING:
      *var_p = pattern;
      break;
  }
  return false;
}

//----------------------------------------------------------------------------
// StringTemplate::value
//----------------------------------------------------------------------------
bool StringTemplate::value(std::string* var_p)
{
  return value_impl(var_p, std::string(*var_p));
}

//----------------------------------------------------------------------------
// StringTemplate::value
//----------------------------------------------------------------------------
bool StringTemplate::value(String* var_p)
{
  return value_impl((std::string*)var_p, std::string(*var_p));
}

//=============================================================================
// EnvVariableInterpreter
//=============================================================================
//----------------------------------------------------------------------------
// EnvVariableInterpreter::value
//----------------------------------------------------------------------------
bool EnvVariableInterpreter::value(std::string* var_p)
{
  return SysUtils::get_env(*var_p, var_p);
}

} // namespace
