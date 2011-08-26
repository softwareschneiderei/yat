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


#ifndef __YAT_STRING_TEMPLATE_H__
#define __YAT_STRING_TEMPLATE_H__

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>
#include <list>
#include <set>

namespace yat
{
//===========================================================================
/// Interface of objects able to return template symbols values
///
/// This class is a interface so it can't be directly instancied
/// @see TemplateString
//===========================================================================
class YAT_DECL ISymbolInterpreter
{
protected:
  /// Constructor
  ISymbolInterpreter() { }

public:
  /// Attempts to evaluate a variable 
  ///
  /// @param pstrVar Variable to evaluate
  /// @return true if template has been evaluated, or false
  ///
  virtual bool value(String *pstrSymbol)=0;
};

//===========================================================================
/// StringTemplate processor
///
/// A StringTemplate is a string that contains items to be replace with
/// there real value
/// example : in the string 'date is $(date)', '$(date)' will be replaced
/// with the current date when processed
//===========================================================================
class YAT_DECL StringTemplate
{
public:
  enum NotFoundReplacement
  {
    EMPTY_STRING,
    SYMBOL_NAME,
    UNCHANGE_STRING
  };

private:
  std::list<ISymbolInterpreter *> m_lstInterpreter;
  NotFoundReplacement       m_eNotFoundReplacement;
  
  bool PrivProcess(String *pstrTemplate, bool bRecurse, std::set<String> &setEvaluatedSymbols);
  bool PrivProcessVar(String *pstrVar, bool bRecurse, bool bDeepEvaluation, std::set<String> &setEvaluatedSymbols);

public:
  ///
  /// Constructor
  ///
  StringTemplate(NotFoundReplacement eNotFoundReplacement = SYMBOL_NAME) : m_eNotFoundReplacement(eNotFoundReplacement) {}

  /// Add evaluator
  ///
  /// @param pEvaluator Evaluator object
  ///
  void add_symbol_interpreter(ISymbolInterpreter *pInterpreter);

  /// Remove evaluator
  ///
  /// @param pEvaluator Evaluator object
  ///
  void remove_symbol_interpreter(ISymbolInterpreter *pInterpreter);

  /// Replace a symbol by its value
  ///
  /// @param pstrVar Variable to evaluate, will contains the result
  /// @return true if evaluation is done, or false
  ///
  bool value(String *pstrSymbol);

  /// Process a template string
  ///
  /// @param strTemplate String to evaluate
  /// @param pstrEvaluated Resulting string
  /// @return true if evaluation is done, or false
  ///
  bool substitute(String *pstrTemplate);
};

//===========================================================================
/// A template symbol interpreter for environnement variables
///
//===========================================================================
class YAT_DECL EnvVariableInterpreter : public ISymbolInterpreter
{
public:
  /// @param pstrVar Variable to evaluate
  /// @return true if evaluation is done, or false
  virtual bool value(String *pstrVar);
};

} // namespace

#endif