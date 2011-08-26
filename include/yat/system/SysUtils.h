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


#ifndef __YAT_SYS_UTILS_H__
#define __YAT_SYS_UTILS_H__

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>

namespace yat
{
//===========================================================================
//
/// Generic system utilities
//
//===========================================================================
class YAT_DECL SysUtils
{
public:
  /// Return the value of a environnement variable
  ///
  /// @param pstrVar Variable to evaluate
  /// @return true if template has been evaluated, or false
  ///
  static bool get_env(const String &strVar, String *pstrValue, const char *pszDef=NULL);

  /// Execute command line
  /// 
  bool exec(const char* pszCmdLine, const char *pszDefDir = NULL,
           int bBackground = true, bool bThrow = true, int* pulReturnCode = NULL);
           
  /// Execute a command line using the identity of another account
  bool exec_as(const char* pszCmdLine, const char *pszDefDir = NULL,
           int bBackground = true, bool bThrow = true, int* pulReturnCode = NULL, uid_t uid = -1, gid_t gid = -1);
};

} // namespace

#endif