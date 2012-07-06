//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2012  The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// See AUTHORS file 
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

#ifndef _YAT_COMMANDLINE_H_
#define _YAT_COMMANDLINE_H_

#include <map>
#include <iostream>
#include <sstream>

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>
#include <yat/Exception.h>

namespace yat 
{

//===========================================================================
//! A simple command line parsor
//! command line is like: "$my_app -a -b value_of_b -c value_of_c blabla"
//===========================================================================
class YAT_DECL CommandLine
{
public:
  //=============================================================================
  //! Command line option
  //=============================================================================
  struct CommandLineOpt
  {
    char   cShortName;
    String strLongName;
    bool   bMandatory;
    String strDesc;
    String strValue;
  };
  typedef std::vector<CommandLineOpt> vecOpts;

  //=============================================================================
  //! Command line argument
  //=============================================================================
  struct CommandLineArg
  {
    String strDesc;
    bool   bMandatory;
    bool   bSingle;
  };

  typedef std::vector<CommandLineArg> vecArgs;

  //! Set command name (displayed in ShowUsage() )
  //!
  void set_cmd_name_version(const String &strName, const String &strVersion);

  //! Add a option
  void add_opt(char cShortName, pcsz pszLongName, pcsz pszValue, pcsz pszDesc);

  //! Add a argument
  void add_arg(pcsz pszDesc, bool bSingle=true, bool bMandatory=true);

  //! Initializing with command line arguments
  //!
  //! \param iArgc, ppszArgv arguments
  //! \return true if aguments are sufficients to launch application
  //! \throw an exception if arguments are not valid
  //!
  bool read(int iArgc, char **ppszArgv) throw (Exception);

  //! Showing Usage
  //!
  //! \param strAppInfo Additionnal information about the application
  //!
  void show_usage(const String &strAppInfo) const;

  //! Retreiving options, arguments and theirs values
  bool is_option(const String &strArg) const;
  String option_value(const String &strOpt) const;
  int arg_count() const;
  String arg(int i) const;

private:

  typedef std::map<String, String> KeyValueMap;
  
  //- Current options values
  KeyValueMap m_dictOptValues; 
  
  //- Simples arguments (not options)
  std::vector<String> m_vecArgs;

  //- Valid options list
  vecOpts     m_vecOptDefs;

  //- Valid arguments list
  vecArgs     m_vecArgDefs;

  //- Argument dictionnary
  KeyValueMap m_dictOpts;

  //- Command name
  String      m_strCmdName;

  //- Version
  String      m_strCmdVersion;

  //- Find option def from short form
  CommandLineOpt *find_short_opt(const char cOpt);

  //- Find option definition from long form
  CommandLineOpt *find_long_opt(const String &strOpt);

  //- Display "bad option" error message
  //-
  //- return always false
  void bad_option(const String &strOpt) throw (Exception);

  //- Show usage
  //-
  //- return false
  bool show_usage(std::ostream &os = std::cout) const;
};

} // yat

#endif
