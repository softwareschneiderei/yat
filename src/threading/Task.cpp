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

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Task.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/threading/Task.i>
#endif // YAT_INLINE_IMPL

namespace yat
{

// ======================================================================
// Task::Config::Config
// ======================================================================
 Task::Config::Config ()
    : enable_timeout_msg (false),
      timeout_msg_period_ms (0),
      enable_periodic_msg (false),
      periodic_msg_period_ms (0),
      enable_precise_periodic_timing(false),
      lock_msg_handling (false),
      lo_wm (kDEFAULT_LO_WATER_MARK),
      hi_wm (kDEFAULT_HI_WATER_MARK),
      throw_on_post_tmo (false),
      user_data (0)
{
  /* noop ctor */
}

// ======================================================================
// Task::Config::Config
// ======================================================================
 Task::Config::Config (bool   _enable_timeout_msg,
                       size_t _timeout_msg_period_ms,
                       bool   _enable_periodic_msg,
                       size_t _periodic_msg_period_ms,
                       bool   _lock_msg_handling,
                       size_t _lo_wm,
                       size_t _hi_wm,
                       bool   _throw_on_post_tmo,
                       void * _user_data)
    : enable_timeout_msg (_enable_timeout_msg),
      timeout_msg_period_ms (_timeout_msg_period_ms),
      enable_periodic_msg (_enable_periodic_msg),
      periodic_msg_period_ms (_periodic_msg_period_ms),
      enable_precise_periodic_timing (false),
      lock_msg_handling (_lock_msg_handling),
      lo_wm (_lo_wm),
      hi_wm (_hi_wm),
      throw_on_post_tmo (_throw_on_post_tmo),
      user_data (_user_data)
{
  /* noop ctor */
}

// ======================================================================
// Task::Config::Config
// ======================================================================
 Task::Config::Config (bool   _enable_timeout_msg,
                       size_t _timeout_msg_period_ms,
                       bool   _enable_periodic_msg,
                       double _periodic_msg_period_ms,
                       bool   _enable_precise_periodic_timing,
                       bool   _lock_msg_handling,
                       size_t _lo_wm,
                       size_t _hi_wm,
                       bool   _throw_on_post_tmo,
                       void * _user_data)
    : enable_timeout_msg (_enable_timeout_msg),
      timeout_msg_period_ms (_timeout_msg_period_ms),
      enable_periodic_msg (_enable_periodic_msg),
      periodic_msg_period_ms (_periodic_msg_period_ms),
      enable_precise_periodic_timing (_enable_precise_periodic_timing),
      lock_msg_handling (_lock_msg_handling),
      lo_wm (_lo_wm),
      hi_wm (_hi_wm),
      throw_on_post_tmo (_throw_on_post_tmo),
      user_data (_user_data)
{
  /* noop ctor */
}

// ======================================================================
// Task::Task
// ======================================================================
Task::Task ()
  : msg_q_ (kDEFAULT_LO_WATER_MARK, kDEFAULT_HI_WATER_MARK),
    timeout_msg_period_ms_ (0),
    periodic_msg_period_ms_ (0),
    precise_periodic_timing_enabled_(false),
    user_data_ (0),
    lock_msg_handling_ (false)
{
  YAT_TRACE("Task::Task");

#if defined (YAT_DEBUG)
  this->next_msg_counter = 0;
  this->ctrl_msg_counter = 0;
  this->user_msg_counter = 0;
#endif
}

// ======================================================================
// Task::Task
// ======================================================================
Task::Task (const Task::Config& cfg)
  : msg_q_ (cfg.lo_wm, cfg.hi_wm, cfg.throw_on_post_tmo),
    timeout_msg_period_ms_ (cfg.timeout_msg_period_ms),
    periodic_msg_period_ms_ (cfg.periodic_msg_period_ms),
    precise_periodic_timing_enabled_ (cfg.enable_precise_periodic_timing),
    user_data_ (cfg.user_data),
    lock_msg_handling_ (cfg.lock_msg_handling),
    received_init_msg_(false)
{
  YAT_TRACE("Task::Task");

  msg_q_.enable_timeout_msg_ = cfg.enable_timeout_msg;
  msg_q_.enable_periodic_msg_ = cfg.enable_periodic_msg;

#if defined (YAT_DEBUG)
  this->next_msg_counter = 0;
  this->ctrl_msg_counter = 0;
  this->user_msg_counter = 0;
#endif
}

// ======================================================================
// Task::~Task
// ======================================================================
Task::~Task ()
{
  YAT_TRACE("Task::~Task");

#if defined (YAT_DEBUG)
  YAT_LOG("Task::run_undetached::entered "
        << this->next_msg_counter
        << " times in MsgQ::next_message");

  YAT_LOG("Task::run_undetached::ctrl msg:: "
        << this->ctrl_msg_counter);

  YAT_LOG("Task::run_undetached::user msg:: "
        << this->user_msg_counter);

  YAT_LOG("Task::run_undetached::total processed msg:: "
        << this->user_msg_counter + this->ctrl_msg_counter);
#endif
}

// ============================================================================
// Task::go_synchronously
// ============================================================================
void Task::go_synchronously (size_t _tmo_ms)
{
  YAT_TRACE("Task::go_synchronously");

  this->go(_tmo_ms);
}

// ============================================================================
// Task::go
// ============================================================================
void Task::go (size_t _tmo_ms)
{
  YAT_TRACE("Task::go");

  this->start_undetached();

  Message * msg = 0;
  try
  {
    msg = Message::allocate (TASK_INIT, INIT_MSG_PRIORITY, true);
  }
  catch (Exception& ex)
  {
    RETHROW_YAT_ERROR(ex,
                      "OUT_OF_MEMORY",
                      "Message allocation failed",
                      "Task::go");
  }

  this->wait_msg_handled (msg, _tmo_ms);
}

// ============================================================================
// Task::go_synchronously
// ============================================================================
void Task::go_synchronously (Message * _msg, size_t _tmo_ms)
{
  YAT_TRACE("Task::go_synchronously");

  this->go(_msg, _tmo_ms);
}

// ============================================================================
// Task::go
// ============================================================================
void Task::go (Message * _msg, size_t _tmo_ms)
{
  YAT_TRACE("Task::go");

  this->start_undetached();

  if (
         (_msg == 0)
      ||
         (_msg->type() != TASK_INIT)
      ||
         (_msg->waitable() == false)
     )
     THROW_YAT_ERROR("PROGRAMMING_ERROR",
                     "invalid INIT message [null, wrong type or not waitable]",
                     "Task::go");

  this->wait_msg_handled (_msg, _tmo_ms);
}

// ============================================================================
// Task::go_asynchronously
// ============================================================================
void Task::go_asynchronously (size_t _tmo_ms)
{
  YAT_TRACE("Task::go_asynchronously");

  this->start_undetached();

  Message * msg = 0;
  try
  {
    msg = Message::allocate (TASK_INIT, INIT_MSG_PRIORITY, true);
  }
  catch (Exception& ex)
  {
    RETHROW_YAT_ERROR(ex,
                      "OUT_OF_MEMORY",
                      "Message allocation failed",
                      "Task::go");
  }

  this->post(msg, _tmo_ms);
}

// ============================================================================
// Task::go_asynchronously
// ============================================================================
void Task::go_asynchronously (Message * _msg, size_t _tmo_ms)
{
  YAT_TRACE("Task::go_asynchronously");

  this->start_undetached();

  if (
         (_msg == 0)
      ||
         (_msg->type() != TASK_INIT)
     )
     THROW_YAT_ERROR("PROGRAMMING_ERROR",
                     "invalid INIT message [null or wrong type]",
                     "Task::go");

  this->post(_msg, _tmo_ms);
}


// ============================================================================
// Task::run_undetached
// ============================================================================
void * Task::run_undetached (void *)
{
  YAT_TRACE("Task::run_undetached");

#if defined (YAT_DEBUG)
  yat::Timestamp last_notification_timestamp, now;
#endif

  //- init flag - set to true when TASK_INIT received
  this->received_init_msg_ = false;

  size_t msg_type;
  Message * msg = 0;

  //- actual tmo on msg waiting
  double tmo = this->actual_timeout();

  //- trick: avoid handling TIMEOUT messages before INIT is itself handled
  //- may be the case for very short Task timeout
  do
  {
    //- release any previously obtained msg
    if (msg) msg->release();
#if defined (YAT_DEBUG)
    this->next_msg_counter++;
#endif
    //- get/wait next message from the msgQ
    if( precise_periodic_timing_enabled_ )
      msg = this->msg_q_.next_message_ex (tmo);
    else
      msg = this->msg_q_.next_message (tmo);
    //- mark TASK_INIT has received
    if ( msg && msg->type() == TASK_INIT )
      this->received_init_msg_ = true;
  }
  while ( ! this->received_init_msg_ );

  //- exit flag - set to true when TASK_EXIT received
  bool received_exit_msg = false;

  //- enter thread's main loop
  while ( ! received_exit_msg )
  {
    //- actual tmo on msg waiting
    tmo = this->actual_timeout();

#if defined (YAT_DEBUG)
    YAT_LOG("Task::run_undetached::waiting next msg or tmo::"
            << this->actual_timeout());
#endif

    //- get/wait next message from the msgQ
    if (! msg)
    {
      do
      {
#if defined (YAT_DEBUG)
        this->next_msg_counter++;
#endif
        //- get next message
        if( precise_periodic_timing_enabled_ )
          msg = this->msg_q_.next_message_ex (tmo);
        else
          msg = this->msg_q_.next_message (tmo);
        //- do not handle TASK_INIT twice
        if (msg && msg->type() == TASK_INIT && this->received_init_msg_)
        {
          msg->release();
          msg = 0;
        }
      }
      while (! msg);
      //- special case: ignore self-posted WAKEUP msg
      if (msg->type() == TASK_WAKEUP)
      {
        msg->release();
        msg = 0;
        continue;
      }
    }

#if defined (YAT_DEBUG)
    _GET_TIME (now);
    YAT_LOG("Task::run_undetached::handling msg::elapsed msecs since last msg::"
            << _ELAPSED_MSEC(last_notification_timestamp, now)
            << " - actual tmo::"
            << this->actual_timeout());
    _GET_TIME (last_notification_timestamp);
#endif

    //- set msg user data
    msg->user_data(this->user_data_);

    //- we may need msg type after msg release
    msg_type = msg->type();

#if defined (YAT_DEBUG)
    if (msg->is_task_ctrl_message ())
      this->ctrl_msg_counter++;
    else
      this->user_msg_counter++;

   YAT_LOG("Task::run_undetached::handling msg ["
           << std::hex
           << (void*)msg
           << std::dec
           << "]");
#endif

    //- got a valid message from message Q
    try
    {
      //- std::cout << "Task::handling msg::"
      //-            << std::hex
      //-            << long(msg)
      //-            << std::dec
      //-            << "::"
      //-            << msg->to_string()
      //-            << std::endl;
      //- call message handler
      if (this->lock_msg_handling_)
      {
        //- enter critical section
        MutexLock guard (this->m_lock);
        this->handle_message (*msg);
      }
      else
      {
        this->handle_message (*msg);
      }
    }
    catch (const Exception& e)
    {
      //- store exception into the message
      msg->set_error(e);
    }
    catch (...)
    {
      Exception e("UNKNOWN_ERROR",
                  "unknown error caught while handling msg",
                  "Task::run_undetached");
      //- store exception into the message
      msg->set_error(e);
    }
#if defined (YAT_DEBUG)

   YAT_LOG("Task::run_undetached::msg ["
           << std::hex
           << (void*)msg
           << std::dec
           << "] handled - notifying waiters");
#endif
    //- mark message as "processed" (this will signal waiters if any)
    msg->processed();
    //- release our msg ref
    msg->release();
    //- abort requested?
    if (msg_type == TASK_EXIT)
    {
      //- close the msgQ
      this->msg_q_.close();
      //- mark TASK_EXIT as received (exit thread's main loop)
      received_exit_msg = true;
    }
    //- reset msg in order to get next msg from msgQ
    msg = 0;
  } //- thread's while loop

  return 0;
}

// ======================================================================
// Task::exit
// ======================================================================
void Task::exit ()
{
  YAT_TRACE("Task::exit");

  //- we may have to implicitly delete the thread
  bool delete_self = false;

  //- enter critical section
  this->m_lock.lock();

  //- get underlying thread state
  Thread::State ts = this->state_i();

  //- if the thread is running, then ask it to exit
  if (ts == yat::Thread::STATE_RUNNING)
  {
    yat::Message * msg = 0;
    try
    {
      msg = Message::allocate (yat::TASK_EXIT, EXIT_MSG_PRIORITY, true);
    }
    catch (Exception &ex)
    {
      this->m_lock.unlock();
      RETHROW_YAT_ERROR(ex,
                        "SOFTWARE_ERROR",
                        "Could not stop task [yat::Message allocation failed]",
                        "Task::exit");
    }
    catch (...)
    {
      this->m_lock.unlock();
      THROW_YAT_ERROR("UNKNOWN_ERROR",
                      "Could not stop task [yat::Message allocation failed]",
                      "Task::exit");
    }
    //- unlock the thread lock (avoid deadlock during message handling)
    this->m_lock.unlock();
    try
    {
#if defined (YAT_DEBUG)
      //- ... then wait for TASK_EXIT msg to be handled
      //- TODO: change kINFINITE_WAIT to a more flexible TIMEOUT
      YAT_LOG("Task::exit - waiting for the TASK_EXIT msg to be handled");
#endif
      this->wait_msg_handled (msg, kINFINITE_WAIT);
    }
    catch (...)
    {
      //- ignore any error
    }
    //- wait for the thread to actually quit
    try
    {
      Thread::IOArg dummy = 0;
#if defined (YAT_DEBUG)
      YAT_LOG("Task::exit - about to join with the underlying thread");
#endif
      this->join (&dummy);
    }
    catch (...)
    {
     //- ignore any error
    }
  }
  else if (ts == yat::Thread::STATE_NEW)
  {
#if defined (YAT_DEBUG)
    //- delete the thread (instanciated but never been started)
    YAT_LOG("Task::exit - about to delete the thread [has never been started]");
#endif
    delete_self = true;
    //- leave critical section
    this->m_lock.unlock();
  }
  else
  {
#if defined (YAT_DEBUG)
    //- nothing to do...
    YAT_LOG("Task::exit - do nothing");
#endif
    //- leave critical section
    this->m_lock.unlock();
  }

  //- delete (if required)
  if (delete_self)
  {
#if defined (YAT_DEBUG)
    YAT_LOG("Task::exit - deleting <this> Task instance");
#endif
    delete this;
  }
}

// ======================================================================
// Task::wait_msg_handled
// ======================================================================
void Task::wait_msg_handled (Message * _msg, size_t _tmo_ms)
{
#if defined (YAT_DEBUG)
  YAT_TRACE("Task::wait_msg_handled");
#endif
  //- check input
  if (! _msg || ! _msg->waitable())
  {
    if (_msg)
      _msg->release();
    THROW_YAT_ERROR("INVALID_ARGUMENT",
                    "invalid message [either null or not waitable]",
                    "Task::wait_msg_handled");
  }

  try
  {
    //- post a shallow copy of the msg
    this->msg_q_.post(_msg->duplicate());
  }
  catch (...)
  {
    _msg->release();
    THROW_YAT_ERROR("INTERNAL_ERROR",
                    "message could not be posted",
                    "Task::wait_msg_handled");
  }
#if defined (YAT_DEBUG)
  YAT_LOG("Task::wait_msg_handled::waiting for msg ["
          << std::hex
          << (void*)_msg
          << std::dec
          << "] to be handled");
#endif
  //- wait for the msg to be handled or tmo expiration
  if (_msg->wait_processed(_tmo_ms))
  {
#if defined (YAT_DEBUG)
    YAT_LOG("Task::wait_msg_handled::msg ["
            << std::hex
            << (void*)_msg
            << std::dec
            << "] handled [gave error::"
            << (_msg->has_error() ? "yes" : "no")
            << "]" );
#endif
    Exception msg_exception;
    bool msg_gave_error = _msg->has_error();
    //- to store error localy before releasing msg
    if (msg_gave_error)
      msg_exception = _msg->get_error();
    //- release msg
    _msg->release();
    //- throw an exception if msg gave error
    if (msg_gave_error)
      throw msg_exception;
    //- msg did not gave error, just return
    return;
  }
#if defined (YAT_DEBUG)
  //- too bad, timeout expired...
  YAT_LOG("Task::wait_msg_handled::timeout expired while waiting for msg ["
          << std::hex
          << (void*)_msg
          << std::dec
          << "] to be handled");
#endif
  //- release msg
  _msg->release();

  //- throw timeout exception
  THROW_YAT_ERROR("TIMEOUT_EXPIRED",
                  "timeout expired while waiting for message to be handled",
                  "Task::wait_msg_handled");
}

// ======================================================================
// Task::reset_msgq_statistics
// ======================================================================
void Task::reset_msgq_statistics ()
{
  this->msg_q_.reset_statistics ();
}

// ======================================================================
// Task::clear_pending_messages
// ======================================================================
size_t Task::clear_pending_messages ()
{
  return this->msg_q_.clear();
}

// ======================================================================
// Task::clear_pending_messages
// ======================================================================
size_t Task::clear_pending_messages (size_t msg_type)
{
  return this->msg_q_.clear_pending_messages(msg_type);
}

// ======================================================================
// TaskExiter::operator()
// ======================================================================
void TaskExiter::operator()(Task* task)
{
  try
  {
#if defined (YAT_DEBUG)
    YAT_TRACE( "Exiting Task object @" << (void*)task );
#endif
    task->exit();
  }
  catch(...)
  {
  }
}


} // namespace
