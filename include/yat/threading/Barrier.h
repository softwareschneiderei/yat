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

#ifndef _YAT_BARRIER_H_
#define _YAT_BARRIER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Mutex.h>
#include <yat/threading/Condition.h>

namespace yat {

// ============================================================================
//! \class Barrier
//! \brief The YAT "Boost type" Barrier class.
//!
//! An object of class barrier is a synchronization primitive used to cause a
//! set of threads to wait until they each perform a certain function or each
//! reach a particular point in their execution.
//!
//! When a Barrier is created, it is initialized with a thread count N. The
//! first N-1 calls to \link wait wait \endlink will all cause their threads
//! to be blocked. The Nth call to \link wait wait \endlink will allow all
//! of the waiting threads to be woken up. A barrier object is not reusable.
//! The Nth call will also mark the barrier object as "expired".
//! The N+1th call to \link wait wait\endlink will throw an exception.
//!
//! \remarks
//! While its destructor is virtual, this class is not supposed to be derived.\n
//! Be sure to clearly understand the internal behaviour before trying to do so.
//!
//! A Barrier object is also not supposed to be destroyed before all wait() calls
//! have returned. Is that case the behavior is undefined.
// ============================================================================
class YAT_DECL Barrier
{
public:
  //! \brief Constructor.
  //!
  //! Constructs a Barrier object that will cause \a count threads to block
  //! on a call to \link wait wait \endlink.
  //!
  //! \param count The number of threads to synchronize.
  Barrier (size_t count);

  //! \brief Destructor.
  //!
  //! If threads are still blocking in a \link wait wait \endlink operation,
  //! the behavior for these threads is undefined.
  virtual ~Barrier ();

  //! \brief Waits until N threads call \link wait wait \endlink, where N equals the
  //! count provided to the constructor for the Barrier object.
  //!
  //! \param tmo_msecs Wait timeout in milliseconds. Wait forever, if set to zero (the default).
  //!
  //! \exception TIMEOUT_EXPIRED Thrown on timeout expiration (in case tmo_msecs not null).
  //!
  //! \remarks
  //! If the barrier is destroyed before wait() can return, the behavior is
  //! undefined.
  void wait (size_t tmo_msecs = 0);

  //! \brief Returns true if the N threads called wait on the barrier, returns false otherwise.
  bool has_expired ();


private:
  //- The number of threads to synchronize (involved threads).
  size_t m_thread_count;

  //- The mutex associated with m_condition.
  Mutex m_mutex;

  //- The condition variable used to synchronized the involved threads.
  Condition m_condition;

  //- The number of threads currently waiting on the Barrier.
  size_t m_waiters_count;

  //- Not implemented private member.
  Barrier (const Barrier&);

  //- Not implemented private member.
  Barrier & operator= (const Barrier&);
};

} // namespace yat

#if defined (YAT_INLINE_IMPL)
# include <yat/threading/Barrier.i>
#endif

#endif //- _YAT_BARRIER_H_
