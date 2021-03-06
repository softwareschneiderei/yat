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

// ****************************************************************************
// YAT NULL MUTEX IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// NullMutex::lock
// ----------------------------------------------------------------------------
YAT_INLINE void NullMutex::lock ()
{
 //- noop
}

// ----------------------------------------------------------------------------
// NullMutex::acquire
// ----------------------------------------------------------------------------
YAT_INLINE void NullMutex::acquire ()
{
 //- noop
}

// ----------------------------------------------------------------------------
// NullMutex::unlock
// ----------------------------------------------------------------------------
YAT_INLINE void NullMutex::unlock ()
{
 //- noop
}

// ----------------------------------------------------------------------------
// NullMutex::release
// ----------------------------------------------------------------------------
YAT_INLINE void NullMutex::release ()
{
 //- noop
}

// ----------------------------------------------------------------------------
// NullMutex::try_lock
// ----------------------------------------------------------------------------
YAT_INLINE MutexState NullMutex::try_lock ()
{
  return yat::MUTEX_LOCKED;
}

// ----------------------------------------------------------------------------
// NullMutex::try_acquire
// ----------------------------------------------------------------------------
YAT_INLINE MutexState NullMutex::try_acquire ()
{
  return yat::MUTEX_LOCKED;
}

// ----------------------------------------------------------------------------
// NullMutex::timed_try_lock
// ----------------------------------------------------------------------------
YAT_INLINE MutexState NullMutex::timed_try_lock (unsigned long)
{
  return yat::MUTEX_LOCKED;
}

// ----------------------------------------------------------------------------
// NullMutex::timed_try_acquire
// ----------------------------------------------------------------------------
YAT_INLINE MutexState NullMutex::timed_try_acquire (unsigned long)
{
  return yat::MUTEX_LOCKED;
}

// ****************************************************************************
// YAT MUTEX IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// Mutex::lock
// ----------------------------------------------------------------------------
YAT_INLINE void Mutex::lock ()
{
  ::pthread_mutex_lock(&m_posix_mux);
}
// ----------------------------------------------------------------------------
// Mutex::acquire
// ----------------------------------------------------------------------------
YAT_INLINE void Mutex::acquire ()
{
  this->lock();
}

// ----------------------------------------------------------------------------
// Mutex::try_lock
// ----------------------------------------------------------------------------
YAT_INLINE MutexState Mutex::try_lock ()
{
  MutexState result = MUTEX_LOCKED;

  if (::pthread_mutex_trylock (&m_posix_mux) != 0)
    result = MUTEX_BUSY;

  return result;
}

// ----------------------------------------------------------------------------
// Mutex::try_acquire
// ----------------------------------------------------------------------------
YAT_INLINE MutexState Mutex::try_acquire ()
{
  return this->try_lock();
}

// ----------------------------------------------------------------------------
// Mutex::timed_try_acquire
// ----------------------------------------------------------------------------
YAT_INLINE MutexState Mutex::timed_try_acquire (unsigned long tmo_msecs)
{
  return this->timed_try_lock(tmo_msecs);
}

// ----------------------------------------------------------------------------
// Mutex::unlock
// ----------------------------------------------------------------------------
YAT_INLINE void Mutex::unlock ()
{
  ::pthread_mutex_unlock(&m_posix_mux);
}

// ----------------------------------------------------------------------------
// Mutex::acquire
// ----------------------------------------------------------------------------
YAT_INLINE void Mutex::release ()
{
  this->unlock();
}

} // namespace yat
