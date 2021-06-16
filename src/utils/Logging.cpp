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
#include <yat/utils/Logging.h>
#include <yat/time/Time.h>
#include <yat/threading/Mutex.h>
#include <yat/threading/Utilities.h>
#include <yat/threading/SyncAccess.h>
#include <iostream>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

namespace yat
{

//=============================================================================
// LogStream
//=============================================================================
LogStream::LogStream(ELogLevel level): std::ostream(this), m_level(level),
                                       m_locked_thread(0), m_locked(false)
{
  m_mtx_msg_uptr.reset(new yat::Mutex);
  m_mtx_locker_uptr.reset(new yat::Mutex);
  m_the_message.reserve(512); // A large enough initial capacity for stream logs
}

//=============================================================================
// LogStream
//=============================================================================
void LogStream::lock()
{
  // As we use recursive mutexes, the lock count for a given thread can't exceed
  // unlock count. m_mtx_locker_uptr is used for manage the lock state
  m_mtx_locker_uptr->lock();
  if( !m_locked || (m_locked && m_locked_thread != ThreadingUtilities::self()) )
  {
    m_mtx_locker_uptr->unlock();

    // Wait for access to the message string
    m_mtx_msg_uptr->lock();

    m_mtx_locker_uptr->lock();
    m_locked = true;
    m_locked_thread = ThreadingUtilities::self();
    m_mtx_locker_uptr->unlock();
  }
  else
    m_mtx_locker_uptr->unlock();
}

int LogStream::sync()
{
  if( m_the_message.size() > 0 )
    // Erase leading '\n'
    m_the_message.erase(m_the_message.size() - 1, 1);

  m_log_target_p->log(m_level, "n/a", m_the_message);
  m_the_message.clear();

  // Release the message mutex
  m_mtx_msg_uptr->unlock();

  m_mtx_locker_uptr->lock();
  m_locked = false;
  m_locked_thread = 0;
  m_mtx_locker_uptr->unlock();

  return 0;
}

int LogStream::overflow(int c)
{

  //if( c != '\n' )
  {
    m_the_message.append(1, char(c));
    return c;
  }
  return 0;
}

std::streamsize LogStream::xsputn(char const* p, std::streamsize n)
{
  m_the_message.append(p, n);
  return n;
}


//=============================================================================
// LogManager
//=============================================================================
LogManager *LogManager::ms_pTheInstance = NULL;

//----------------------------------------------------------------------------
// LogManager::LogManager
//----------------------------------------------------------------------------
LogManager::LogManager() :
  m_verbose(LOG_VERBOSE), m_result(LOG_RESULT), m_info(LOG_INFO),
  m_warning(LOG_WARNING), m_error(LOG_ERROR), m_notice(LOG_NOTICE),
  m_critical(LOG_CRITICAL), m_alert(LOG_ALERT), m_emergency(LOG_EMERGENCY),
  m_iMinLevel(LOG_INFO)
{
  update_stream_log_target(&m_defLogHandler);
}

//----------------------------------------------------------------------------
// LogManager::Instance
//----------------------------------------------------------------------------
LogManager *LogManager::Instance()
{
  if( NULL == ms_pTheInstance )
    ms_pTheInstance = new LogManager;

  return ms_pTheInstance;
}

//----------------------------------------------------------------------------
// LogManager::update_stream_log_target
//----------------------------------------------------------------------------
void LogManager::update_stream_log_target(ILogTarget *pLogTarget)
{
  m_result.m_log_target_p = pLogTarget;
  m_verbose.m_log_target_p = pLogTarget;
  m_info.m_log_target_p = pLogTarget;
  m_notice.m_log_target_p = pLogTarget;
  m_warning.m_log_target_p = pLogTarget;
  m_error.m_log_target_p = pLogTarget;
  m_critical.m_log_target_p = pLogTarget;
  m_alert.m_log_target_p = pLogTarget;
  m_emergency.m_log_target_p = pLogTarget;
}

//----------------------------------------------------------------------------
// LogManager::push_log_target
//----------------------------------------------------------------------------
void LogManager::push_log_target(ILogTarget *pLogTarget)
{
  Instance()->m_stkCatchLogTarget.push(pLogTarget);
  Instance()->update_stream_log_target(pLogTarget);
}

//----------------------------------------------------------------------------
// LogManager::pop_log_target
//----------------------------------------------------------------------------
void LogManager::pop_log_target()
{
  // Get a reference to the singleton object
  LogManager &o = *Instance();

  if( !o.m_stkCatchLogTarget.empty() )
    o.m_stkCatchLogTarget.pop();

  Instance()->update_stream_log_target(current_log_target());
}

//----------------------------------------------------------------------------
// LogManager::current_log_target
//----------------------------------------------------------------------------
ILogTarget *LogManager::current_log_target()
{
  // Get a reference to the singleton object
  LogManager &o = *Instance();

  if( !o.m_stkCatchLogTarget.empty() )
    return o.m_stkCatchLogTarget.top();

  return &o.m_defLogHandler;
}

//----------------------------------------------------------------------------
// LogManager::init
//----------------------------------------------------------------------------
void LogManager::init(int iMinLevel, const std::string &_strFilter)
{
  // Get a reference to the singleton object
  LogManager &o = *Instance();

  o.m_iMinLevel = iMinLevel;

  std::string strFilter = _strFilter, strType;
  while( strFilter.size() > 0 )
  {
    yat::StringUtil::extract_token(&strFilter, '|', &strType);
    if( strType.size() > 0 )
      o.m_setTypes.insert(strType);
  }
}

//----------------------------------------------------------------------------
// LogManager::log
//----------------------------------------------------------------------------
void LogManager::log(ELogLevel eLevel, pcsz pszType, const std::string& strMsg)
{
  // Get a reference to the singleton
  LogManager &o = *Instance();

  // Minimum level message to display (except for LOG_RESULT type messages)
  if( int(eLevel) < o.m_iMinLevel && eLevel != LOG_RESULT )
    // Do nothing
    return;

  // verbose messages filtering
  if( o.m_setTypes.size() > 0 &&
      o.m_setTypes.find(std::string(pszType)) == o.m_setTypes.end() && LOG_VERBOSE == eLevel )
    // Type not found in filter set
    return;

  // Notify log tarteg
  if( current_log_target() )
    current_log_target()->log(eLevel, pszType, strMsg);
}

//----------------------------------------------------------------------------
// LogManager::verbose_stream
//----------------------------------------------------------------------------
std::ostream& LogManager::verbose_stream()
{
  LogManager &o = *Instance();

  if( o.m_iMinLevel > LOG_VERBOSE )
    return o.m_null;

  o.m_verbose.lock();
  return o.m_verbose;
}

//----------------------------------------------------------------------------
// LogManager::result_stream
//----------------------------------------------------------------------------
std::ostream& LogManager::result_stream()
{
  LogManager &o = *Instance();

  if( o.m_iMinLevel > LOG_RESULT )
    return o.m_null;

  o.m_result.lock();
  return o.m_result;
}

//----------------------------------------------------------------------------
// LogManager::info_stream
//----------------------------------------------------------------------------
std::ostream& LogManager::info_stream()
{
  LogManager &o = *Instance();

  if( o.m_iMinLevel > LOG_INFO )
    return o.m_null;

  o.m_info.lock();
  return o.m_info;
}

//----------------------------------------------------------------------------
// LogManager::notice_stream
//----------------------------------------------------------------------------
std::ostream& LogManager::notice_stream()
{
  LogManager &o = *Instance();

  if( o.m_iMinLevel > LOG_NOTICE )
    return o.m_null;

  o.m_notice.lock();
  return o.m_notice;
}

//----------------------------------------------------------------------------
// LogManager::warning_stream
//----------------------------------------------------------------------------
std::ostream& LogManager::warning_stream()
{
  LogManager &o = *Instance();

  if( o.m_iMinLevel > LOG_WARNING )
    return o.m_null;

  o.m_warning.lock();
  return o.m_warning;
}

//----------------------------------------------------------------------------
// LogManager::error_stream
//----------------------------------------------------------------------------
std::ostream& LogManager::error_stream()
{
  LogManager &o = *Instance();

  if( o.m_iMinLevel > LOG_ERROR )
    return o.m_null;
  o.m_error.lock();
  return o.m_error;
}

//----------------------------------------------------------------------------
// LogManager::critical_stream
//----------------------------------------------------------------------------
std::ostream& LogManager::critical_stream()
{
  LogManager &o = *Instance();

  if( o.m_iMinLevel > LOG_CRITICAL )
    return o.m_null;
  o.m_critical.lock();
  return o.m_critical;
}

//----------------------------------------------------------------------------
// LogManager::alert_stream
//----------------------------------------------------------------------------
std::ostream& LogManager::alert_stream()
{
  LogManager &o = *Instance();

  if( o.m_iMinLevel > LOG_ALERT )
    return o.m_null;
  o.m_alert.lock();
  return o.m_alert;
}

//----------------------------------------------------------------------------
// LogManager::emergency_stream
//----------------------------------------------------------------------------
std::ostream& LogManager::emergency_stream()
{
  LogManager &o = *Instance();

  o.m_emergency.lock();
  return o.m_emergency;
}

//=============================================================================
// DefaultLogHandler
//=============================================================================
//----------------------------------------------------------------------------
// DefaultLogHandler::log
//----------------------------------------------------------------------------
void DefaultLogHandler::log(ELogLevel eLevel, pcsz pszType, const std::string& strMsg)
{
  // Formatting message
  CurrentTime dtCur;
  std::string strLogDate = yat::StringUtil::str_format("%4d-%02d-%02d,%02d:%02d:%06.3f",
                                        dtCur.year(), dtCur.month(), dtCur.day(),
                                        dtCur.hour(), dtCur.minute(), dtCur.second());

  std::string strLevel;
  switch( eLevel )
  {
    case LOG_EMERGENCY:
      strLevel = "EMERGENCY";
      break;
    case LOG_ALERT:
      strLevel = "ALERT";
      break;
    case LOG_CRITICAL:
      strLevel = "CRITICAL";
      break;
    case LOG_ERROR:
      strLevel = "ERROR";
      break;
    case LOG_WARNING:
      strLevel = "WARNING";
      break;
    case LOG_NOTICE:
      strLevel = "NOTICE";
      break;
    case LOG_INFO:
      strLevel = "INFO";
      break;
    case LOG_VERBOSE:
      strLevel = "VERBOSE";
      break;
    case LOG_RESULT:
      strLevel = "RESULT";
      break;
    case LOG_LEVEL_COUNT:
    default:
      break;
  }

  if( pszType )
    std::clog << strLogDate << ' ' << strLevel << " [" << pszType << "]:" << strMsg << '\n';
  else
    std::clog << strLogDate << " [" << strLevel << "] " << strMsg << '\n';
};

//=============================================================================
// LogCatcher
//=============================================================================
//----------------------------------------------------------------------------
// LogCatcher::LogCatcher
//----------------------------------------------------------------------------
LogCatcher::LogCatcher(ILogTarget *pLogTarget)
{
  LogManager::push_log_target(pLogTarget);
};

//----------------------------------------------------------------------------
// LogCatcher::~LogCatcher
//----------------------------------------------------------------------------
LogCatcher::~LogCatcher()
{
  LogManager::pop_log_target();
};

//=============================================================================
// LogForward
//=============================================================================
//----------------------------------------------------------------------------
// LogForward::LogForward
//----------------------------------------------------------------------------
LogForward::LogForward(pfn_log_fwd pfn_log_fwd)
{
  m_pfn_log_fwd = pfn_log_fwd;
}

//----------------------------------------------------------------------------
// LogForward::Log
//----------------------------------------------------------------------------
void LogForward::log(ELogLevel eLevel, pcsz pszType, const std::string& strMsg)
{
  if( m_pfn_log_fwd )
    m_pfn_log_fwd(eLevel, pszType, PSZ(strMsg));
}

#define BUF_LEN 4096
//=============================================================================
// Macro for Log functions
//=============================================================================
#define YAT_LOG_MSG(level)                                      \
  static char buf[BUF_LEN];                                     \
  static Mutex mtx;                                             \
  AutoMutex<> lock(mtx);                                        \
  va_list argptr;                                               \
  va_start(argptr, pszFormat);                                  \
  VSNPRINTF(buf, BUF_LEN, pszFormat, argptr);                   \
  va_end(argptr);                                               \
  std::string strMsg = buf;                                     \
  LogManager::log(level, pszType, strMsg);

// Log functions
void log_result(pcsz pszType, pcsz pszFormat, ...)    { YAT_LOG_MSG(LOG_RESULT) }
void log_verbose(pcsz pszType, pcsz pszFormat, ...)   { YAT_LOG_MSG(LOG_VERBOSE) }
void log_info(pcsz pszType, pcsz pszFormat, ...)      { YAT_LOG_MSG(LOG_INFO) }
void log_notice(pcsz pszType, pcsz pszFormat, ...)    { YAT_LOG_MSG(LOG_NOTICE) }
void log_warning(pcsz pszType, pcsz pszFormat, ...)   { YAT_LOG_MSG(LOG_WARNING) }
void log_error(pcsz pszType, pcsz pszFormat, ...)     { YAT_LOG_MSG(LOG_ERROR) }
void log_critical(pcsz pszType, pcsz pszFormat, ...)  { YAT_LOG_MSG(LOG_CRITICAL) }
void log_alert(pcsz pszType, pcsz pszFormat, ...)     { YAT_LOG_MSG(LOG_ALERT) }
void log_emergency(pcsz pszType, pcsz pszFormat, ...) { YAT_LOG_MSG(LOG_EMERGENCY) }

void log_result(const std::string& msg)    { LogManager::log(LOG_RESULT, "res", msg); }
void log_verbose(const std::string& msg)   { LogManager::log(LOG_VERBOSE, "vbs", msg); }
void log_info(const std::string& msg)      { LogManager::log(LOG_INFO, "inf", msg); }
void log_notice(const std::string& msg)    { LogManager::log(LOG_NOTICE, "not", msg); }
void log_warning(const std::string& msg)   { LogManager::log(LOG_WARNING, "wrn", msg); }
void log_error(const std::string& msg)     { LogManager::log(LOG_ERROR, "err", msg); }
void log_critical(const std::string& msg)  { LogManager::log(LOG_CRITICAL, "crt", msg); }
void log_alert(const std::string& msg)     { LogManager::log(LOG_ALERT, "alt", msg); }
void log_emergency(const std::string& msg) { LogManager::log(LOG_EMERGENCY, "emg", msg); }

void result(const std::string& msg)    { LogManager::log(LOG_RESULT, "", msg); }
void verbose(const std::string& msg)   { LogManager::log(LOG_VERBOSE, "", msg); }
void info(const std::string& msg)      { LogManager::log(LOG_INFO, "", msg); }
void notice(const std::string& msg)    { LogManager::log(LOG_NOTICE, "", msg); }
void warning(const std::string& msg)   { LogManager::log(LOG_WARNING, "", msg); }
void error(const std::string& msg)     { LogManager::log(LOG_ERROR, "", msg); }
void critical(const std::string& msg)  { LogManager::log(LOG_CRITICAL, "", msg); }
void alert(const std::string& msg)     { LogManager::log(LOG_ALERT, "", msg); }
void emergency(const std::string& msg) { LogManager::log(LOG_EMERGENCY, "", msg); }

} // namespace
