//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2010  The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// see AUTHORS file 
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
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author N.Leclercq - Synchrotron SOLEIL
 */

#ifndef _YAT_REF_COUNTER_H_
#define _YAT_REF_COUNTER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <iostream>
#include <typeinfo> 
#include <yat/CommonHeader.h>
#include <yat/threading/Mutex.h>
#include <yat/threading/Utilities.h>

namespace yat 
{

#define PTR_DBG(s)
//#define PTR_DBG(s) std::cout << std::hex << "[" << yat::ThreadingUtilities::self() << "][" << (void*)(this) << "] - " << std::dec << s << std::endl;

// Define the default counter type
typedef yat::uint32 counter_t;

// ============================================================================
// functor: DefaultDeleter
// ============================================================================
template <typename T>
struct DefaultDeleter
{
  void operator()(T* object)
  {
    try
    {
      PTR_DBG( "Deleting object @" << (void*)object );
      delete object;
    }
    catch(...)
    {
    }
  }
};

// ============================================================================
// class: CountBase
// Base class for CountImpl
// ============================================================================
template <typename C = counter_t, typename L = yat::Mutex>
class CountBase
{
public:
  CountBase() : m_use_count(1), m_weak_count(0)
  {
    PTR_DBG("CountBase::CountBase");
  }
  
  virtual ~CountBase()
  {
    PTR_DBG("CountBase::~CountBase");
  }

  // object disposal method
  virtual void dispose() = 0; // nothrow

  const C add_ref()
  {
    PTR_DBG("CountImpl::add_ref(->" << m_use_count+1 << ")");
    yat::AutoMutex<L> guard(this->m_lock);
    ++m_use_count;
    return m_use_count;
  }
  
  const C add_weak_ref()
  {
    PTR_DBG("CountImpl::add_weak_ref(->" << m_weak_count+1 << ")");
    yat::AutoMutex<L> guard(this->m_lock);
    ++m_weak_count;
    return m_weak_count;
  }
  
  const C dec_ref()
  {
    PTR_DBG("CountImpl::dec_ref(->" << m_use_count-1 << ")");
    yat::AutoMutex<L> guard(this->m_lock);
    YAT_ASSERT(this->m_use_count > 0);
    --m_use_count;
    return m_use_count;
  }
  
  const C dec_weak_ref()
  {
    PTR_DBG("CountImpl::dec_weak_ref(->" << m_weak_count-1 << ")");
    YAT_ASSERT(this->m_weak_count > 0);
    --m_weak_count;
    return m_weak_count;
  }
  
  C & use_count()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return m_use_count;
  }

  const C & weak_count()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return m_weak_count;
  }

  bool expired()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return 0 == m_use_count && 0 == m_weak_count;
  }
  
  void lock()
  {
    m_lock.lock();
  }

  void unlock()
  {
    m_lock.unlock();
  }

protected:
  C m_use_count;
  C m_weak_count;
  L m_lock;
};

// ============================================================================
// class: CountImpl 
// where the things are actually counted !
// ============================================================================
template <typename T, typename D, typename C = counter_t, typename L = yat::Mutex>
class CountImpl: public CountBase<C,L>
{
public:
  
  CountImpl(T* p, D d) : m_deleter(d), m_data(p)
  {
    PTR_DBG("CountImpl::CountImpl");
  }
  
  ~CountImpl()
  {
    PTR_DBG("CountImpl::~CountImpl");
  }

  void dispose()
  {
    m_deleter(m_data);
  }

private:
  D  m_deleter;
  T* m_data;
};

template <typename C = counter_t, typename L = yat::Mutex> class WeakCounter;

// ============================================================================
// class: SharedCounter
// ============================================================================
template <typename C = counter_t, typename L = yat::Mutex>
class SharedCounter
{
  friend class WeakCounter<C,L>;
public:

  typedef SharedCounter<C,L> ThisType;
  
  //! constructor
  template <typename T>
  SharedCounter(T* p)
  {
    PTR_DBG("SharedCounter::SharedCounter(p)");
    this->m_count = new CountImpl<T,DefaultDeleter<T>,C,L>(p, DefaultDeleter<T>());
  }

  //! constructor with specific deleter
  template <typename T, typename D>
  SharedCounter(T* p, D d)
  {
    PTR_DBG("SharedCounter::SharedCounter(p, d)");
    this->m_count = new CountImpl<T,D,C,L>(p, d);
  }

  //! destructor
  virtual ~SharedCounter()
  {
    PTR_DBG("SharedCounter::~SharedCounter()");
    release();
  };

  //! copy constructor from weak pointer
  explicit SharedCounter (const WeakCounter<C,L>& cnt)
  {
    PTR_DBG("SharedCounter::SharedCounter(const WeakCounter<T,L>&)");
    cnt.m_count->lock();
    if( cnt.m_count->use_count() > 0 )
    {
      this->m_count = cnt.m_count;
      this->m_count->add_ref();
    }
    cnt.m_count->unlock();
  }

  //! copy constructor
  SharedCounter (const ThisType& cnt)
  {
    PTR_DBG("SharedCounter::SharedCounter(const ThisType&)");
    this->m_count = cnt.m_count;
    this->m_count->add_ref();    
  }

  //! release counter and underlying object
  void release ()
  {
    PTR_DBG("SharedCounter::release()");
    m_count->dec_ref();
    if( m_count->expired() )
    {
      // No more reference to the counter object, we can destroy it
      this->m_count->dispose();
      delete this->m_count;
      this->m_count = 0;
    }
    else if( m_count->use_count() == 0 )
    {
      // At least one weak reference remains on the counter
      // We just destroy the referenced object
      this->m_count->dispose();
    }
  }

  //! reset content
  void reset ()
  {
    PTR_DBG("SharedCounter::reset()");
    ThisType().swap(*this);
  }

  //! copy operator
  const ThisType& operator= (const ThisType& s)
  {
    PTR_DBG("SharedCounter::operator=(const ThisType&)");
    if (this != &s)
    {
      release();
      this->m_count = s.m_count;
      this->m_count->add_ref();
    }
    return *this;
  }
  
  //! swap content
  void swap (ThisType& s)
  {
    std::swap(this->m_count, s.m_count);
  }

  //! unique
  bool unique () const
  {
    return this->m_count->use_count() == 1;
  }

  //! use count
  const C use_count () const
  {
    return this->m_count ? this->m_count->use_count() : 0;
  }

  //- implicit conversion to bool
  typedef C* SharedCounter::*unspecified_bool_type;
  operator unspecified_bool_type() const
  {
    return this->m_count ? m_count->use_count() : 0;
  }

private:
  CountBase<C, L>* m_count; // The shared counter
  L m_lock;
};

// ============================================================================
// class: WeakCounter
// This class is used by the WeakPtr pointer type
// ============================================================================
template <typename C, typename L>
class WeakCounter
{
  friend class SharedCounter<C,L>;

public:

  typedef WeakCounter<C,L> ThisType;
  
  //! constructor
  WeakCounter ()
  {
    PTR_DBG("WeakCounter::WeakCounter()");
    this->m_count = 0;
  }

  //! copy constructor
  WeakCounter (const ThisType& cnt)
  {
    PTR_DBG("WeakCounter::WeakCounter(const ThisType&)");
    this->m_count = cnt.m_count;
    this->m_count->add_weak_ref();
  }

  //! copy constructor from shared pointer
  WeakCounter (const SharedCounter<C,L>& cnt)
  {
    PTR_DBG("WeakCounter::WeakCounter(const SharedCounter<T,L>&)");
    this->m_count = cnt.m_count;
    this->m_count->add_weak_ref();
  }

  //! destructor
  virtual ~WeakCounter()
  {
    PTR_DBG("WeakCounter::~WeakCounter()");
    release();
  };

  //! copy operator
  const ThisType& operator= (const ThisType& s)
  {
    PTR_DBG("WeakCounter::operator=(const ThisType&)");
    if (this != &s)
    {
      release();
      this->m_count = s.m_count;
      this->m_count->add_weak_ref();
    }
    return *this;
  }
  
  //! release counter
  void release()
  {
    PTR_DBG("WeakCounter::release()");
    if( m_count )
    {
      m_count->dec_weak_ref();
      if( m_count->expired() )
      {
        delete this->m_count;
        this->m_count = 0;
      }
    }
  }

  //! reset content
  void reset ()
  {
    PTR_DBG("WeakCounter::reset()");
    ThisType().swap(*this);
  }

  //! swap content
  void swap (const WeakCounter & s)
  {
    std::swap(this->m_count, s.m_count);
  }

  //! use count
  const C use_count () const
  {
    return this->m_count ? this->m_count->use_count() : 0;
  }

  //- implicit conversion to bool
  typedef C* WeakCounter::*unspecified_bool_type;
  operator unspecified_bool_type() const
  {
    return this->m_count ? m_count->use_count() : 0;
  }

private:
  CountBase<C,L>* m_count; // The shared counter
};

} // namespace

#endif



