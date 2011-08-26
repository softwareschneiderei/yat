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


#ifndef __YAT_LOGGING_H__
#define __YAT_LOGGING_H__

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>
#include <stack>
#include <set>

namespace yat
{
//=============================================================================
/// Logging messages
///
/// Usage:
/// Just log messages using the log_xxx methods to log into the clog stream
/// If a special log storage/presentation is necessary then the things to do are:
///  - implement the ILogTarget interface
///  - Instantiate a LogCatcher object with a pointer to the ILogTarget object
///
/// TODO : managing syslog messages
//=============================================================================

//=============================================================================
/// ELogLevel
/// Define severity types using syslog levels definitions
//=============================================================================
enum ELogLevel
{
  LOG_RESULT = 0,  /// This is not a information message but a real result that must not be filtered
  LOG_VERBOSE,     /// To known every executed actions at low level
  LOG_INFO,        /// This is the default message level, it report normal information about the system or the application
  LOG_NOTICE,      /// Describe a (perhaps unusual) event that is important to repport
  LOG_WARNING,     /// Report a warning
  LOG_ERROR,       /// A error occured but the system is still functionnal
  LOG_CRITICAL,    /// Critical error.
  LOG_ALERT,       /// Immediate fixing is needed.
  LOG_EMERGENCY,   /// The system will shutdown now because of an irrecoverable failure
  LOG_LEVEL_COUNT  /// Not a log level, just the total number of levels
};

//=============================================================================
/// ILogTarget log target interface 
///
//=============================================================================
class ILogTarget
{
public:
  /// Log message
  virtual void log(ELogLevel eLevel, pcsz pszType, const String &strMsg)=0;
};

/// Log target stack, for logging redirection
typedef std::stack<class ILogTarget *> LogTargetStack;

//=============================================================================
/// Default log handler : print log on console using clog stream
//
//=============================================================================
class DefaultLogHandler: public ILogTarget
{
public:
  /// Log message
  void log(ELogLevel eLevel, pcsz pszType, const String &strMsg);
};

//=============================================================================
/// Logging manager class
//
//=============================================================================
class LogManager
{
friend class LogCatcher;
private:
  static LogManager *ms_pTheInstance;
  static LogManager *Instance();
  DefaultLogHandler  m_defLogHandler;

  // Log target
  LogTargetStack     m_stkCatchLogTarget;

  // Min severity level of logged messages
  int         m_iMinLevel;

  // Logged messages types
  std::set<String>  m_setTypes;

  // Add a new log target to the stack
  static void push_log_target(ILogTarget *pLogTarget);

  // Remove top log target
  static void pop_log_target();

  // Constructor
  LogManager();

public:

  /// Initialize LogHandler.
  ///
  /// @param eMinLevel Min severity level
  /// @param strFilter List (separator = '|') of source types used for message filtering
  ///
  static void init(int iMinLevel, const String &strFilter=String::nil);

  /// Log message
  static void log(ELogLevel eLevel, pcsz pszType, const String &strMsg);

  /// Min log level take in account
  static int min_level() { return Instance()->m_iMinLevel; }

  /// Return current log target
  static ILogTarget *current_log_target();
};

/// Log forwarding function type declaration
typedef void (*pfn_log_fwd)(int iLevel, const char *pszType, const char *pszMsg);

//=============================================================================
/// Helper class for log forwarding 
//
//=============================================================================
class LogForward: public ILogTarget
{
private:
	// Function to forward log to
  pfn_log_fwd m_pfn_log_fwd;

public:
	/// Constructor
  LogForward(pfn_log_fwd pfn_log_fwd);

  /// ILogTarget
  virtual void log(ELogLevel eLevel, pcsz pszType, const String &strMsg);
};

//=============================================================================
/// Class used to define object that catch log, during LogCatcher live time
/// LogCatcher object are stacked:
/// when a LogCatcher object die, than the previous still living will receive messages
/// when the first created LogCatcher is deleted messages are catched by the DefaultLogHandler
//
//=============================================================================
class LogCatcher
{
public:
  /// Constructor : Push log target in ClogHandler stack
  LogCatcher(ILogTarget *pLogTarget);

  /// Destructor : remove top log target from the stack
  ~LogCatcher();
};

//=============================================================================
/// Log functions
//
//=============================================================================

void log_result(pcsz pszType, pcsz pszFormat, ...);
void log_verbose(pcsz pszType, pcsz pszFormat, ...);
void log_info(pcsz pszType, pcsz pszFormat, ...);
void log_notice(pcsz pszType, pcsz pszFormat, ...);
void log_warning(pcsz pszType, pcsz pszFormat, ...);
void log_error(pcsz pszType, pcsz pszFormat, ...);
void log_critical(pcsz pszType, pcsz pszFormat, ...);
void log_alert(pcsz pszType, pcsz pszFormat, ...);
void log_emergency(pcsz pszType, pcsz pszFormat, ...);

#define LOG_EXCEPTION(domain, e) \
  do \
  { \
    for (size_t i = 0; i < e.errors.size(); i++) \
      yat::log_error(domain, "(%d). %s. %s. From %s.", PSZ(e.errors[i].reason), PSZ(e.errors[i].desc), PSZ(e.errors[i].origin)); \
  } while(0)

} // namespace

#endif