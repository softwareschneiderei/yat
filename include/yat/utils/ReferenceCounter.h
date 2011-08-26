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

// ============================================================================
// class: ReferenceCounter
// ============================================================================
template <typename T = unsigned long, typename L = yat::NullMutex>
class ReferenceCounter
{
public:

  //! constructor
  ReferenceCounter (const T& initial_value = 0)
    : m_count(initial_value)
  {}

  //! destructor
  virtual ~ReferenceCounter()
  {
    //-noop
  };

  //! increment the underlying counter
  const T & increment () 
  {
    yat::AutoMutex<L> guard(this->m_lock);
    this->m_count++;
    return this->m_count;
  }

  //! decrement the underlying counter
  const T & operator++  () 
  {
    yat::AutoMutex<L> guard(this->m_lock);
    this->m_count++;
    return this->m_count;
  }

  //! dencrement the underlying counter
  const T & decrement () 
  {
    yat::AutoMutex<L> guard(this->m_lock);
    this->m_count--;
    return this->m_count;
  }

  //! decrement the underlying counter
  const T & operator-- () 
  {
    yat::AutoMutex<L> guard(this->m_lock);
    this->m_count--;
    return this->m_count;
  }

  //! decrement the underlying counter
  bool operator== (const T& v) 
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return this->m_count == v;
  }

  //! unique
  bool unique ()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return this->m_count == 1;
  }

  //! use count
  const T & use_count ()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return this->m_count;
  }

  //! increment value
  const T & increment_value () const
  {
    return 1;
  }

  //- implicit conversion to bool
  typedef T* ReferenceCounter::*unspecified_bool_type;
  operator unspecified_bool_type() const
  {
    return this->m_count ? &m_count : 0;
  }

private:
  T m_count;
  L m_lock;
};

} // namespace

#endif



