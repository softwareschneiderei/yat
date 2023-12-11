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

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/JThread.h>
#include <yat/utils/Logging.h>

namespace yat
{

//==============================================================================
// class StopToken
//==============================================================================

//------------------------------------------------------------------------
StopToken& StopToken::operator=(const StopToken &other)
{
  m_stop_state_wptr = other.m_stop_state_wptr;
  return *this;
}

//------------------------------------------------------------------------
bool StopToken::stop_possible() const
{
  return !m_stop_state_wptr.is_null() && !m_stop_state_wptr.expired();
}

//------------------------------------------------------------------------
bool StopToken::stop_requested() const
{
  return m_stop_state_wptr.is_null() ? false :
                        (m_stop_state_wptr.expired() ? false :
                         bool(m_stop_state_wptr.lock()->stop_requested));
}

//------------------------------------------------------------------------
void StopToken::swap(StopToken& other)
{
  m_stop_state_wptr.swap(other.m_stop_state_wptr);
}

//==============================================================================
// class StopSource
//==============================================================================

//------------------------------------------------------------------------
bool StopSource::stop_possible() const
{
  return !m_stop_state_ptr.is_null();
}

//------------------------------------------------------------------------
bool StopSource::stop_requested() const
{
  return m_stop_state_ptr.is_null() ? false :
                        bool(m_stop_state_ptr->stop_requested);
}

//------------------------------------------------------------------------
void StopSource::request_stop()
{
  if( m_stop_state_ptr )
    m_stop_state_ptr->stop_requested = true;
}

//------------------------------------------------------------------------
StopToken StopSource::get_token() const
{
  StopToken tok;
  if( m_stop_state_ptr )
    tok.m_stop_state_wptr = m_stop_state_ptr;
  return tok;
}

//------------------------------------------------------------------------
void StopSource::swap(StopSource& other)
{
  m_stop_state_ptr.swap(other.m_stop_state_ptr);
}

//==============================================================================
// class JThread
//==============================================================================

//------------------------------------------------------------------------
JThread::JThread(JThread&& other)
{
  m_impl = other.m_impl;
  other.m_impl = 0;
  std::swap(m_stop_source, other.m_stop_source);
}

//------------------------------------------------------------------------
JThread& JThread::operator=(JThread&& other)
{
  if( m_impl )
  {
    m_stop_source.request_stop();
    join();
  }

  std::swap(m_impl, other.m_impl);
  std::swap(m_stop_source, other.m_stop_source);

  return *this;
}

//------------------------------------------------------------------------
JThread::Id JThread::get_id() const
{
  if( m_impl )
    return uintptr(&m_impl);
  return NON_EXECUTING_THREAD;
}

//------------------------------------------------------------------------
ThreadUID JThread::native_handle() const
{
  if( m_impl )
    return m_impl->id();
  return YAT_INVALID_THREAD_UID;
}

//------------------------------------------------------------------------
void JThread::join()
{
  if( m_impl )
    m_impl->join();
  m_impl = 0;
}

//------------------------------------------------------------------------
bool JThread::running() const
{
  if( m_impl )
    return Thread::STATE_RUNNING == m_impl->state();
  throw Exception("BAD_STATE",
                  "JThread object does not represent any thread of execution",
                  "yat::JThread::running") ;
}

//------------------------------------------------------------------------
bool JThread::terminated() const
{
  if( m_impl )
    return Thread::STATE_TERMINATED == m_impl->state();
  throw Exception("BAD_STATE",
                  "JThread object does not represent any thread of execution",
                  "yat::JThread::terminated") ;
}

//------------------------------------------------------------------------
void JThread::swap(JThread& other)
{
  std::swap(m_impl, other.m_impl);
  std::swap(m_stop_source, other.m_stop_source);
}

//------------------------------------------------------------------------
unsigned int JThread::hardware_concurrency()
{
  return ThreadingUtilities::harware_concurrency();
}

//------------------------------------------------------------------------
JThread::~JThread()
{
  try
  {
    if( m_impl )
    {
      m_stop_source.request_stop();
      join();
    }
  }
  catch(...) {}
}

//------------------------------------------------------------------------
JThread::Impl::Impl(JThreadFunction cb, Thread::IOArg ioarg, bool detached):
Thread(ioarg), m_cb(cb), m_id(0)
{
  if( detached )
    start();
  else
    start_undetached();
}

//------------------------------------------------------------------------
JThread::Impl::Impl(JThreadFunction cb, StopToken token, Thread::IOArg ioarg, bool detached):
Thread(ioarg), m_cb(cb), m_id(0)
{
  m_stop_token.swap(token);
  start_undetached();
}

//------------------------------------------------------------------------
Thread::IOArg JThread::Impl::run_undetached(Thread::IOArg ioarg)
{
  m_id = ThreadingUtilities::self();
  UniquePtr<JTFunctionParams> arg_ptr;
  arg_ptr.reset(static_cast<JTFunctionParams*>(ioarg));
  m_cb(*(arg_ptr.get()));
  return 0;
}

//------------------------------------------------------------------------
void JThread::Impl::run(Thread::IOArg ioarg)
{
  m_id = ThreadingUtilities::self();
  UniquePtr<JTFunctionParams> arg_ptr;
  arg_ptr.reset(static_cast<JTFunctionParams*>(ioarg));
  m_cb(*(arg_ptr.get()));
}

//------------------------------------------------------------------------
void JThread::Impl::join()
{
  Thread::join(0);
}

} // namespace

