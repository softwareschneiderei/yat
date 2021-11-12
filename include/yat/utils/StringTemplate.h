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

#ifndef __YAT_STRING_TEMPLATE_H__
#define __YAT_STRING_TEMPLATE_H__

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>
#include <list>
#include <set>

namespace yat
{
// ============================================================================
//! \class ISymbolInterpreter
//! \brief Interface class providing template symbols interpreter.
//!
//! This abstract class can not be used as this and must be derived.
//! The goal of the value() function is to replace a symbol by its real value.
//! See yat::StringTemplate class.
// ============================================================================
class YAT_DECL ISymbolInterpreter
{
protected:
  //! \brief Constructor.
  ISymbolInterpreter() { }

public:
  //! \brief Attempts to evaluate a variable.
  //!
  //! Returns true if template has been evaluated, false otherwise.
  //! \param[in,out] pstrSymbol Variable to evaluate, will contain the result.
  virtual bool value(std::string *pstrSymbol)=0;
};

// ============================================================================
//! \class StringTemplate
//! \brief %String template processor.
//!
//! A StringTemplate object is a string that contains items which will be replaced
//! by their real value. The substitution function looks for "$(xxx)" and
//! "$xxx" patterns.
//! For instance: in the string 'date is \$date', '\$date' will be replaced
//! by the current date when processed. \n
//! The substitution will be realized by each symbol interpreter added in the
//! template processor.
//! The escape sequence is "$$":
//! Processing the input template "$$ga bu" will return "$ga bu".
//! One can use more advanced substitutions:
//! \$\(var|def_var\) will remplace 'var' by its value if 'var' is defined. If not,
//! it will replace by the value of 'def_var' value.
//! \$\(var|'def_value'\) If 'var' is not defined it will remplaced by 'def_value'.
//! \remark .
//! \$\(uc:var\) will remplace 'var' by its value converted in uppercase
//! \$\(lc:var\) will remplace 'var' by its value converted in lowercase
// ============================================================================
class YAT_DECL StringTemplate
{
public:
  //! \brief Substitution behavior in case the string is not found.
  enum NotFoundReplacement
  {
    //! Replace by an empty string.
    EMPTY_STRING,
    //! Keep symbol name.
    SYMBOL_NAME,
    //! Keep template.
    UNCHANGE_STRING
  };

private:
  std::list<ISymbolInterpreter *> m_interpreters;
  NotFoundReplacement             m_not_found_replacement;

  bool PrivProcess(std::string* template_p, bool recurse, std::set<std::string>& evaluated_variables);
  bool PrivProcessVar(std::string* var_p, bool recurse, bool deep_evaluation, std::set<std::string>& evaluated_variables);
  bool substitute_impl(std::string* template_p, std::vector<std::string>* not_found_p, bool old_impl);
  bool value_impl(std::string* variable_p, const std::string& pattern);


public:
  //! \brief Constructor.
  //! \param eNotFoundReplacement %Template not found strategy.
  StringTemplate(NotFoundReplacement not_found_replacement = SYMBOL_NAME) : m_not_found_replacement(not_found_replacement) {}

  //! \brief Adds a symbol interpreter.
  //! \param pInterpreter Symbol interpreter.
  void add_symbol_interpreter(ISymbolInterpreter* interpreter_p);

  //! \brief Removes a symbol interpreter.
  //! \param pInterpreter Symbol interpreter.
  void remove_symbol_interpreter(ISymbolInterpreter* interpreter_p);

  //! \brief Replaces a variable by its real value.
  //!
  //! Returns true if evaluation is done, false otherwise.
  //! \param[in,out] pstrSymbol Symbol to evaluate, will contain the result.
  bool value(std::string* variable_p);

  //! \brief Processes a template string.
  //!
  //! Evaluates the whole string. Every single template substitution is
  //! done by symbol interpreters.
  //! Returns true if evaluation is fully done, false otherwise.
  //! \param[in,out] template_p %String to evaluate, will contain the result.
  bool substitute(std::string* template_p);

  //! \brief Processes a template string. New implementation.
  //!
  //! Evaluates the whole string. Every single template substitution is
  //! done by symbol interpreters.
  //! Returns true if at least one variable to evaluate was found, false otherwise.
  //! \param[in,out] template_p %String to evaluate, will contain the result.
  //! \param[out] not_found_p list (not cleared) of variables that can't be evaluated
  bool substitute_ex(std::string* template_p, std::vector<std::string>* not_found_p=NULL);

  //! \deprecated
  bool value(String* pstrSymbol);

  //! \deprecated
  bool substitute(String* template_p);
};

// ============================================================================
//! \class EnvVariableInterpreter
//! \brief A template symbol interpreter for environment variables.
//!
//! This class provides an evaluation function for system environment variables.
//! Inherits from ISymbolInterpreter class.
// ============================================================================
class YAT_DECL EnvVariableInterpreter : public ISymbolInterpreter
{
public:
  //! \brief Attempts to evaluate a variable.
  //!
  //! Returns true if template has been evaluated, false otherwise.
  //! \param[in,out] pstrVar Variable to evaluate, will contain the result.
  virtual bool value(std::string* pstrVar);
};

} // namespace

#endif