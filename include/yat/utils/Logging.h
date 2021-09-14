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

#ifndef __YAT_LOGGING_H__
#define __YAT_LOGGING_H__

#include <stack>
#include <set>
#include <sstream>
#include <yat/CommonHeader.h>
#include <yat/memory/UniquePtr.h>
#include <yat/utils/String.h>

namespace yat
{

// =============================================================================
//! \brief Defines severity types using "syslog" levels definitions.
// =============================================================================
enum ELogLevel
{
  //! To know every executed actions at low level.
  LOG_VERBOSE = 0,
  //! This is not an information message but a real result that must not be filtered.
  LOG_RESULT,
  //! This is the default message level, it reports normal information about the system or the application.
  LOG_INFO,
  //! Describes a (perhaps unusual) event that is important to report.
  LOG_NOTICE,
  //! Reports a warning.
  LOG_WARNING,
  //! An error occurred but the system is still functional.
  LOG_ERROR,
  //! Critical error.
  LOG_CRITICAL,
  //! Immediate fixing is needed.
  LOG_ALERT,
  //! The system will shutdown now because of an unrecoverable failure.
  LOG_EMERGENCY,
  //! No level specified
  LOG_NOLEVEL,
  //! Not a log level, just the total number of levels.
  LOG_LEVEL_COUNT
};

// ============================================================================
//! \class NullStream
//! \brief NullStream object inherited from std::ostream
//!
//! It's used to drop logs
// ============================================================================
struct NullStream : std::ostream
{
  NullStream(): std::ios(0), std::ostream(0) {}
};
template <typename T>
NullStream& operator<<(NullStream& ns, T) { return ns; }

// ============================================================================
//! \class LogStream
//! \brief LogStream object inherited from std::ostream & std::streambuf.
//!
//! It provide a set of stream for serializing log messages
// ============================================================================
class LogStream : public std::ostream, public std::streambuf
{
  friend class LogManager;

public:
  LogStream(ELogLevel level);

private:
  yat::String                       m_the_message;
  ELogLevel                         m_level;
  class ILogTarget*                 m_log_target_p;
  yat::UniquePtr<class yat::Mutex>  m_mtx_msg_uptr;
  yat::UniquePtr<class yat::Mutex>  m_mtx_locker_uptr;
  unsigned long                     m_locked_thread;
  bool                              m_locked;

  int sync();
  int overflow(int c);
  void lock();
  std::streamsize xsputn(char const* p, std::streamsize n);
};

// ============================================================================
//! \class ILogTarget
//! \brief Log target interface.
//!
//! This is the base class for logging management.
//! This abstract class can not be used as this and must be derived.
//!
//! \par %Message logging usage:
//! Just instantiate a LogManager and log messages using the yat::log_xxx() functions
//! to log into the clog %stream.
//! \par
//! If a special log storage/presentation is necessary, then the things to do are:
//!  - implement the ILogTarget interface,
//!  - instantiate a LogCatcher object with a pointer to the ILogTarget object.
// ============================================================================
class YAT_DECL ILogTarget
{
public:
  //! \brief Logs message with specified level.
  //! \param eLevel Log level.
  //! \param pszType %Message type.
  //! \param strMsg %Message to log.
  virtual void log(ELogLevel eLevel, pcsz pszType, const std::string &strMsg)=0;
};

//! \brief Log target stack, for logging redirection.
typedef std::stack<class ILogTarget *> LogTargetStack;

// ============================================================================
//! \class DefaultLogHandler
//! \brief Default log handler: prints log on console using clog %stream.
//!
//! Inherits from ILogTarget class.
// ============================================================================
class YAT_DECL DefaultLogHandler: public ILogTarget
{
public:
  //! \brief Logs a message on the clog %stream.
  //! \param eLevel Log level.
  //! \param pszType %Message type.
  //! \param strMsg %Message to log.
  void log(ELogLevel eLevel, pcsz pszType, const std::string &strMsg);
};


// ============================================================================
//! \class LogManager
//! \brief Log manager class.
//!
//! This class implements a log manager that provides logging functions with filter
//! capabilities (filter on message type, on message level). \n
//! This class is a singleton.
// ============================================================================
class YAT_DECL LogManager
{
friend class LogCatcher;

public:

  //! \brief Initializes the log handler.
  //!
  //! The logged messages will be filtered with severity level and types.
  //! \param iMinLevel Minimum severity level.
  //! \param strFilter List (separator = '|') of message types used for message filtering.
  static void init(int iMinLevel, const std::string &strFilter= yat::StringUtil::empty);

  //! \brief Logs a message.
  //! \param eLevel Log level.
  //! \param pszType %Message type.
  //! \param strMsg %Message to log.
  static void log(ELogLevel eLevel, pcsz pszType, const std::string &strMsg);

  //! \brief Gets the minimum log level taken into account.
  static int min_level() { return Instance()->m_iMinLevel; }

  //! Returns the current log target.
  static ILogTarget *current_log_target();

  // Do not use directly the following methods but use the macros defined below
  static std::ostream& verbose_stream();
  static std::ostream& result_stream();
  static std::ostream& info_stream();
  static std::ostream& warning_stream();
  static std::ostream& error_stream();
  static std::ostream& notice_stream();
  static std::ostream& critical_stream();
  static std::ostream& alert_stream();
  static std::ostream& emergency_stream();

private:

  NullStream m_null;
  LogStream  m_verbose;
  LogStream  m_result;
  LogStream  m_info;
  LogStream  m_warning;
  LogStream  m_error;
  LogStream  m_notice;
  LogStream  m_critical;
  LogStream  m_alert;
  LogStream  m_emergency;

  static LogManager *ms_pTheInstance;
  static LogManager *Instance();
  DefaultLogHandler  m_defLogHandler;

  std::string        m_stream_current;

  //- Log target
  LogTargetStack     m_stkCatchLogTarget;

  //- Min severity level of logged messages
  int         m_iMinLevel;

  //- Logged messages types
  std::set<std::string>  m_setTypes;

  //- Add a new log target to the stack
  static void push_log_target(ILogTarget *pLogTarget);

  //- Remove top log target
  static void pop_log_target();

  //- Constructor
  LogManager();

  void update_stream_log_target(ILogTarget *pLogTarget);
};

//! \brief Log forwarding function type declaration.
typedef void (*pfn_log_fwd)(int iLevel, const char *pszType, const char *pszMsg);

// ============================================================================
//! \class LogForward
//! \brief Helper class for log forwarding.
//!
//! This class provide an interface to forward messages logged towards another function.
//! This function is defined with the yat::pfn_log_fwd type.
//!
//! Inherits from ILogTarget class.
// ============================================================================
class YAT_DECL LogForward: public ILogTarget
{
private:
  //- Function to forward log to
  pfn_log_fwd m_pfn_log_fwd;

public:
  //! \brief Constructor.
  //! \param pfn_log_fwd Pointer to function to forward log to.
  LogForward(pfn_log_fwd pfn_log_fwd);

  //! \brief Logs a message.
  //! \param eLevel Log level.
  //! \param pszType %Message type.
  //! \param strMsg %Message to log.
  virtual void log(ELogLevel eLevel, pcsz pszType, const std::string &strMsg);
};

// ============================================================================
//! \class LogCatcher
//! \brief Log catcher class.
//!
//! This class defines an object that catches log, during the LogCatcher life time.
//!
//! LogCatcher object are managed as following:
//! - when a LogCatcher object die, then the previous object still alive will receive messages;
//! - when the first created LogCatcher is deleted, messages are catched by the DefaultLogHandler.
//!
//! \par %Message logging usage:
//! Just instantiate a LogManager and log messages using the yat::log_xxx() functions
//! to log into the clog %stream.
//! \par
//! If a special log storage/presentation is necessary, then the things to do are:
//!  - implement the ILogTarget interface,
//!  - instantiate a LogCatcher object with a pointer to the ILogTarget object.
//!
// ============================================================================
class YAT_DECL LogCatcher
{
public:
  //! \brief Constructor.
  //!
  //! Pushes the log target that catches log in ClogHandler stack.
  //! \param pLogTarget Log target.
  LogCatcher(ILogTarget *pLogTarget);

  //! \brief Destructor.
  //!
  //! Removes top log target from the stack.
  ~LogCatcher();
};

// =============================================================================
//! Log functions
// =============================================================================
//@{

//! \brief log a verbose message with format
//! \verbatim
//! yat::verbose("A message with an argument value: {}.").arg(value);
//! yat::verbose("A simple message");
//! \endverbatim
class YAT_DECL LogFormat verbose(const std::string& msg_fmt);
//! \brief log a notice message with format
class YAT_DECL LogFormat notice(const std::string& msg_fmt);
//! \brief log a result message with format
class YAT_DECL LogFormat result(const std::string& msg_fmt);
//! \brief log a info message with format
class YAT_DECL LogFormat info(const std::string& msg_fmt);
//! \brief log a warning message with format
class YAT_DECL LogFormat warning(const std::string& msg_fmt);
//! \brief log a error message with format
class YAT_DECL LogFormat error(const std::string& msg_fmt);
//! \brief log a alert message with format
class YAT_DECL LogFormat alert(const std::string& msg_fmt);
//! \brief log a emergency message with format
class YAT_DECL LogFormat emergency(const std::string& msg_fmt);
//! \brief log a critical message with format
class YAT_DECL LogFormat critical(const std::string& msg_fmt);
//! \brief log a generic message with format
class YAT_DECL LogFormat log(const std::string& msg_fmt);

// Internal class used to log formatted messages
// This object can't be manually allocated nor copied
class YAT_DECL LogFormat : public Format
{
// methods allowed to construct a LogFormat object
friend LogFormat verbose(const std::string& msg_fmt);
friend LogFormat notice(const std::string& msg_fmt);
friend LogFormat result(const std::string& msg_fmt);
friend LogFormat info(const std::string& msg_fmt);
friend LogFormat warning(const std::string& msg_fmt);
friend LogFormat error(const std::string& msg_fmt);
friend LogFormat alert(const std::string& msg_fmt);
friend LogFormat emergency(const std::string& msg_fmt);
friend LogFormat critical(const std::string& msg_fmt);
friend LogFormat log(const std::string& msg_fmt);

public:
  template<typename T> LogFormat& arg(T v)
  {
    try { Format::arg(v); }
    catch(...) {}
    return *this;
  }

  ~LogFormat()
  {
    switch( level_ )
    {
      case LOG_VERBOSE:
        LogManager::log(LOG_VERBOSE, "", get());
        break;
      case LOG_NOTICE:
        LogManager::log(LOG_NOTICE, "", get());
        break;
      case LOG_RESULT:
        LogManager::log(LOG_RESULT, "", get());
        break;
      case LOG_INFO:
        LogManager::log(LOG_INFO, "", get());
        break;
      case LOG_WARNING:
        LogManager::log(LOG_WARNING, "", get());
        break;
      case LOG_ERROR:
        LogManager::log(LOG_ERROR, "", get());
        break;
      case LOG_CRITICAL:
        LogManager::log(LOG_CRITICAL, "", get());
        break;
      case LOG_ALERT:
        LogManager::log(LOG_ALERT, "", get());
        break;
      case LOG_EMERGENCY:
        LogManager::log(LOG_EMERGENCY, "", get());
        break;
      case LOG_NOLEVEL:
        LogManager::log(LOG_NOLEVEL, "", get());
        break;
      default:
        break;
    }
  }

private:
  LogFormat(const std::string& s, ELogLevel l): yat::Format(s),
   level_(l) {  }

  // unused & unusable
  LogFormat& operator=(const LogFormat&) { return *this;}
  LogFormat(const LogFormat&) : yat::Format() {}

  ELogLevel level_;
};

//@} Log functions

// =============================================================================
//! \deprecated Log functions
//@{
YAT_DECL void log_result(pcsz pszType, pcsz pszFormat, ...);
YAT_DECL void log_verbose(pcsz pszType, pcsz pszFormat, ...);
YAT_DECL void log_info(pcsz pszType, pcsz pszFormat, ...);
YAT_DECL void log_notice(pcsz pszType, pcsz pszFormat, ...);
YAT_DECL void log_warning(pcsz pszType, pcsz pszFormat, ...);
YAT_DECL void log_error(pcsz pszType, pcsz pszFormat, ...);
YAT_DECL void log_critical(pcsz pszType, pcsz pszFormat, ...);
YAT_DECL void log_alert(pcsz pszType, pcsz pszFormat, ...);
YAT_DECL void log_emergency(pcsz pszType, pcsz pszFormat, ...);
YAT_DECL void log_result(const std::string& msg);
YAT_DECL void log_verbose(const std::string& msg);
YAT_DECL void log_info(const std::string& msg);
YAT_DECL void log_notice(const std::string& msg);
YAT_DECL void log_warning(const std::string& msg);
YAT_DECL void log_error(const std::string& msg);
YAT_DECL void log_critical(const std::string& msg);
YAT_DECL void log_alert(const std::string& msg);
YAT_DECL void log_emergency(const std::string& msg);
//@} Deprecated Log functions

} // namespace


//! \deprecated
#define LOG_EXCEPTION(domain, e) \
  do \
  { \
    for (size_t i = 0; i < e.errors.size(); i++) \
      yat::LogManager::error_stream() << e.errors[i].reason << ". " \
                                      << e.errors[i].desc << ". "   \
                                      << e.errors[i].origin << std::endl; \
  } while(0)

//! \deprecated
#define YAT_LOG_EXCEPTION(e)        LOG_EXCEPTION("exc", e)
#define YAT_LOG_VERBOSE(...)     yat::log_verbose("vbs", __VA_ARGS__)
#define YAT_LOG_RESULT(...)       yat::log_result("res", __VA_ARGS__)
#define YAT_LOG_INFO(...)           yat::log_info("inf", __VA_ARGS__)
#define YAT_LOG_NOTICE(...)       yat::log_notice("not", __VA_ARGS__)
#define YAT_LOG_WARNING(...)     yat::log_warning("wrn", __VA_ARGS__)
#define YAT_LOG_ERROR(...)         yat::log_error("err", __VA_ARGS__)
#define YAT_LOG_CRITICAL(...)   yat::log_critical("crt", __VA_ARGS__)
#define YAT_LOG_ALERT(...)         yat::log_alert("alt", __VA_ARGS__)
#define YAT_LOG_EMERGENCY(...) yat::log_emergency("emg", __VA_ARGS__)

//=============================================================================
//! \deprecated Macro for stream log functions
//=============================================================================
#define YAT_MSG_STREAM(level, type, s)                        \
do {                                                          \
  std::ostringstream oss;                                     \
  oss << s;                                                   \
  yat::LogManager::log(level, type, oss.str());               \
} while(0)

//! \deprecated macros YAT_XXX_STREAM are deprecated. use YAT_XXX instead
#define YAT_VERBOSE_STREAM(s)   YAT_MSG_STREAM(yat::LOG_VERBOSE, "vbs", s)
#define YAT_RESULT_STREAM(s)    YAT_MSG_STREAM(yat::LOG_RESULT, "res", s)
#define YAT_INFO_STREAM(s)      YAT_MSG_STREAM(yat::LOG_INFO, "inf", s)
#define YAT_WARNING_STREAM(s)   YAT_MSG_STREAM(yat::LOG_WARNING, "wrn", s)
#define YAT_NOTICE_STREAM(s)    YAT_MSG_STREAM(yat::LOG_NOTICE, "not", s)
#define YAT_ERROR_STREAM(s)     YAT_MSG_STREAM(yat::LOG_ERROR, "err", s)
#define YAT_CRITICAL_STREAM(s)  YAT_MSG_STREAM(yat::LOG_CRITICAL, "crt", s)
#define YAT_ALERT_STREAM(s)     YAT_MSG_STREAM(yat::LOG_ALERT, "alt", s)
#define YAT_EMERGENCY_STREAM(s) YAT_MSG_STREAM(yat::LOG_EMERGENCY, "emg", s)

//! \brief stream log macros
#define YAT_VERBOSE   yat::LogManager::verbose_stream()
#define YAT_RESULT    yat::LogManager::result_stream()
#define YAT_INFO      yat::LogManager::info_stream()
#define YAT_WARNING   yat::LogManager::warning_stream()
#define YAT_NOTICE    yat::LogManager::notice_stream()
#define YAT_ERROR     yat::LogManager::error_stream()
#define YAT_CRITICAL  yat::LogManager::critical_stream()
#define YAT_ALERT     yat::LogManager::alert_stream()
#define YAT_EMERGENCY yat::LogManager::emergency_stream()

#define YAT_EOL  std::endl

//=============================================================================
//! \def YAT_FREQUENCY_LIMITED_STATEMENT
//! \brief  MACRO to help executing statements that have to be limited in frequency
//! \brief like log messages in a high frequency loop
//!
//! \param statement the code to execute
//! \param interval_sec minimal time interval, is seconds, between to execution of the statement
//!
//! \warning In a multithreaded application, and because the macro uses static data,
//!          it may seems to not working properly if more than one thread execute
//!          it from the same location in the source code!
//!
//! \verbatim
//! YAT_FREQUENCY_LIMITED_STATEMENT
//!   (
//!     YAT_VERBOSE << "bla bla" << std::endl,
//!     1
//!   );
//! \endverbatim
//=============================================================================
#define YAT_FREQUENCY_LIMITED_STATEMENT(statement, interval_sec) \
do \
{ \
  static yat::Timer _s_yat_frequency_limited_statement_timer_; \
  static bool _s_yat_frequency_limited_statement_timer_first_exec_ = false; \
  if( !_s_yat_frequency_limited_statement_timer_first_exec_ || \
     _s_yat_frequency_limited_statement_timer_.elapsed_sec() >= interval_sec ) \
  { \
   statement;               \
   _s_yat_frequency_limited_statement_timer_.restart();     \
   if( !_s_yat_frequency_limited_statement_timer_first_exec_ )    \
     _s_yat_frequency_limited_statement_timer_first_exec_ = true; \
  } \
} while(0)


#endif
