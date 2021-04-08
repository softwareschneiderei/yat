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
// Copyright (C) 2006-2021  The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB.
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// Ramon Sune (ALBA) for the Signal class
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
//      Stephane Poirier
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author S.Poirier - Synchrotron SOLEIL
 */

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <yat/system/SysUtils.h>
#include <yat/utils/String.h>
#include <yat/time/Timer.h>

namespace yat
{

//----------------------------------------------------------------------------
// SysUtils::get_env
//----------------------------------------------------------------------------
bool SysUtils::get_env(const std::string &strVar, std::string *pstrValue, const char *pszDef)
{
  char *pszBuf = ::getenv(PSZ(strVar));
  if ( pszBuf == NULL )
  { // if the variable  is undefined, use default value
    if ( pszDef )
      *pstrValue = pszDef;
    return false;
  }

  *pstrValue = pszBuf;
  return true;
}

//----------------------------------------------------------------------------
// SysUtils::get_env
//----------------------------------------------------------------------------
bool SysUtils::get_env(const std::string &strVar, yat::String *pstrValue, const char *pszDef)
{
  char *pszBuf = ::getenv(PSZ(strVar));
  if ( pszBuf == NULL )
  { // if the variable  is undefined, use default value
    if ( pszDef )
      *pstrValue = pszDef;
    return false;
  }

  *pstrValue = pszBuf;
  return true;
}

//----------------------------------------------------------------------------
// SysUtils::exec
//----------------------------------------------------------------------------
bool SysUtils::exec(const char* pszCmdLine, const char *, int bBackground, bool, int *puiReturnCode)
{
  std::string sCmd = pszCmdLine;
  if( bBackground )
    // Background task
    sCmd += "&";

  // Execute shell command then exit
  *puiReturnCode = ::system(PSZ(sCmd));

  return true;
}

//----------------------------------------------------------------------------
// SysUtils::exec_as
//----------------------------------------------------------------------------
bool SysUtils::exec_as(const char* pszCmdLine, const char *, int bBackground, bool bThrow, int *puiReturnCode, uid_t uid, gid_t gid)
{
  int rc = 0;

  std::string sCmd = pszCmdLine;
  if( bBackground )
    // Background task
    sCmd += "&";

  // fork a new process
  pid_t pid = ::fork();
  if( 0 == pid )
  {
    // Child process
    if( gid > 0 )
    {
      // Set new gid
      rc = ::setgid(gid);
      if( rc )
        ::exit(-1);
    }
    if( uid > 0 )
    {
      // Set new uid
      rc = ::setuid(uid);
      if( rc )
        ::exit(-1);
    }
    // Execute shell command then exit
    ::exit(::system(PSZ(sCmd)));
  }
  else if( pid < 0 && bThrow )
    throw Exception("SYSTEM_ERROR", "Error cannot execute shell command", "SysUtils::exec_as");
  else if( pid < 0 )
    return false;

  // Parent process, let's wait for the child to finish
  ::waitpid(pid, puiReturnCode, WEXITSTATUS(*puiReturnCode));
  return true;
}

//----------------------------------------------------------------------------
// SysUtils::is_root
//----------------------------------------------------------------------------
bool SysUtils::is_root()
{
  return (0 == geteuid());
}

//----------------------------------------------------------------------------
// free function: waitpid_eintr
//----------------------------------------------------------------------------
pid_t waitpid_eintr(int *status_p)
{
  pid_t pid = 0;
  while ( (pid = ::waitpid(-1, status_p, 0)) == -1 )
  {
    if (errno == EINTR)
      continue;
    else
    {
      std::ostringstream oss;
      oss << "waitpid failure (Errno " << errno << ": " << strerror(errno);
      throw Exception("SYSTEM_ERROR", oss.str(), "waitpid_eintr");
    }
  }
  return pid;
}

//----------------------------------------------------------------------------
// SysUtils::exec_script
//----------------------------------------------------------------------------
int SysUtils::exec_script(const yat::String& script, const std::vector<yat::String>& args_vec,
                          std::size_t timeout, bool *is_timeout_p, std::size_t* exec_time_ms_p)
{
  *is_timeout_p = false;
  yat::Timer tm_exec;

  if( 0 == timeout )
    timeout = 60000; // 1 minute

  const pid_t timer_pid = ::fork();
  if( timer_pid < 0 )
    throw Exception("SYSTEM_ERROR",
                    "Error cannot execute shell command (failed to fork timer process).",
                    "SysUtils::exec_script");
  if( 0 == timer_pid )
  {
    // Timer process
    if( timeout > 1000 )
      ::sleep(timeout / 1000);
    else
      ::usleep(timeout * 1000);
    exit(0);
  }

  const pid_t script_pid = ::fork();
  if( script_pid < 0 )
    throw Exception("SYSTEM_ERROR",
                    "Error cannot execute shell command (failed to fork command process).",
                    "SysUtils::exec_script");

  if( 0 == script_pid )
  { // Child process executing the script
    std::vector<yat::String> vpath;
    script.split('/', &vpath);

    // Arguments
    char **args = new char*[args_vec.size()+2];
    // First arg is the script file name
    args[0] = (char*)vpath.back().c_str();
    for( std::size_t i = 0; i < args_vec.size(); ++i )
    {
      args[i+1] = (char*)args_vec[i].c_str();
    }
    args[args_vec.size()+1] = NULL;

    ::execv(script.c_str(), args);
    // An error occured
    exit(EXEC_ERROR_STATUS);
  }

  int status = 0;
  const pid_t finished_first = waitpid_eintr(&status);
  if (finished_first == timer_pid)
  {
    // time out, kill the script process
    ::kill(script_pid, SIGKILL);
    ::waitpid(script_pid, NULL, 0);

    *is_timeout_p = true;
    if( exec_time_ms_p )
      *exec_time_ms_p = timeout;
  }
  else if (finished_first == script_pid)
  {
    // Script executed, kill the timer process
    ::kill(timer_pid, SIGKILL);
    ::waitpid(timer_pid, NULL, 0);

    if( exec_time_ms_p )
      *exec_time_ms_p = tm_exec.elapsed_msec();
  }
  else
  {
    throw Exception("SYSTEM_ERROR", "Unknown error while executing script.", "SysUtils::exec_script");
  }
  return WEXITSTATUS(status);
}

} // namespace
