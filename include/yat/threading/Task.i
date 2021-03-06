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

namespace yat
{

// ============================================================================
// Task::post
// ============================================================================
YAT_INLINE void Task::post (size_t msg_type, size_t tmo_msecs)
{
  Message * m = new (std::nothrow) Message(msg_type);
  if (! m)
  {
    THROW_YAT_ERROR("OUT_OF_MEMORY",
                    "yat::Message allocation failed",
                    "Task::post");
  }
  this->post(m, tmo_msecs);
}

// ============================================================================
// Task::wait_msg_handled
// ============================================================================
YAT_INLINE void Task::wait_msg_handled (size_t msg_type, size_t tmo_msecs)
{
  Message * m = new (std::nothrow) Message(msg_type, DEFAULT_MSG_PRIORITY, true);
  if (! m)
  {
    THROW_YAT_ERROR("OUT_OF_MEMORY",
                    "yat::Message allocation failed",
                    "Task::post");
  }
  this->wait_msg_handled(m, tmo_msecs);
}

// ============================================================================
// Task::message_queue
// ============================================================================
YAT_INLINE MessageQ & Task::message_queue ()
{
  return this->msg_q_;
}

// ============================================================================
// Task::enable_timeout_msg
// ============================================================================
YAT_INLINE void Task::enable_timeout_msg (bool behaviour)
{
  this->msg_q_.enable_timeout_msg_ = behaviour;
}

// ============================================================================
// Task::timeout_msg_enabled
// ============================================================================
YAT_INLINE bool Task::timeout_msg_enabled () const
{
  return this->msg_q_.enable_timeout_msg_;
}

// ============================================================================
// Task::set_timeout_msg_period
// ============================================================================
YAT_INLINE void Task::set_timeout_msg_period (std::size_t _tmo)
{
  if( _tmo > ((std::size_t)(-1) >> 1) )
    THROW_YAT_ERROR("BAD_ARG",
                    "timeout msg period too high (may be a negative value is passed)",
                    "Task::set_timeout_msg_period");
  this->timeout_msg_period_ms_ = _tmo;
}

// ============================================================================
// Task::get_timeout_msg_period
// ============================================================================
YAT_INLINE size_t Task::get_timeout_msg_period () const
{
  return this->timeout_msg_period_ms_;
}

// ============================================================================
// Task::enable_periodic_msg
// ============================================================================
YAT_INLINE void Task::enable_periodic_msg (bool b)
{
  bool was_not_enabled = ! this->msg_q_.enable_periodic_msg_;
  this->msg_q_.enable_periodic_msg_ = b;
  if ( was_not_enabled && this->msg_q_.enable_periodic_msg_ && this->received_init_msg_ )
  {
    this->post(TASK_WAKEUP);
  }
}

// ============================================================================
// Task::enable_precise_periodic_timing
// ============================================================================
YAT_INLINE void Task::enable_precise_periodic_timing (bool b)
{
  bool enable_state = this->precise_periodic_timing_enabled_;
  this->precise_periodic_timing_enabled_ = b;
  if ( enable_state != b && this->received_init_msg_ )
  {
    this->post(TASK_WAKEUP);
  }
}

// ============================================================================
// Task::periodic_msg_enabled
// ============================================================================
YAT_INLINE bool Task::periodic_msg_enabled () const
{
  return this->msg_q_.enable_periodic_msg_;
}

// ============================================================================
// Task::set_periodic_msg_period_usec
// ============================================================================
YAT_INLINE void Task::set_periodic_msg_period (double _tmo)
{
  if( _tmo < 0 )
    THROW_YAT_ERROR("BAD_ARG",
                    "periodic msg period can't be negative",
                    "Task::set_periodic_msg_period");
  this->periodic_msg_period_ms_ = _tmo;
}

// ============================================================================
// Task::get_periodic_msg_period
// ============================================================================
YAT_INLINE double Task::get_periodic_msg_period () const
{
  return this->periodic_msg_period_ms_;
}

// ============================================================================
// Task::actual_timeout_msg_period
// ============================================================================
YAT_INLINE double Task::actual_timeout () const
{
  if (this->msg_q_.enable_periodic_msg_ && this->periodic_msg_period_ms_)
    return this->periodic_msg_period_ms_;
  if (this->msg_q_.enable_timeout_msg_ && this->timeout_msg_period_ms_)
    return this->timeout_msg_period_ms_;
  return kDEFAULT_TASK_TMO_MSECS;
}

// ============================================================================
// Task::post
// ============================================================================
YAT_INLINE void Task::post (yat::Message * _msg, size_t _tmo_msecs)
{
  this->msg_q_.post (_msg, _tmo_msecs);
}

// ============================================================================
// Task::msgq_lo_wm
// ============================================================================
YAT_INLINE void Task::msgq_lo_wm (size_t _lo_wm)
{
	this->msg_q_.lo_wm(_lo_wm);
}

// ============================================================================
// Task::msgq_lo_wm
// ============================================================================
YAT_INLINE size_t Task::msgq_lo_wm () const
{
	return this->msg_q_.lo_wm();
}

// ============================================================================
// Task::msgq_hi_wm
// ============================================================================
YAT_INLINE void Task::msgq_hi_wm (size_t _hi_wm)
{
	this->msg_q_.hi_wm(_hi_wm);
}

// ============================================================================
// Task::msgq_hi_wm
// ============================================================================
YAT_INLINE size_t Task::msgq_hi_wm () const
{
	return this->msg_q_.hi_wm();
}

// ============================================================================
// Task::msgq_wm_unit
// ============================================================================
YAT_INLINE void Task::msgq_wm_unit (MessageQ::WmUnit _u)
{
  this->msg_q_.wm_unit(_u);
}

// ============================================================================
// Task::msgq_wm_unit
// ============================================================================
YAT_INLINE MessageQ::WmUnit Task::msgq_wm_unit () const
{
  return this->msg_q_.wm_unit();
}

// ============================================================================
// Task::msgq_statistics
// ============================================================================
YAT_INLINE const MessageQ::Statistics & Task::msgq_statistics ()
{
  return this->msg_q_.statistics();
}

// ============================================================================
// Task::throw_on_post_msg_timeout
// ============================================================================
YAT_INLINE void Task::throw_on_post_msg_timeout (bool _strategy)
{
  this->msg_q_.throw_on_post_msg_timeout(_strategy);
}

} //- namespace
