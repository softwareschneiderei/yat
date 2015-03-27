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
// Copyright (C) 2006-2014 The Tango Community
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
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */
// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Barrier.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/threading/Barrier.i>
#endif // YAT_INLINE_IMPL


namespace yat {

// ----------------------------------------------------------------------------
// Barrier::Barrier
// ----------------------------------------------------------------------------
Barrier::Barrier (size_t _count)
 : m_thread_count (_count),
   m_condition (m_mutex),
   m_waiters_count (0)
{
  YAT_TRACE("Barrier::Barrier");

  YAT_LOG("Barrier::Barrier:: " << m_thread_count << " threads involved");
}

// ----------------------------------------------------------------------------
// Barrier::~Barrier
// ----------------------------------------------------------------------------
Barrier::~Barrier ()
{
  YAT_TRACE("Barrier::Barrier");
}

// ----------------------------------------------------------------------------
// Barrier::wait
// ----------------------------------------------------------------------------
void Barrier::wait (size_t tmo_msecs)
{
  //- enter critical section
  MutexLock guard(this->m_mutex);

  //- are all expected threads waiting on the barrier?
  if ( this->m_waiters_count == this->m_thread_count )
  {
    THROW_YAT_ERROR("PROGRAMMING_ERROR",
                    "barrier has expired! [hint: did a thread called 'wait' twice of the same barrier?]",
                    "Barrier::wait");
  }
  
  //- increment waiters count
  this->m_waiters_count++;

  YAT_LOG("Barrier::wait::thread " << DUMP_THREAD_UID << "::about to wait on Barrier");

  //- are all expected threads waiting on the barrier?
  if ( this->m_waiters_count == this->m_thread_count )
  {
    YAT_LOG("Barrier::wait::all expected waiters present. Reset/notify Barrier...");
    //- notify all waiters
    this->m_condition.broadcast();
    //- done: return 
    return;
  }

  //- make the calling thread wait
  if ( tmo_msecs )
  {
    if ( ! this->m_condition.timed_wait(tmo_msecs) )
    {
      THROW_YAT_ERROR("TIMEOUT_EXPIRED",
                      "barrier timeout expired!",
                      "Barrier::wait");
    }
  }
  else
  {
    this->m_condition.wait();
  }
  
  YAT_LOG("Barrier::wait::thread " << DUMP_THREAD_UID << "::woken up");
}

} // namespace yat
