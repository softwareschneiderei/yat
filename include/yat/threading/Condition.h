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

#ifndef _YAT_CONDITION_H_
#define _YAT_CONDITION_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Implementation.h>

// ----------------------------------------------------------------------------
// Implementation-specific header file.
// ----------------------------------------------------------------------------
#if ! defined(YAT_CONDITION_IMPLEMENTATION)
# error "implementation header file incomplete [no condition implementation]"
#endif

namespace yat {

// ----------------------------------------------------------------------------
// FORWARD DECL
// ----------------------------------------------------------------------------
class Mutex;

// ============================================================================
//! \class Condition
//! \brief The YAT Condition variable class
//!
//! The Windows implementation is based on D.C.Schmidt & Al solution describes
//! in the following article: http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
//!
//! Under Linux (and any other \c POSIX platforms), the code relies on the local
//! \c pthread implementation.
//!
//! \remarks
//! While its destructor is virtual, this class is not supposed to be derived.\n
//! Be sure to clearly understand the internal behaviour before trying to do so.
// ============================================================================
class YAT_DECL Condition
{
public:
  //! \brief Constructor.
  //!
  //! Each condition must be associated to a mutex (use in preference Automutex) that must be hold while
  //! evaluating the condition. It means that \a external_mutex must be locked
  //! prior to any call to the Condition interface. See \link
  //! http://www.cs.wustl.edu/~schmidt/win32-cv-1.html D.C.Schmidt and I.Pyarali
  //! \endlink article for details.
  //! \param external_mutex Associated mutex to lock/unlock while using the condition
  Condition (yat::Mutex& external_mutex);

  //! \brief Destructor.
  //!
  //! While this destructor is virtual, this class is not supposed to be derived.
  //! Be sure to understand the internal behaviour before trying to do so.
  virtual ~Condition ();

  //! \brief Wait until the condition is either \link Condition::signal signaled\endlink
  //! or \link Condition::broadcast broadcast\endlink by another thread.
  //!
  //! The associated \a external_mutex <b>must be locked</b> by the calling thread.
  //! \remark The associated external_mutex is unlocked by the wait() function.
  void wait ();

  //! \brief Wait for the condition to be \link Condition::signal signaled\endlink
  //! or \link Condition::broadcast broadcast\endlink by another thread.
  //!
  //! Returns \c false in case the specified timeout expired before the condition
  //! was notified. Returns \c true otherwise.
  //!
  //! The associated \a external_mutex <b>must be locked</b> by the calling thread.
  //!
  //! \param tmo_msecs The timeout in milliseconds.
  //! \return \c false [timeout expired] or \c true [condition notified]
  //! \remark The associated external_mutex is unlocked by the timed_wait() function.
  bool timed_wait (unsigned long tmo_msecs);

  //! \brief Wait for the condition to be \link Condition::signal signaled\endlink
  //! or \link Condition::broadcast broadcast\endlink by another thread.
  //!
  //! Returns \c false in case the specified timeout expired before the condition
  //! was notified. Returns \c true otherwise.
  //!
  //! The associated \a external_mutex <b>must be locked</b> by the calling thread.
  //!
  //! \param tmo_secs The seconds part of the timeout
  //! \param tmo_nsecs The nanoseconds part of the timeout
  //! \return \c false [timeout expired] or \c true [condition notified]
  //! \remark The associated external_mutex is unlocked by the timed_wait() function.
  bool timed_wait (unsigned long tmo_secs, unsigned long tmo_nsecs);

  //! \brief Signals the condition by notifying \b one of the waiting threads.
  //!
  //! The associated \a external_mutex <b>must be locked</b> by the calling thread.
  void signal ();

  //! \brief Broadcasts the condition by notifying \b all waiting threads.
  //!
  //! The associated \a external_mutex <b>must be locked</b> by the calling thread.
  void broadcast ();

private:
  //- The so called "external mutex" (see D.Schmidt's article).
  Mutex & m_external_lock;

  //- Not implemented private member.
  Condition (const Condition&);

  //- Not implemented private member.
  Condition & operator= (const Condition&);

  //- hidden/abstract platform specific implementation.
  YAT_CONDITION_IMPLEMENTATION;
};

} // namespace yat

#if defined (YAT_INLINE_IMPL)
# if defined (YAT_WIN32)
#  include <yat/threading/impl/WinNtConditionImpl.i>
# else
#  include <yat/threading/impl/PosixConditionImpl.i>
# endif
#endif

#endif //- _YAT_CONDITION_H_
