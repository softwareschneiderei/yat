//----------------------------------------------------------------------------
// Copyright (c) 2004-2014 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2012  N.Leclercq & The Tango Community
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
//      Nicolas Leclercq
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
public:
  PulserCoreImpl (const Pulser::Config& cfg)
    : cfg_(cfg), pulses_(0), job_done_(false)
  {}

  ~PulserCoreImpl ()
  {}

   void job_done ()
   {
     job_done_ = true;
   }
   
protected:
  virtual void handle_message (yat::Message& msg)
  {
    switch ( msg.type() )  
    {
        case yat::TASK_PERIODIC:
        {
          if ( ! this->job_done_ && ! this->cfg_.callback.is_empty () )
          {
            try
            {
              this->cfg_.callback(this->cfg_.user_data);
            }
            catch ( ... ) {}
          }
          if ( this->job_done_ || (this->cfg_.num_pulses && ( ++this->pulses_ >= this->cfg_.num_pulses )) )
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
  bool job_done_;
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
  {
    this->resume();
    return;
  }

  this->impl_ = new PulserCoreImpl(this->cfg_);
  this->impl_->enable_periodic_msg(true);
  this->impl_->set_periodic_msg_period(this->cfg_.period_in_msecs);
  this->impl_->go(); 
}

// ============================================================================
// Pulser::stop
// ============================================================================
void Pulser::stop () 
{
  YAT_TRACE("Pulser::stop");
  
  this->impl_->job_done();
}

// ============================================================================
// Pulser::set_period
// ============================================================================
void Pulser::set_period (size_t p_msecs)
{
  YAT_TRACE("Pulser::set_period");

  if ( this->impl_ )
    this->impl_->set_periodic_msg_period(p_msecs);
}
  
// ============================================================================
// Pulser::get_period
// ============================================================================
size_t Pulser::get_period () const
{
  YAT_TRACE("Pulser::get_period");

  return this->impl_ ? this->impl_->get_periodic_msg_period() : 0;
}

// ============================================================================
// Pulser::suspend
// ============================================================================
void Pulser::suspend () 
{
  YAT_TRACE("Pulser::suspend");
  
  if ( this->impl_ )
    this->impl_->enable_periodic_msg(false);
}

// ============================================================================
// Pulser::resume
// ============================================================================
void Pulser::resume () 
{
  YAT_TRACE("Pulser::resume");
  
  if ( this->impl_ )
    this->impl_->enable_periodic_msg(true);
}

} // namespace
