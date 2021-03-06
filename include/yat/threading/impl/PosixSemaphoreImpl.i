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

namespace yat {

// ----------------------------------------------------------------------------
// Semaphore::wait
// ----------------------------------------------------------------------------
YAT_INLINE void Semaphore::wait ()
{
  this->timed_wait(0);
}

// ----------------------------------------------------------------------------
// Semaphore::timed_wait
// ----------------------------------------------------------------------------
YAT_INLINE bool Semaphore::timed_wait (unsigned long _tmo_msecs)
{
  bool r = true;
  this->m_mux.lock();
  if (! this->m_value)
  {
    r = this->m_cond.timed_wait(_tmo_msecs);
  }
  if (r)
  {
    this->m_value--;
  }
  this->m_mux.unlock();
  return r;
}

// ----------------------------------------------------------------------------
// Semaphore::try_wait
// ----------------------------------------------------------------------------
YAT_INLINE SemaphoreState Semaphore::try_wait ()
{
  SemaphoreState s;
  this->m_mux.lock();
  if (this->m_value > 0)
  {
    this->m_value--;
    s = SEMAPHORE_DEC;
 	}
	else
	{
	  s = SEMAPHORE_NO_RSC;
	}
  this->m_mux.unlock();
  return s;
}

// ----------------------------------------------------------------------------
// Semaphore::post
// ----------------------------------------------------------------------------
YAT_INLINE void Semaphore::post ()
{
  this->m_mux.lock();
  this->m_value++;
  this->m_cond.signal();
  this->m_mux.unlock();
}

} // namespace yat
