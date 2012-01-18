//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2010  The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// Ramon Sune (ALBA) for the yat::Signal class 
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
#include <yat/CommonHeader.h>
#include <yat/threading/Mutex.h>
#include <yat/threading/Utilities.h>
#include <iostream>
 
namespace yat 
{

#define PTR_DBG(s)
//#define PTR_DBG(s) std::cout << std::hex << "[" << yat::ThreadingUtilities::self() << "][" << (void*)(this) << "] - " << std::dec << s << std::endl;

// ============================================================================
// class: CountImpl 
// where the things are actually counted !
// ============================================================================
template <typename T = yat::uint32, typename L = yat::NullMutex>
class CountImpl
{
  typedef CountImpl<T, L> ThisType;
  
public:
  
  CountImpl() : m_use_count(1), m_weak_count(0)
  {
  }
  
  const T add_ref()
  {
    PTR_DBG("CountImpl::add_ref(->" << m_use_count+1 << ")");
    yat::AutoMutex<L> guard(this->m_lock);
    YAT_ASSERT(this->m_use_count > 0);
    ++m_use_count;
    return m_use_count;
  }
  
  const T add_weak_ref()
  {
    PTR_DBG("CountImpl::add_weak_ref(->" << m_weak_count+1 << ")");
    yat::AutoMutex<L> guard(this->m_lock);
    YAT_ASSERT(this->m_weak_count >= 0);
    ++m_weak_count;
    return m_weak_count;
  }
  
  const T dec_ref()
  {
    PTR_DBG("CountImpl::dec_ref(->" << m_use_count-1 << ")");
    yat::AutoMutex<L> guard(this->m_lock);
    YAT_ASSERT(this->m_use_count > 0);
    --m_use_count;
    return m_use_count;
  }
  
  const T dec_weak_ref()
  {
    PTR_DBG("CountImpl::dec_weak_ref(->" << m_weak_count-1 << ")");
    YAT_ASSERT(this->m_weak_count > 0);
    --m_weak_count;
    return m_weak_count;
  }
  
  const T use_count()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return m_use_count;
  }

  const T weak_count()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return m_weak_count;
  }

  bool expired()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return 0 == m_use_count && 0 == m_weak_count;
  }
  
  const ThisType& operator= (const ThisType& s)
  {
    if (this != &s)
    {
      this->m_use_count = s.m_use_count;
      this->m_weak_count = s.m_weak_count;
    }
    return *this;
  }

  void lock()
  {
    m_lock.lock();
  }

  void unlock()
  {
    m_lock.unlock();
  }

private:
  T m_use_count;
  T m_weak_count;
  L m_lock;
};

template <typename T = unsigned long, typename L = yat::NullMutex> class WeakCounter;

// ============================================================================
// class: SharedCounter
// ============================================================================
template <typename T = yat::uint32, typename L = yat::NullMutex>
class SharedCounter
{
  friend class WeakCounter<T,L>;
public:

  typedef SharedCounter<T,L> ThisType;
  
  // referenced object deletion interface
  class IDeleter
  {
  public:
    //! discard referenced object
    virtual void destroy()=0;
  };

  //! constructor
  SharedCounter()
  {
    this->m_count = new CountImpl<T,L>();
    this->m_deleter = 0;
  }

  //! destructor
  virtual ~SharedCounter()
  {
    PTR_DBG("SharedCounter::~SharedCounter()");
  };

  //! deleter affectation
  void set_deleter(IDeleter *deleter)
  {
    this->m_deleter = deleter;
  }
  
  //! copy constructor
  SharedCounter (const WeakCounter<T,L>& cnt);

  //! copy constructor
  SharedCounter (const ThisType& cnt)
  {
    this->m_count = cnt.m_count;
    this->m_count->add_ref();    
  }

  //! release counter and underlying object
  void release()
  {
    PTR_DBG("SharedCounter::release()");
    m_count->dec_ref();
    if( m_count->expired() )
    {
      // No more reference to the counter object, we can destroy it
      delete this->m_count;
      this->m_count = 0;
      if( this->m_deleter )
        this->m_deleter->destroy();
    }
    else if( m_count->use_count() == 0 )
    {
      // At least one weak reference remains on the counter
      // We just destroy the referenced object
      if( this->m_deleter )
        this->m_deleter->destroy();
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
  const T & use_count () const
  {
    return this->m_count->use_count();
  }

  //- implicit conversion to bool
  typedef T* SharedCounter::*unspecified_bool_type;
  operator unspecified_bool_type() const
  {
    return this->m_count ? m_count->use_count() : 0;
  }

private:
  CountImpl<T, L>* m_count; // The shared counter
  IDeleter *m_deleter;
  L m_lock;
};

// ============================================================================
// class: WeakCounter
// This class is used by the WeakPtr pointer type
// ============================================================================
template <typename T, typename L>
class WeakCounter
{
  friend class SharedCounter<T,L>;

public:

  typedef WeakCounter<T,L> ThisType;
  
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
    this->m_count->add_ref();    
  }

  //! copy constructor
  WeakCounter (const SharedCounter<T,L>& cnt)
  {
    PTR_DBG("WeakCounter::WeakCounter(const SharedCounter<T,L>&)");
    this->m_count = cnt.m_count;
    this->m_count->add_weak_ref();
  }

  //! destructor
  virtual ~WeakCounter()
  {
    PTR_DBG("WeakCounter::~WeakCounter()");
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
  const T & use_count () const
  {
    return this->m_count ? this->m_count->use_count() : 0;
  }

  //- implicit conversion to bool
  typedef T* WeakCounter::*unspecified_bool_type;
  operator unspecified_bool_type() const
  {
    return this->m_count ? m_count->use_count() : 0;
  }

private:
  CountImpl<T,L>* m_count; // The shared counter
};

//! copy constructor
template <class T, class L>
SharedCounter<T, L>::SharedCounter (const WeakCounter<T,L>& cnt)
{
  PTR_DBG("SharedCounter::SharedCounter(const WeakCounter<T,L>&)");
  this->m_count->lock();
  if( cnt.m_count->use_count() > 0 )
  {
    this->m_count = cnt.m_count;
    this->m_count->add_ref();
  }
  this->m_count->unlock();
}

} // namespace

#endif



