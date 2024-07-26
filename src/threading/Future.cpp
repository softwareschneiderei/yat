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

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Future.h>
#include <yat/utils/Logging.h>

namespace yat
{

// ============================================================================
// class __AsyncWorker__
// ============================================================================
//---------------------------------------------------------------------------
__AsyncWorker__::__AsyncWorker__(WeakPtr<__AsyncPool__>& pool_wptr)
{
  YAT_FUTURE_LOG("__AsyncWorker__::__AsyncWorker__() {}", this);
  m_pool_wptr = pool_wptr;
}

__AsyncWorker__::~__AsyncWorker__()
{
  YAT_FUTURE_LOG("__AsyncWorker__::~__AsyncWorker__() {}", this);
}

//---------------------------------------------------------------------------
void __AsyncWorker__::handle_message (yat::Message& msg)
{
  if( __AsyncWorker__::RUN == msg.type() )
  {
    YAT_FUTURE_LOG("__AsyncWorker__::handle_message() RUN msg received");
    AsyncRequest req = msg.get_data<AsyncRequest>();
    run(req.first, req.second);
  }
}

//---------------------------------------------------------------------------
void __AsyncWorker__::run(AsyncFunction cb, SharedPtr<AsyncParams> args_ptr)
{
  try
  {
    cb(*(args_ptr.get()));
  }
  catch(const Exception& ye)
  {
    args_ptr->promise.set_exception(ye);
  }
  catch(const std::exception& e)
  {
    yat::Exception ye("STD_ERROR", e.what(), "yat::async");
    args_ptr->promise.set_exception(ye);
  }
  catch(...)
  {
    yat::Exception ye("UNKNOWN_ERROR", "Unknown error occured", "yat::async");
    args_ptr->promise.set_exception(ye);
  }
  SharedPtr<__AsyncPool__> ptr = m_pool_wptr.lock();
  if( ptr )
  {
    Message* msg_p = Message::allocate(__AsyncManager__::DONE, DEFAULT_MSG_PRIORITY, true);
    msg_p->attach_data<Task>(this, false);
    ptr->wait_msg_handled(msg_p);
  }
}

// ============================================================================
// class __AsyncManager__
// ============================================================================
//---------------------------------------------------------------------------
__AsyncPool__::__AsyncPool__()
{
  //- a compromise
  m_max_threads = 2 * ThreadingUtilities::hardware_concurrency();
  m_min_threads = 0;
}

__AsyncPool__::~__AsyncPool__()
{
  YAT_FUTURE_LOG("__AsyncPool__::~__AsyncPool__()");
}

void __AsyncPool__::set_ptr(SharedPtr<__AsyncPool__>& pool_ptr)
{
  m_this_wptr = pool_ptr;
}

//---------------------------------------------------------------------------
void __AsyncPool__::async_request(AsyncFunction cb, SharedPtr<AsyncParams> args_ptr)
{
  YAT_FUTURE_LOG("__AsyncPool__::async_request()");
  __AsyncWorker__* worker_p = 0;
  {
    AutoMutex<> lock(m_mtx);
    if( m_idle_threads.size() > 0 )
    {
      YAT_FUTURE_LOG("__AsyncPool__::async_request() reuse worker");
      std::set<SharedPtr<__AsyncWorker__>>::iterator it = m_idle_threads.begin();
      worker_p = it->get();
      m_active_threads.insert(*it);
      m_idle_threads.erase(it);
    }
    else if( m_active_threads.size() < m_max_threads )
    {
      YAT_FUTURE_LOG("__AsyncPool__::async_request() new worker");

      SharedPtr<__AsyncWorker__> worker_ptr(new __AsyncWorker__(m_this_wptr), TaskExiter());
      worker_ptr->go();
      m_active_threads.insert(worker_ptr);
      worker_p = worker_ptr.get();
    }
    else
    {
      YAT_FUTURE_LOG("__AsyncManager__::Pool::async_request() no worker available");
      m_pending_requests.push_back(std::make_pair(cb, args_ptr));
    }
  }
  if( worker_p )
  {
    Message* msg_p = Message::allocate(__AsyncWorker__::RUN);
    msg_p->attach_data(std::make_pair(cb, args_ptr));
    worker_p->post(msg_p);
  }
}

//---------------------------------------------------------------------------
void __AsyncPool__::handle_message (yat::Message& msg)
{
  switch( msg.type() )
  {
  case yat::TASK_INIT:
    set_periodic_msg_period(250);
    break;
  case __AsyncManager__::DONE:
    {
      YAT_FUTURE_LOG("__AsyncPool__::handle_message() DONE notified.");

      Task* task_p = msg.detach_data<Task>();
      SharedPtr<__AsyncWorker__> worker_ptr;

      AutoMutex<> lock(m_mtx);
      std::set<SharedPtr<__AsyncWorker__>>::iterator it = m_active_threads.begin();
      for( ; it != m_active_threads.end(); ++it )
      {
        if( it->get() == task_p )
        {
          worker_ptr = *it;
          break;
        }
      }

      if( m_pending_requests.size() > 0 )
      {
        //- reuse worker
        YAT_FUTURE_LOG("__AsyncPool__::handle_message() reuse worker");
        std::vector<AsyncRequest>::iterator it = m_pending_requests.begin();
        AsyncRequest req = *it;

        Message* msg_p = Message::allocate(__AsyncWorker__::RUN);
        msg_p->attach_data(req);
        worker_ptr->post(msg_p);

        m_pending_requests.erase(it);
      }
      else if( it != m_active_threads.end() )
      {
        YAT_FUTURE_LOG("__AsyncManager__::Pool::handle_message() no pending request");
        m_idle_threads.insert(*it);
        m_active_threads.erase(it);
        enable_periodic_msg(true);
      }
    }
    break;
  case yat::TASK_PERIODIC:
    {
      AutoMutex<> lock(m_mtx);
      if( m_idle_threads.size() > m_min_threads )
      {
        YAT_FUTURE_LOG("__AsyncPool__::handle_message() discard worker");
        m_idle_threads.erase(m_idle_threads.begin());
      }
      else
        enable_periodic_msg(false);
    }
    break;
  }
}

//---------------------------------------------------------------------------
void __AsyncPool__::set_pool_size(std::size_t min, std::size_t max)
{
  AutoMutex<> lock(m_mtx);
  m_max_threads = max;
  m_min_threads = min;
}

// ============================================================================
// class __AsyncManager__
// ============================================================================
//---------------------------------------------------------------------------
__AsyncManager__::__AsyncManager__()
{
  YAT_FUTURE_LOG("__AsyncManager__::__AsyncManager__()");

  m_pool_ptr.reset(new __AsyncPool__, TaskExiter());
  m_pool_ptr->set_ptr(m_pool_ptr);
  m_pool_ptr->go();
}

__AsyncManager__::~__AsyncManager__()
{
  YAT_FUTURE_LOG("__AsyncManager__::~__AsyncManager__()");
}

//---------------------------------------------------------------------------
void __AsyncManager__::async_request(AsyncFunction cb, SharedPtr<AsyncParams>& args_ptr)
{
  YAT_FUTURE_LOG("__AsyncManager__::async_request()");
  instance().m_pool_ptr->async_request(cb, args_ptr);
}

//---------------------------------------------------------------------------
void __AsyncManager__::set_pool_size(std::size_t min, std::size_t max)
{
  instance().m_pool_ptr->set_pool_size(min, max);
}

// ============================================================================
// free functions
// ============================================================================
void async_set_pool_size(std::size_t min, std::size_t max)
{
  __AsyncManager__::set_pool_size(min, max);
}

} // namespace

