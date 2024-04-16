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

#ifndef _YAT_FUTURE_H_
#define _YAT_FUTURE_H_

// ----------------------------------------------------------------------------
// DEPENDENCIES
// ----------------------------------------------------------------------------
#include <yat/any/GenericContainer.h>
#include <yat/threading/Thread.h>
#include <yat/threading/Condition.h>
#include <yat/threading/Utilities.h>
#include <yat/threading/Atomic.h>
#include <yat/utils/ArgPack.h>
#include <yat/utils/Callback.h>
#include <yat/utils/Logging.h>
#include <yat/time/Time.h>

namespace yat {

#ifdef YAT_DEBUG_FUTURE
  #define YAT_FUTURE_LOG(s, ...) \
    do \
    { \
      yat::fcout(s), ##__VA_ARGS__; \
    } while(0)
#else
  #define YAT_FUTURE_LOG(s, ...)
#endif

// =============================================================================
// The AsyncResult is an internal class, it should not be instantiated in client 
// application
// =============================================================================
struct AsyncResult
{
public:
  AsyncResult() : m_ready(false), m_error(false), m_cond(m_mtx)
  {
    YAT_FUTURE_LOG("AsyncResult::AsyncResult() {}", this);
  }

  ~AsyncResult()
  {
    YAT_FUTURE_LOG("AsyncResult::~AsyncResult() {}", this);
  }

  template <typename T>
  void set_value(T& v)
  {
    if( !m_ready )
    {
      m_container_ptr = new GenericContainer<T>();
      dynamic_cast<GenericContainer<T>*>(m_container_ptr.get())->set(v);
      m_ready = true;
      AutoMutex<> lock_(m_mtx);
      m_cond.broadcast();
    }
    else
      throw yat::Exception("ERROR", "The shared state already store a value or exception",
                           "yat::AsyncResult::set_value");
  }

  template <typename T>
  void set_value(const T& v)
  {
    if( !m_ready )
    {
      m_container_ptr = new GenericContainer<T>();
      dynamic_cast<GenericContainer<T>*>(m_container_ptr.get())->set(v);
      m_ready = true;
      AutoMutex<> lock_(m_mtx);
      m_cond.broadcast();
    }
    else
      throw yat::Exception("ERROR", "The shared state already store a value or exception",
                           "yat::AsyncResult::set_value");
  }

  void set_exception(const Exception& ye)
  {
    if( !m_ready )
    {
      m_error = true;
      m_except = ye;
      m_ready = true;
      AutoMutex<> lock_(m_mtx);
      m_cond.broadcast();
    }
    else
      throw yat::Exception("ERROR", "The shared state already store a value or exception",
                           "yat::AsyncResult::set_exception");
  }

  template <typename T>
  T& get()
  {
    if( m_ready )
    {
      if( !m_error )
      {
        GenericContainer<T>* content_p = dynamic_cast<GenericContainer<T>*>(m_container_ptr.get());
        if( content_p )
          return content_p->get();
        else
          throw Exception("BAD_CAST", "can't get value because of mismatching types", "yat::AsyncResult::get<T>");
      }
      else
        throw m_except;
    }
    throw Exception("NO_DATA", "value not available", "yat::AsyncResult::get<T>");
  }

  void get()
  {
    if( m_ready )
    {
      if( m_error )
        throw m_except;
    }
    throw Exception("NO_DATA", "value not available", "yat::AsyncResult::get");
  }

  bool ready() const { return m_ready; }

  void wait()
  {
    AutoMutex<> lock_(m_mtx);
    if( !m_ready )
      m_cond.wait();
  }

  bool wait_for(const Duration& duration)
  {
    AutoMutex<> lock_(m_mtx);
    if( !m_ready )
    {
      yat::uint32 secs, nanos;
      duration.get(&secs, &nanos);
      return m_cond.timed_wait(secs, nanos);
    }
    return true;
  }

private:
  atomic_bool m_ready;
  bool        m_error;
  SharedPtr<Container> m_container_ptr;
  Exception m_except;
  Mutex m_mtx;
  Condition m_cond;
};

typedef YAT_SHARED_PTR(AsyncResult) AsyncResultPtr;
typedef YAT_WEAK_PTR(AsyncResult) AsyncResultWPtr;

// =============================================================================
//! \class Future
//!
//! \brief Waits for a value (possibly referenced by other futures) that is set
//! asynchronously 
//!
//! The class Future provides a mechanism to access the result of
//! asynchronous operations:\n
//! An asynchronous operation (created via yat::async) can provide a yat::Future
//! object to the creator of that asynchronous operation. 
//! The creator of the asynchronous operation can then use a variety of methods
//! to query, wait for, or extract a value from the yat::Future. These methods
//! may block if the asynchronous operation has not yet provided a value. 
//! When the asynchronous operation is ready to send a result to the creator,
//! it can do so by modifying shared state (e.g. yat::Promise::set_value) that is
//! linked to the creator's yat::Future.
// =============================================================================
class YAT_DECL Future
{
  friend class Promise;

public:

  //! Construct an empty  yat::Future associated with no state
  Future()
  {
    YAT_FUTURE_LOG("Future::Future() {}", this);
  }

  ~Future()
  {
    YAT_FUTURE_LOG("Future::~Future() {}", this);
  }

  //! Move constructor
  Future(Future&& other)
  {
    YAT_FUTURE_LOG("Future::Future(Future&&) {}", this);
    m_result_ptr = other.m_result_ptr;
    other.m_result_ptr = 0;
  }

  //! Moves the Future object
  Future& operator=(Future&& other)
  {
    m_result_ptr = other.m_result_ptr;
    other.m_result_ptr = 0;
    return *this;
  }

  //! \brief Returns the result
  //!
  //! Waits until the future has a valid result and (depending on which template
  //! type is used) retrieves it. It effectively calls wait() in order to wait
  //! for the result.\n
  //! If an exception was stored in the shared state referenced by the future
  //! (e.g. via a call to yat::Promise::set_exception()) then that exception
  //! will be thrown. 
  //!
  //! \return result value
  template <typename T>
  const T& get() const
  {
    wait();
    return m_result_ptr->get<T>();
  }

  //! \brief Wait the result: return nothing or throw an exception
  //!
  //! Waits until the future has a valid result. It effectively calls wait()
  //! in order to wait for the result.\n
  //! If an exception was stored in the shared state referenced by the future
  //! (e.g. via a call to yat::Promise::set_exception()) then that exception
  //! will be thrown. 
  void get() const
  {
    wait();
    m_result_ptr->get();
  }

  //! Checks if the future has a shared state 
  bool valid() const 
  {
    return m_result_ptr ? true : false;
  }

  //! \brief Waits for the result to become available 
  void wait() const
  {
    if( valid() )
    {
      m_result_ptr->wait();
    }
    else
      throw Exception("NO_STATE", "No valid shared state", "yat::Future::wait");
  }

  //! \brief Waits with a timeout for the result to become available
  //!
  //! \param tmout waiting time, return true if the future become ready before
  //! timeout expiration, false otherwise
  bool wait_for(const Duration& tmout)
  {
    if( valid() )
    {
      return m_result_ptr->wait_for(tmout);
    }
    else
      throw Exception("NO_STATE", "No valid shared state", "yat::Future::wait");
  }

  //! checks if the result is available (the cat is dead or alive ?)
  bool is_ready()
  {
    if( valid() )
    {
      return m_result_ptr->ready();
    }
    else
      throw Exception("NO_STATE", "No valid shared state", "yat::Future::wait");
  }

private:
  AsyncResultPtr m_result_ptr;
};

// =============================================================================
//! \class Promise
//!
//! \brief Stores a value for asynchronous retrieval
//!
//! The class yat::promise provides a facility to store a value or an exception
//! that is later acquired asynchronously via a yat::Future object created by
//! the yat::Promise object. Note that the yat::Promise object is meant to be
//! used only once.
//! \n
//! Each promise is associated with a shared state, which contains some state
//! information and a result which may be not yet evaluated, evaluated to a
//! value (possibly void) or evaluated to an exception. A promise may do three
//! things with the shared state:\n
//! - make ready: the promise stores the result or the exception in the shared
//!   state. Marks the state ready and unblocks any thread waiting on a future
//!   associated with the shared state.\n
//! - release: the promise gives up its reference to the shared state. If this
//!   was the last such reference, the shared state is destroyed. Unless this
//!   was a shared state created by yat::async which is not yet ready, this
//!   operation does not block.\n
//! - abandon: the promise stores the exception of type yat::Exception makes
//!   the shared state ready, and then releases it.
// =============================================================================
class YAT_DECL Promise
{
public:
  Promise() : m_get_future_called(false), m_result_ptr(new AsyncResult)
  {
    YAT_FUTURE_LOG("Promise::Promise() {}", this);
  }

  ~Promise()
  {
    YAT_FUTURE_LOG("Promise::~Promise() {}", this);
  }

  //! move c-tor
  Promise(Promise&& other)
  {
    YAT_FUTURE_LOG("Promise::Promise(Promise&&) {}", this);
    m_result_ptr = other.m_result_ptr;
    other.m_result_ptr = 0;
    std::swap(m_get_future_called, other.m_get_future_called);
  }

  //! Move assignment operator
  Promise& operator=( Promise&& other )
  {
    YAT_FUTURE_LOG("Promise::operator=(Promise&&)", this);
    m_result_ptr = other.m_result_ptr;
    other.m_result_ptr = 0;
    m_get_future_called = other.m_get_future_called;
    return *this;
  }

  //! Gets the yat::Future object
  //! This call can be performed only once on a yat::Promise instance
  Future get_future()
  {
    if( !m_result_ptr )
    {
      throw Exception("NO_STATE", "no result shared state.",
                                    "yat::Promise::get_future");

    }
    if( !m_get_future_called )
    {
      m_get_future_called = true;
      Future future;
      future.m_result_ptr = m_result_ptr;
      return future;
    }
    throw Exception("ERROR", "get_future has already been called.",
                                  "yat::Promise::get_future");
  }

  //! Sets the result to specific value then make the shared state ready
  template <typename T>
  void set_value(T& v)
  {
    m_result_ptr->set_value<T>(v);
  }

  //! Sets the result to specific value then make the shared state ready
  template <typename T>
  void set_value(const T& v)
  {
    m_result_ptr->set_value<T>(v);
  }

  void set_exception(const Exception& e)
  {
    m_result_ptr->set_exception(e);
  }

private:
  bool m_get_future_called;
  AsyncResultPtr m_result_ptr;

  Promise(const Promise& other);
  Promise& operator=(const Promise& other);
};

// =============================================================================
//! The argument type passed to the thread function invoked through a JThread
//! object
// =============================================================================
struct YAT_DECL AsyncParams
{
  Promise promise;
  ArgPack args;

  AsyncParams()
  {
    YAT_FUTURE_LOG("AsyncParams::AsyncParams() {}", this);
  }

  ~AsyncParams()
  {
    YAT_FUTURE_LOG("AsyncParams::~AsyncParams() {}", this);
  }

  template<typename T>
  const T& get(std::size_t idx=0) const
  {
    return args.get<T>(idx);
  }

  std::size_t count() const { return args.count(); }
};

typedef AsyncParams& AsyncFunctionArg;

const char* ASYNC_ERROR = "Can't execute asynchronous function";

// =============================================================================
//! The thread entry point of the function or a method invoked through
//! yat::async
// =============================================================================
YAT_DEFINE_CALLBACK(AsyncFunction, AsyncFunctionArg);

class __AsyncImpl__: public Thread
{
public:
  __AsyncImpl__(AsyncFunction cb, SharedPtr<AsyncParams>& args_ptr)
  : Thread(args_ptr.get()), m_cb(cb), m_args_ptr(args_ptr)
  {
    YAT_FUTURE_LOG("__AsyncImpl__::__AsyncImpl__() {}", this);
    start();
  }

  ~__AsyncImpl__()
  {
    YAT_FUTURE_LOG("__AsyncImpl__::~__AsyncImpl__() {}", this);
  }

  void run(Thread::IOArg)
  {
    try
    {
      m_cb(*(m_args_ptr.get()));
    }
    catch(const Exception& ye)
    {
      m_args_ptr->promise.set_exception(ye);
    }
    catch(const std::exception& e)
    {
      yat::Exception ye("STD_ERROR", e.what(), "yat::async");
      m_args_ptr->promise.set_exception(ye);
    }
    catch(...)
    {
      yat::Exception ye("UNKNOWN_ERROR", "Unknown error occured", "yat::async");
      m_args_ptr->promise.set_exception(ye);      
    }
  }

  void exit() {}

private:
  AsyncFunction m_cb;
  SharedPtr<AsyncParams> m_args_ptr;
};

//! \brief asynchronously execute a function
//!
//! The function template std::async runs the function f asynchronously
//! and returns a yat::Future that will eventually hold the result of that
//! function call.
//!
//! \param f function to be asynchronously executed
template<typename Function>
YAT_DECL Future async(Function f)
{
  SharedPtr<AsyncParams> args_ptr = new AsyncParams;
  try
  {
    new __AsyncImpl__(AsyncFunction::instanciate(f), args_ptr);
  }
  catch(Exception& ex)
  {
    RETHROW_YAT_ERROR(ex, "ERROR", ASYNC_ERROR, "yat::async<Function>");
  }

  return args_ptr->promise.get_future();
}

//! \brief asynchronously execute a function
//!
//! The function template std::async runs the function f asynchronously
//! and returns a yat::Future that will eventually hold the result of that
//! function call.
//!
//! \param f function to be asynchronously executed
//! \param args function argument(s)
template<typename Function>
YAT_DECL Future async(Function f, ArgPack&& args)
{
  SharedPtr<AsyncParams> args_ptr = new AsyncParams;
  args_ptr->args.swap(args);
  try
  {
    new __AsyncImpl__(AsyncFunction::instanciate(f), args_ptr);
  }
  catch(Exception& ex)
  {
    RETHROW_YAT_ERROR(ex, "ERROR", ASYNC_ERROR, "yat::async<Function>");
  }

  return args_ptr->promise.get_future();
}

//! \brief asynchronously execute a member function of an object
//!
//! The function template std::async runs the function f asynchronously
//! and returns a yat::Future that will eventually hold the result of that
//! function call.
//!
//! \param m member function to be asynchronously executed
//! \param o object instance
template<typename Object, class ClassMember>
YAT_DECL Future async(ClassMember m, Object& o)
{
  SharedPtr<AsyncParams> args_ptr = new AsyncParams;
  try
  {
    new __AsyncImpl__(AsyncFunction::instanciate(o, m), args_ptr);
  }
  catch(Exception& ex)
  {
    RETHROW_YAT_ERROR(ex, "ERROR", ASYNC_ERROR, "yat::async<Object, ClassMember>");
  }

  return args_ptr->promise.get_future();
}

//! \brief asynchronously execute a member function of an object
//!
//! The function template std::async runs the function f asynchronously
//! and returns a yat::Future that will eventually hold the result of that
//! function call.
//!
//! \param m member function to be asynchronously executed
//! \param o object instance
//! \param args member function argument(s)
template<typename Object, class ClassMember>
YAT_DECL Future async(ClassMember m, Object& o, ArgPack&& args)
{
  SharedPtr<AsyncParams> args_ptr = new AsyncParams;
  args_ptr->args.swap(args);
  try
  {
    new __AsyncImpl__(AsyncFunction::instanciate(o, m), args_ptr);
  }
  catch(Exception& ex)
  {
    RETHROW_YAT_ERROR(ex, "ERROR", ASYNC_ERROR, "yat::async<Object, ClassMember>");
  }

  return args_ptr->promise.get_future();
}

} // namespace yat

#endif //- _YAT_FUTURE_H_
