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
// Copyright (C) 2006-2024 The Tango Community
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

#ifndef _YAT_JTHREAD_H_
#define _YAT_JTHREAD_H_

// ----------------------------------------------------------------------------
// DEPENDENCIES
// ----------------------------------------------------------------------------
#include <yat/threading/Thread.h>
#include <yat/threading/Utilities.h>
#include <yat/threading/Atomic.h>
#include <yat/utils/ArgPack.h>
#include <yat/utils/Callback.h>
#include <yat/utils/Logging.h>

namespace yat {

// =============================================================================
struct StopState
{
  StopState() : stop_requested(false) {}
  atomic_bool stop_requested;
};

typedef YAT_SHARED_PTR(StopState) StopStatePtr;
typedef YAT_WEAK_PTR(StopState) StopStateWPtr;

// =============================================================================
//! \brief an interface for querying if a yat::JThread cancellation request has
//! been made
//!
//! The yat::StopToken class provides the means to check if a stop request has
//! been made or can be made, for its associated yat::StopSource object. It is
//! essentially a thread-safe "view" of the associated stop-state.
// =============================================================================
class YAT_DECL StopToken
{
  friend class StopSource;
public:

  //! Construct an empty StopToken with no associated stop-state
  StopToken() {}

  //! Copy constructor. Constructs a StopToken whose associated stop-state is
  //! the same as that of other.
  StopToken(const StopToken &src) : m_stop_state_wptr(src.m_stop_state_wptr) {}

  //! Copy-assigns the associated stop-state of other to that of *this.
  StopToken& operator=(const StopToken &other);

  //! Checks if stop is possible
  bool stop_possible() const;

  //! Checks whether the associated stop-state has been requested to stop
  bool stop_requested() const;

  //! Specializes the std::swap algorithm
  void swap(StopToken& other);

private:
  StopStateWPtr m_stop_state_wptr;
};

struct NoStopState
{
  NoStopState() {};
};

// =============================================================================
//! The StopSource class provides the means to issue a stop request,
//! such as for Jthread cancellation. A stop request made for one
//! StopSource object is visible to all StopSources and StopTokens
//! of the same associated stop-state.
//! Once a stop is requested, it cannot be withdrawn.
//! Additional stop requests have no effect.
// =============================================================================
class YAT_DECL StopSource
{
  friend class JThread;
public:

  //! Constructs a StopSource with new shared StopState
  StopSource() { m_stop_state_ptr = new StopState; }

  //! Construct an empty StopSource with no associated stop-state
  StopSource(NoStopState) { }

  //! Constructs a StopSource whose associated stop-state is the same as that of src
  StopSource(const StopSource &src) : m_stop_state_ptr(src.m_stop_state_ptr) {}

  //! Checks if the StopSource object has a stop-state.
  bool stop_possible() const;

  //! Checks if the StopSource object has a stop-state and that state has
  //! received a stop request.
  bool stop_requested() const;

  //! Issues a stop request to the stop-state, if the stop_source object has
  //! a stop-state and it has not yet already had stop requested
  void request_stop();

  //! Returns a StopToken object associated with the StopSource's stop-state,
  //! if the stop_source has stop-state; otherwise returns a default-constructed (empty) stop_token.
  StopToken get_token() const;

  //! Exchanges the stop-state of this object with that of other.
  void swap(StopSource& other);

private:
  StopStatePtr m_stop_state_ptr;
};

// =============================================================================
//! The structure passed to the thread function invoked through a JThread
//! object
// =============================================================================
struct YAT_DECL JTFunctionParams
{
  //! The token that may be used to stop the thread execution
  //! It has no effect in detached mode
  StopToken stop_token;
  ArgPack args;

  //! Gets an input argument passed through a ArgPack object at JThread construction
  //!
  //! \param idx index of the argument
  //! \return argument value
  template<typename T>
  const T& get(std::size_t idx=0) const
  {
    return args.get<T>(idx);
  }

  //! Return the number of input arguments
  std::size_t count() const { return args.count(); }
};

typedef JTFunctionParams& JThreadFunctionArg;

// =============================================================================
//! The thread entry point of the function or a method invoked through
//! a JThread object
// =============================================================================
YAT_DEFINE_CALLBACK(JThreadFunction, JThreadFunctionArg);

//! Value returned by JThread::get_id() when the JThread object does not represent
//! an executing thread
#define NON_EXECUTING_THREAD 0

// =============================================================================
//! The class JThread represents a single thread of execution. It has the same
//! general behavior as Thread, except that JThread automatically rejoins
//! on destruction, and can be cancelled/stopped in certain situations.
//! Threads begin execution immediately upon construction of the associated
//! thread object (pending any OS scheduling delays), starting at the top-level
//! function provided as a constructor argument.
//! The return value of the top-level function is ignored and if it terminates
//! by throwing an exception, std::terminate is called.
//! Unlike Thread, the JThread logically holds an internal private member
//! of type yat:StopSource, which maintains a shared stop-state.
//! The function passed to the JThread constructor takes a StopToken as
//! its first argument, which will be passed in by the JThread from its internal
//! StopSource. This allows the function to check if stop has been requested
//! during its execution, and return if it has.
//! JThread objects may also be in the state that does not represent any
//! thread (after default construction, swap, detach, or join), and a thread of
//! execution may be not associated with any JThread objects (if launched in
//! detached mode).
//! No two JThread objects may represent the same thread of execution;
//! JThread is not CopyConstructible or CopyAssignable.
// =============================================================================
class YAT_DECL JThread
{
public:

  //! Logical JThread identifier
  typedef uintptr Id;

  JThread(): m_impl(0) {}

  //! Creates new yat::JThread object and associates it with a thread of execution.
  //!
  //! \param f the callable function executed in the new thread
  //! \param args arguments of the function
  //! \param detached true if the new thread have to be executed in detached mode
  template<typename Function>
  JThread(Function f, ArgPack&& args, bool detached = false)
  {
    JTFunctionParams* args_p = new JTFunctionParams;
    args_p->args.swap(args);
    launch1(f, args_p, detached);
  }

  //! Creates new yat::JThread object and associates it with a thread of execution.
  //!
  //! \param m the member function executed in the new thread
  //! \param o the instance of the object defining the executed member function
  //! \param args arguments of the function
  //! \param detached true if the new thread have to be executed in detached mode
  template<typename Object, class ClassMember>
  JThread(ClassMember m, Object& o, ArgPack&& args, bool detached = false)
  {
    JTFunctionParams* args_p = new JTFunctionParams;
    args_p->args.swap(args);
    launch2(m, o, args_p, detached);
  }

  //! Creates new yat::JThread object and associates it with a thread of execution.
  //!
  //! \param f the callable function executed in the new thread
  //! \param detached true if the new thread have to be executed in detached mode
  template<typename Function>
  JThread(Function f, bool detached = false)
  {
    JTFunctionParams* args_p = new JTFunctionParams;
    launch1(f, args_p, detached);
  }

  //! Creates new yat::JThread object and associates it with a thread of execution.
  //!
  //! \param m the member function executed in the new thread
  //! \param o the instance of the object defining the executed member function
  //! \param detached true if the new thread have to be executed in detached mode
  template<typename Object, class ClassMember>
  JThread(ClassMember m, Object& o, bool detached = false)
  {
    JTFunctionParams* args_p = new JTFunctionParams;
    launch2(m, o, args_p, detached);
  }

  //! move constructor
  JThread(JThread&& other);

  //! If the thread is joinable then request for stop then join it
  ~JThread();

  //! Moves the jthread object
  JThread& operator=(JThread&& other);

  //! Returns the thread logical identifier or NON_EXECUTING_THREAD if the
  //! JThread object does not represent any executing thread
  Id get_id() const;

  //! Returns the underlying implementation-defined thread handle
  ThreadUID native_handle() const;

  //! Join the thread if possible
  void join();

  //! Checks if the JThread object identifies an active thread of execution
  bool joinable() const { return m_impl ? true : false; }

  //! Checks if the underlying thread is currently running
  //! throw an exception if this object does not represent a thread
  bool running() const;

  //! Checks if the underlying thread is terminated
  //! throw an exception if this object does not represent a thread
  bool terminated() const;

  //! Exchange the underlying handle of this JThread with that of other.
  //! Therefore this object no longer represent a thread
  void swap(JThread& other);

  //! Return the number of physical threads
  static unsigned int hardware_concurrency();

  //! returns a stop_source object associated with the shared stop state of the thread
  StopSource get_stop_source() { return m_stop_source; }

  //! returns a stop_token associated with the shared stop state of the thread
  StopToken get_stop_token() { return m_stop_source.get_token(); }

  //! requests execution stop via the shared stop state of the thread
  void request_stop() { m_stop_source.request_stop(); }

private:

  template<typename Function>
  void launch1(Function f, JTFunctionParams* args_p, bool detached)
  {
    if( !detached )
      args_p->stop_token = m_stop_source.get_token();

    try
    {
      m_impl = new Impl(JThreadFunction::instanciate(f), args_p, detached);
      if( detached )
        m_impl = 0;
    }
    catch(Exception& ex)
    {
      delete args_p;
      RETHROW_YAT_ERROR(ex, "ERROR", "Can't create JThread object", "yat::JThread::JThread<f>");
    }
  }

  template<typename Object, class ClassMember>
  void launch2(ClassMember m, Object& o, JTFunctionParams* args_p, bool detached)
  {
    if( !detached )
      args_p->stop_token = m_stop_source.get_token();

    try
    {
      m_impl = new Impl(JThreadFunction::instanciate(o, m), args_p, detached);
      if( detached )
        m_impl = 0;
    }
    catch(Exception& ex)
    {
      delete args_p;
      RETHROW_YAT_ERROR(ex, "ERROR", "Can't create JThread object", "yat::JThread::JThread<m,o>");
    }
  }

  // private implementation
  class Impl : public Thread
  {
  public:
    Impl(JThreadFunction cb, Thread::IOArg ioarg, bool detached);
    Impl(JThreadFunction cb, StopToken token, Thread::IOArg ioarg, bool detached);
    Thread::IOArg run_undetached(Thread::IOArg ioarg);
    void run(Thread::IOArg ioarg);
    ThreadUID id() const { return m_id; }
    void join();
    void exit() {}

  private:
    JThreadFunction m_cb;
    ThreadUID m_id;
    StopToken m_stop_token;
  };

  JThread(const JThread&);
  JThread(JThread&);
  JThread& operator=(const JThread&);

  StopSource m_stop_source;
  Impl *m_impl;
};

} // namespace yat

#endif //- _YAT_JTHREAD_H_
