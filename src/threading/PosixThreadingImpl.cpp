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
// Copyright (C) 2006-2021  N.Leclercq & The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
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

// ----------------------------------------------------------------------------
// DEPENDENCIES
// ----------------------------------------------------------------------------
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>
#include <yat/threading/Utilities.h>
#include <yat/threading/Mutex.h>
#include <yat/threading/Condition.h>
#include <yat/threading/Semaphore.h>
#include <yat/threading/Thread.h>
#include <yat/utils/String.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/threading/impl/PosixMutexImpl.i>
# include <yat/threading/impl/PosixConditionImpl.i>
# include <yat/threading/impl/PosixSemaphoreImpl.i>
# include <yat/threading/impl/PosixThreadImpl.i>
#endif

// ----------------------------------------------------------------------------
// SOME PSEUDO CONSTs
// ----------------------------------------------------------------------------
#define MAX_SLEEP_SECONDS  (long)4294966  //- this is (2^32 - 2) / 1000
#define MAX_NSECS 1000000000

// ----------------------------------------------------------------------------
// PLATFORM SPECIFIC THREAD PRIORITIES
// ----------------------------------------------------------------------------
#if defined(PthreadSupportThreadPriority)
 static int lowest_priority;
 static int normal_priority;
 static int highest_priority;
#endif

namespace yat {

// ****************************************************************************
// YAT DUMMY_MUTEX IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// NullMutex::NullMutex
// ----------------------------------------------------------------------------
NullMutex::NullMutex ()
{
 //- noop
}
// ----------------------------------------------------------------------------
// NullMutex::~NullMutex
// ----------------------------------------------------------------------------
NullMutex::~NullMutex ()
{
 //- noop
}

// ****************************************************************************
// YAT MUTEX IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// Mutex::Mutex
// ----------------------------------------------------------------------------
Mutex::Mutex ()
  : m_posix_mux ()
{
  YAT_TRACE("Mutex::Mutex");

#if (PthreadDraftVersion == 4)
# error pthread draft version 4 not supported - add recursive mutex support
  ::pthread_mutex_init(&m_posix_mux, pthread_mutexattr_default);
#else
  pthread_mutexattr_t ma;
  ::pthread_mutexattr_init(&ma);
  ::pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE);
  ::pthread_mutex_init(&m_posix_mux, &ma);
  ::pthread_mutexattr_destroy(&ma);
#endif
}

// ----------------------------------------------------------------------------
// Mutex::~Mutex
// ----------------------------------------------------------------------------
Mutex::~Mutex()
{
  YAT_TRACE("Mutex::~Mutex");

  ::pthread_mutex_destroy(&m_posix_mux);
}

// ----------------------------------------------------------------------------
// Mutex::timed_try_lock
// ----------------------------------------------------------------------------
MutexState Mutex::timed_try_lock (unsigned long tmo_msecs)
{
  MutexState ms = yat::MUTEX_BUSY;

  yat::Timeout tmo(tmo_msecs, yat::Timeout::TMO_UNIT_MSEC, true);

  while ( ! tmo.expired() )
  {
    ms = this->try_lock();
    if (ms == yat::MUTEX_LOCKED)
      break;
    yat::Thread::yield();
  }

  return ms;
}

// ****************************************************************************
// YAT SEMAPHORE IMPL
// ****************************************************************************
#define SEMAPHORE_MAX_COUNT ((unsigned int)(-1) >> 1)

// ----------------------------------------------------------------------------
// Semaphore::Semaphore
// ----------------------------------------------------------------------------
Semaphore::Semaphore (unsigned int _initial_value)
  : m_mux () , m_cond (m_mux), m_value (_initial_value)
{
  if( _initial_value > SEMAPHORE_MAX_COUNT )
  {
    throw Exception("BAD_VALUE",
            yat::Format("initial_value ({}) too high for Semaphore object."
                        " max: {}").arg(_initial_value)
                                   .arg(SEMAPHORE_MAX_COUNT),
                        "Semaphore::Semaphore");
  }
  YAT_TRACE("Semaphore::Semaphore");
}

// ----------------------------------------------------------------------------
// Semaphore::~Semaphore
// ----------------------------------------------------------------------------
Semaphore::~Semaphore()
{
  YAT_TRACE("Semaphore::~Semaphore");
}

// ****************************************************************************
// YAT CONDITION IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// Condition::Condition
// ----------------------------------------------------------------------------
Condition::Condition (Mutex & external_lock)
 : m_external_lock (external_lock),
   m_posix_cond ()
{
  YAT_TRACE("Condition::Condition");

#if (PthreadDraftVersion == 4)
  ::pthread_cond_init(&m_posix_cond, pthread_condattr_default);
#else
  ::pthread_cond_init(&m_posix_cond, 0);
#endif
}

// ----------------------------------------------------------------------------
// Condition::~Condition
// ----------------------------------------------------------------------------
Condition::~Condition ()
{
  YAT_TRACE("Condition::Condition");

  ::pthread_cond_destroy(&m_posix_cond);
}

// ----------------------------------------------------------------------------
// Condition::timed_wait
// ----------------------------------------------------------------------------
bool Condition::timed_wait (unsigned long _tmo_msecs)
{
  //- null tmo means infinite wait

  bool signaled = true;

  if (_tmo_msecs == 0)
  {
    ::pthread_cond_wait(&m_posix_cond, &m_external_lock.m_posix_mux);
  }
  else
  {
    //- get absoulte time
    struct timespec ts;
    ThreadingUtilities::get_time(ts, _tmo_msecs);
    //- wait for the condition to be signaled or tmo expiration
    int result = ::pthread_cond_timedwait(&m_posix_cond,
                                          &m_external_lock.m_posix_mux,
                                          &ts);
    if (result == ETIMEDOUT)
      signaled = false;
  }

  return signaled;
}

// ----------------------------------------------------------------------------
// Condition::timed_wait
// ----------------------------------------------------------------------------
bool Condition::timed_wait (unsigned long _tmo_secs, unsigned long _tmo_nsecs)
{
  //- null tmo means infinite wait

  bool signaled = true;

  if (_tmo_secs == 0 && _tmo_nsecs == 0)
  {
    ::pthread_cond_wait(&m_posix_cond, &m_external_lock.m_posix_mux);
  }
  else
  {
    //- get absoulte time
    struct timespec ts;
    ThreadingUtilities::get_time(ts, _tmo_secs, _tmo_nsecs);
    //- wait for the condition to be signaled or tmo expiration
    int result = ::pthread_cond_timedwait(&m_posix_cond,
                                          &m_external_lock.m_posix_mux,
                                          &ts);
    if (result == ETIMEDOUT)
      signaled = false;
  }

  return signaled;
}

// ****************************************************************************
// YAT THREAD IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// NanoSleep support
// ----------------------------------------------------------------------------
#if ! defined(NoNanoSleep)
# define YAT_HAS_NANOSLEEP
#endif

// ----------------------------------------------------------------------------
// YAT common thread entry point (non-OO OS intertace to OO YAT interface)
// ----------------------------------------------------------------------------
Thread::IOArg yat_thread_common_entry_point (Thread::IOArg _p)
{
  YAT_TRACE_STATIC("yat_thread_common_entry_point");

  //- check input (parano. impl.)
  if (! _p) return 0;

  //- reinterpret input
  Thread * me = reinterpret_cast<Thread*>(_p);

  //- store the thread identifier
  me->m_uid = ThreadingUtilities::self();

  YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " is starting up");

  //- select detached or undetached mode
  if (me->m_detached)
  {
    YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " will run detached");
    //- just protect yat impl. against user code using a try/catch statement
    try
    {
      me->run(me->m_iarg);
    }
    catch (...)
    {
      //- ignore any exception
    }
  }
  else
  {
    YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " will run undetached");
    //- just protect yat impl. against user code using a try/catch statement
    try
    {
      me->m_oarg = me->run_undetached(me->m_iarg);
    }
    catch (...)
    {
      //- ignore any exception

    }
  }

  YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " has leaved its main loop");

  //- set state to terminated
  {
    //- must lock the mutex even in the case of a detached thread. This is because
    //- a thread may run to completion before the thread that created it has had a
    //- chance to get out of start().  By locking the mutex we ensure that the
    //- creating thread must have reached the end of start() before we delete the
    //- thread object.  Of course, once the call to start() returns, the user can
    //- still incorrectly refer to the thread object, but that's their problem.
    AutoMutex<Mutex> guard(me->m_lock);
    YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " - changing state to TERMINATED");
    //- set state to TERMINATED
    me->m_state = yat::Thread::STATE_TERMINATED;
  }

  //- commit suicide in case the thread ran detached
  if (me->m_detached)
  {
    YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " is detached and will be deleted now.");
    delete me;
  }

  YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " - leaving the place...");

  return 0;
}

// ----------------------------------------------------------------------------
// Thread::Thread
// ----------------------------------------------------------------------------
Thread::Thread (Thread::IOArg _iarg, Thread::Priority _p)
 : //- platform independent members
   m_state (yat::Thread::STATE_NEW),
   m_priority (_p),
   m_iarg (_iarg),
   m_oarg (0),
   m_detached (true),
   m_uid (YAT_INVALID_THREAD_UID),
   //- platform specific members
   m_posix_thread (0)
{
  YAT_TRACE("Thread::Thread");

#if defined(PthreadSupportThreadPriority)
 static bool init_done = false;
 if (! init_done)
 {
   lowest_priority = ::sched_get_priority_min(SCHED_FIFO);
   highest_priority = ::sched_get_priority_max(SCHED_FIFO);
   switch (highest_priority - lowest_priority)
   {
     case 0:
     case 1:
       normal_priority = lowest_priority;
       break;
     default:
       normal_priority = lowest_priority + 1;
       break;
   }
   init_done = true;
 }
#endif
}

// ----------------------------------------------------------------------------
// Thread::~Thread
// ----------------------------------------------------------------------------
Thread::~Thread ()
{
  YAT_TRACE("Thread::~Thread");

  YAT_LOG("Thread::~Thread::deleting thread " << std::hex << this->m_uid << std::dec);
}

// ----------------------------------------------------------------------------
// Thread::start [detatched thread]
// ----------------------------------------------------------------------------
void Thread::start ()
{
  YAT_TRACE("Thread::start");
  //- mark the thread as detached
  this->m_detached = true;
  //- then spawn it
  this->spawn();
}

// ----------------------------------------------------------------------------
// Thread::start_undetached [undetatched thread]
// ----------------------------------------------------------------------------
void Thread::start_undetached ()
{
  YAT_TRACE("Thread::start_undetached");
  //- mark the thread as undetached
  this->m_detached = false;
  //- then spawn it
  this->spawn();
}

// ----------------------------------------------------------------------------
// Thread::spawn (common to detatched & undetached threads)
// ----------------------------------------------------------------------------
void Thread::spawn ()
{
  YAT_TRACE("Thread::spawn");

  //- enter critical section
  AutoMutex<Mutex> guard(this->m_lock);

  //- be sure the thread is not already running or terminated
  if (this->m_state != yat::Thread::STATE_NEW)
  {
    YAT_LOG("Thread::spawn::thread is either already running or terminated");
    return;
  }

  //- intialize thread attributes
  pthread_attr_t thread_attrs;

#if (PthreadDraftVersion == 4)
  ::pthread_attr_create(&thread_attrs);
#else
  ::pthread_attr_init(&thread_attrs);
#endif

  //- set detach attribute
  YAT_LOG("Thread::spawn::changing thread detach-state attr");
  int ds = this->m_detached
         ? PTHREAD_CREATE_DETACHED
         : PTHREAD_CREATE_JOINABLE;
  ::pthread_attr_setdetachstate(&thread_attrs, ds);

  //- set thread priority
#if defined(PthreadSupportThreadPriority)
  YAT_LOG("Thread::spawn::changing thread priority attr");
# if (PthreadDraftVersion <= 6)
  ::pthread_attr_setprio(&thread_attrs, yat_to_posix_priority(m_priority));
# else
  struct sched_param sp;
  sp.sched_priority = yat_to_posix_priority(m_priority);
  ::pthread_attr_setschedparam(&thread_attrs, &sp));
# endif
#endif

  //- spawn the thread
  YAT_LOG("Thread::spawn::spawing thread");
  int result = 0;
#if (PthreadDraftVersion == 4)
  result = ::pthread_create(&m_posix_thread,
                            thread_attrs,
                            yat_thread_common_entry_point,
                            static_cast<void*>(this));
  ::pthread_attr_delete(&thread_attrs);
#else
  result = ::pthread_create(&m_posix_thread,
                            &thread_attrs,
                            yat_thread_common_entry_point,
                            static_cast<void*>(this));
  ::pthread_attr_destroy(&thread_attrs);
#endif
  //- check result
  if (result)
    throw Exception(); //-TODO

  //- mark the thread as running (before leaving the critical section)
  this->m_state = yat::Thread::STATE_RUNNING;
}

// ----------------------------------------------------------------------------
// Thread::join
// ----------------------------------------------------------------------------
void Thread::join (Thread::IOArg * oarg_)
{
  YAT_TRACE("Thread::join");

  {
    //- enter critical section
    AutoMutex<Mutex> guard(this->m_lock);
    //- check thread state
    if (
           (this->m_state != yat::Thread::STATE_RUNNING)
        &&
           (this->m_state != yat::Thread::STATE_TERMINATED)
       )
       {
         if (oarg_) *oarg_ = 0;
         delete this;
         return;
       }
  }

  //- be sure the thread is not detached
  if (this->m_detached)
    throw Exception(); //-TODO

  YAT_LOG("Thread::join::waiting for the thread to terminate [pthread_join]");

  if (::pthread_join(m_posix_thread, 0))
    throw Exception(); //-TODO

#if (PthreadDraftVersion == 4)
  //- with draft 4 pthreads implementations we have to detach the thread
  //- after join. if not, the storage for the thread will not be reclaimed.
  ::pthread_detach(&m_posix_thread);
#endif

  YAT_LOG("Thread::join::thread exit successfully");

  //- return the "thread result"
  if (oarg_)
    *oarg_ = this->m_oarg;

  //- commit suicide
  delete this;
}

// ----------------------------------------------------------------------------
// Thread::priority
// ----------------------------------------------------------------------------
void Thread::priority (Priority _p)
{
  YAT_TRACE("Thread::priority");

  //- enter critical section
  AutoMutex<Mutex> guard(this->m_lock);

  //- check thread state
  if (this->m_state != yat::Thread::STATE_RUNNING)
    throw Exception(); //-TODO

#if defined(PthreadSupportThreadPriority)
# if (PthreadDraftVersion == 4)
    ::pthread_setprio(posix_thread, this->yat_to_posix_priority(pri)));
# elif (PthreadDraftVersion == 6)
    pthread_attr_t attr;
    ::pthread_attr_init(&attr);
    ::pthread_attr_setprio(&attr, this->yat_to_posix_priority(pri)));
    ::pthread_setschedattr(m_posix_thread, attr));
# else
    struct sched_param sp;
    sp.sched_priority = this->yat_to_posix_priority(_p);
    if (::pthread_setschedparam(m_posix_thread, SCHED_OTHER, &sp))
      throw Exception(); //-TODO
# endif
#endif

  //- store new priority
  this->m_priority = _p;
}

// ----------------------------------------------------------------------------
// Thread::yield
// ----------------------------------------------------------------------------
void Thread::yield ()
{
#if YAT_HAS_PTHREAD_YIELD == 1
# if (PthreadDraftVersion == 6)
  ::pthread_yield(NULL);
# elif (PthreadDraftVersion < 9)
  ::pthread_yield();
# endif
#else
  ::sched_yield();
#endif
}

// ----------------------------------------------------------------------------
// Thread::yat_to_posix_priority
// ----------------------------------------------------------------------------
int Thread::yat_to_posix_priority (Priority _p)
{
#if defined(PthreadSupportThreadPriority)
  switch (_p)
  {
    case yat::Thread::PRIORITY_LOW:
      return lowest_priority;

    case yat::Thread::PRIORITY_HIGH:
      return highest_priority;

    case yat::Thread::PRIORITY_RT:
      return highest_priority;

    default:
      return normal_priority;
  }
#else
  switch (_p)
  {
    default:
      throw Exception(); //-TODO
  }
#endif
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::self
// ----------------------------------------------------------------------------
ThreadUID ThreadingUtilities::self ()
{
  return static_cast<yat::ThreadUID>(::pthread_self());
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::sleep
// ----------------------------------------------------------------------------
void ThreadingUtilities::sleep (long _secs, long _nano_secs)
{
#if defined(YAT_HAS_NANOSLEEP)

  timespec ts2;
  timespec ts1 = {_secs, _nano_secs};

  while (::nanosleep(&ts1, &ts2))
  {
    if (errno == EINTR)
    {
      ts1.tv_sec  = ts2.tv_sec;
      ts1.tv_nsec = ts2.tv_nsec;
      continue;
    }
  }

#else

  if (_secs > 2000)
    while ((_secs = ::sleep(_secs))) ;
  else
     ::usleep(_secs * 1000000 + (_nano_secs / 1000));

#endif
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::hardware_concurrency
// ----------------------------------------------------------------------------
unsigned int ThreadingUtilities::harware_concurrency()
{
  return ::sysconf(_SC_NPROCESSORS_ONLN);
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::get_time
// ----------------------------------------------------------------------------
void ThreadingUtilities::get_time (unsigned long & abs_sec_,
                                   unsigned long & abs_nano_sec_,
                                   unsigned long _rel_sec,
                                   unsigned long _rel_nano_sec)
{
  timespec abs;

  struct timeval tv;
  ::gettimeofday(&tv, NULL);

  abs.tv_sec = tv.tv_sec;
  abs.tv_nsec = tv.tv_usec * 1000;

  abs.tv_sec += _rel_sec + abs.tv_nsec / MAX_NSECS;
  abs.tv_nsec += _rel_nano_sec;
  abs.tv_nsec %= MAX_NSECS;

  abs_sec_ = abs.tv_sec;
  abs_nano_sec_ = abs.tv_nsec;
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::get_time
// ----------------------------------------------------------------------------
void ThreadingUtilities::get_time (Timespec & abs_time, unsigned long delay_msecs)
{
  struct timeval now;
  ::gettimeofday(&now, NULL);

  abs_time.tv_sec  = now.tv_sec + delay_msecs / 1000;
  delay_msecs -=  delay_msecs / 1000 * 1000;
  abs_time.tv_nsec = now.tv_usec * 1000;
  abs_time.tv_nsec += delay_msecs * 1000000;
  abs_time.tv_sec  += abs_time.tv_nsec / MAX_NSECS;
  abs_time.tv_nsec %= MAX_NSECS;
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::get_time
// ----------------------------------------------------------------------------
void ThreadingUtilities::get_time (Timespec & abs_time, unsigned long delay_secs, unsigned long nano_secs)
{
  struct timeval now;
  ::gettimeofday(&now, NULL);

  abs_time.tv_sec  = now.tv_sec + delay_secs;
  abs_time.tv_nsec = now.tv_usec * 1000;
  abs_time.tv_nsec += nano_secs;
  abs_time.tv_sec  += abs_time.tv_nsec / MAX_NSECS;
  abs_time.tv_nsec %= MAX_NSECS;
}

} // namespace yat
