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
//      Stephane Poirier
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Pulser.h>

namespace yat
{

// ======================================================================
// Pulser's underlying implementation
// ======================================================================
class PulserCoreImpl : public yat::Task
{
  typedef enum
  {
    START_MSG = yat::FIRST_USER_MSG,
    STOP_MSG,
    SUSPEND_MSG,
    RESUME_MSG,
    NUM_PULSES_CHANGED
  } PulserCoreImplMsgType;

public:
  PulserCoreImpl (const Pulser::Config& cfg)
    : cfg_(cfg), pulses_(0)
  {}

  ~PulserCoreImpl ()
  {}

  void start (bool sync)
  {
    if( sync )
      this->wait_msg_handled(START_MSG);
    else
      this->post(START_MSG);
  }

  void stop (bool sync)
  {
    if( sync )
      this->wait_msg_handled(STOP_MSG);
    else
      this->post(STOP_MSG);
  }

  void suspend (bool sync)
  {
    if( sync )
      this->wait_msg_handled(SUSPEND_MSG);
    else
      this->post(SUSPEND_MSG);
  }

  void resume (bool sync)
  {
    if( sync )
      this->wait_msg_handled(RESUME_MSG);
    else
      this->post(RESUME_MSG);
  }

  size_t get_num_pulses () const {
    return this->cfg_.num_pulses;
  }

  bool is_running() const {
    return this->periodic_msg_enabled();
  }

  bool is_done() const {
    return this->pulses_ == this->cfg_.num_pulses;
  }

  void set_num_pulses (size_t num_pulses) {
    this->wait_msg_handled(NUM_PULSES_CHANGED, num_pulses, 1000);
  }

protected:
  virtual void handle_message (yat::Message& msg)
  {
    switch ( msg.type() )
    {
      case START_MSG:
        {
          this->pulses_ = 0;
          this->set_periodic_msg_period(this->cfg_.period_in_msecs);
          this->enable_periodic_msg(true);
          this->enable_precise_periodic_timing(true);
        }
        break;
      case RESUME_MSG:
        {
          if( this->pulses_ < this->cfg_.num_pulses )
            this->enable_periodic_msg(true);
        }
        break;
      case STOP_MSG:
      case SUSPEND_MSG:
        {
          this->enable_periodic_msg(false);
        }
        break;
      case NUM_PULSES_CHANGED:
        {
          this->cfg_.num_pulses = msg.get_data<size_t>();
        }
        break;
      case yat::TASK_PERIODIC:
        {
          if ( this->cfg_.callback.is_empty() )
          {
            this->enable_periodic_msg(false);
            return;
          }
          try
          {
            this->cfg_.callback(this->cfg_.user_data);
          }
          catch ( ... )
          {
            //- noop
          }
          if ( this->cfg_.num_pulses && ( ++this->pulses_ == this->cfg_.num_pulses ) )
          {
            this->enable_periodic_msg(false);
          }
        }
        break;
      default:
        break;
    }
  }

private:
  Pulser::Config cfg_;
  size_t pulses_;
};

// ======================================================================
// Pulser::Config::Config
// ======================================================================
Pulser::Config::Config ()
  : period_in_msecs(1000),
    num_pulses(0),
    user_data(0)
{
  /* noop ctor */
}

// ======================================================================
// Pulser::Pulser
// ======================================================================
Pulser::Pulser (const Pulser::Config& cfg)
  : cfg_(cfg), impl_(0)
{
  YAT_TRACE("Pulser::Pulser");

  //- check configuration
  if( cfg_.period_in_msecs <= 0 )
    THROW_YAT_ERROR("BAD_ARG",
                    "pulser period can't be zero or negative",
                    "Pulser::Pulser");

  if( cfg_.num_pulses > ((std::size_t)(-1) >> 1) )
    THROW_YAT_ERROR("BAD_ARG",
                    "pulses number too high (may be a negative value is passed)",
                    "Pulser::Pulser");

  this->impl_ = new PulserCoreImpl(this->cfg_);
  this->impl_->go();
}

// ======================================================================
// Pulser::~Pulser
// ======================================================================
Pulser::~Pulser ()
{
  try
  {
    if ( this->impl_ )
      this->impl_->exit();
  }
  catch ( ... ) {}
}

// ============================================================================
// Pulser::start
// ============================================================================
void Pulser::start ()
{
  YAT_TRACE("Pulser::start");

  if ( this->impl_ )
    this->impl_->start(false);
}

// ============================================================================
// Pulser::start_sync
// ============================================================================
void Pulser::start_sync ()
{
  YAT_TRACE("Pulser::start_sync");

  if ( this->impl_ )
    this->impl_->start(true);
}

// ============================================================================
// Pulser::stop
// ============================================================================
void Pulser::stop ()
{
  YAT_TRACE("Pulser::stop");

  if ( this->impl_ )
    this->impl_->stop(false);
}

// ============================================================================
// Pulser::stop_sync
// ============================================================================
void Pulser::stop_sync ()
{
  YAT_TRACE("Pulser::stop_sync");

  if ( this->impl_ )
    this->impl_->stop(true);
}

// ============================================================================
// Pulser::set_period
// ============================================================================
void Pulser::set_period (double p_msecs)
{
  YAT_TRACE("Pulser::set_period");

  cfg_.period_in_msecs = p_msecs;

  if ( this->impl_ )
    this->impl_->set_periodic_msg_period(p_msecs);
}

// ============================================================================
// Pulser::get_period
// ============================================================================
double Pulser::get_period () const
{
  YAT_TRACE("Pulser::get_period");

  return is_running() ? this->impl_->get_periodic_msg_period() : cfg_.period_in_msecs;
}

// ============================================================================
// Pulser::set_num_pulses
// ============================================================================
void Pulser::set_num_pulses (size_t num_pulses)
{
  YAT_TRACE("Pulser::set_num_pulses");

  cfg_.num_pulses = num_pulses;

  if ( this->impl_ )
    this->impl_->set_num_pulses(num_pulses);
}

// ============================================================================
// Pulser::get_num_pulses
// ============================================================================
size_t Pulser::get_num_pulses () const
{
  YAT_TRACE("Pulser::get_num_pulses");

  return is_running() ? this->impl_->get_num_pulses() : cfg_.num_pulses;
}

// ============================================================================
// Pulser::suspend
// ============================================================================
void Pulser::suspend ()
{
  YAT_TRACE("Pulser::suspend");

  if ( this->impl_ )
    this->impl_->suspend(false);
}

// ============================================================================
// Pulser::suspend_sync
// ============================================================================
void Pulser::suspend_sync ()
{
  YAT_TRACE("Pulser::suspend_sync");

  if ( this->impl_ )
    this->impl_->suspend(true);
}

// ============================================================================
// Pulser::resume
// ============================================================================
void Pulser::resume ()
{
  YAT_TRACE("Pulser::resume");

  if ( this->impl_ )
    this->impl_->resume(false);
}

// ============================================================================
// Pulser::resume_sync
// ============================================================================
void Pulser::resume_sync ()
{
  YAT_TRACE("Pulser::resume_sync");

  if ( this->impl_ )
    this->impl_->resume(true);
}

// ============================================================================
// Pulser::is_done
// ============================================================================
bool Pulser::is_done () const
{
  YAT_TRACE("Pulser::is_done");

  if ( this->impl_ )
    return this->impl_->is_done();

  // code never reached!
  return false;
}

// ============================================================================
// Pulser::is_running
// ============================================================================
bool Pulser::is_running () const
{
  YAT_TRACE("Pulser::is_running");

  if ( this->impl_ )
    return this->impl_->is_running();

  // code never reached!
  return false;
}

} // namespace
