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
// Copyright (C) 2006-2022 The Tango Community
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

#ifndef _YAT_ATOMIC_H_
#define _YAT_ATOMIC_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Mutex.h>

// ============================================================================
//! \class Atom<T>
//! \brief Non lock-free atomic storage.
//! Provide threadsafe access to bacic-type variables by avoiding the declaration
//! of lock object in user code
//!
//! \todo lock-free implementation (at least on linux)
//!
//! \notice The main use is for basic-type object
//!
//! \verbatim
//! yat::Atomint<int> ai = 1;
//! int i(ai);
//! int j = ai + i;
//! \endverbatim
// ============================================================================
template<typename T>
class Atomic
{
public:
  //! c-tor
  Atomic<T>()            { }
  Atomic<T>(const T& v)  { m_value = v; }

  //! Accessor, return a copy
  operator const T() const
  {
    AutoMutex<> _lock(m_mtx);
    return m_value;
  }

  //! Equal operator
  T operator=(const T& v)
  {
    AutoMutex<> _lock(m_mtx);
    m_value = v;
    return m_value;
  }

  T operator++()
  {
    AutoMutex<> _lock(m_mtx);
    return ++m_value;
  }

  T operator++(int)
  {
    AutoMutex<> _lock(m_mtx);
    return m_value++;
  }

  T operator--()
  {
    AutoMutex<> _lock(m_mtx);
    return --m_value;
  }

  T operator--(int)
  {
    AutoMutex<> _lock(m_mtx);
    return m_value--;
  }

  T operator+=(T v)
  {
    AutoMutex<> _lock(m_mtx);
    m_value += v;
    return m_value;
  }

  T operator-=(T v)
  {
    AutoMutex<> _lock(m_mtx);
    m_value -= v;
    return m_value;
  }

  T operator&=(T v)
  {
    AutoMutex<> _lock(m_mtx);
    m_value &= v;
    return m_value;
  }

  T operator|=(T v)
  {
    AutoMutex<> _lock(m_mtx);
    m_value |= v;
    return m_value;
  }

  T operator^=(T v)
  {
    AutoMutex<> _lock(m_mtx);
    m_value ^= v;
    return m_value;
  }

  T exchange(T desired)
  {
    AutoMutex<> _lock(m_mtx);
    std::swap(desired, m_value);
    return desired;
  }

  T load()
  {
    AutoMutex<> _lock(m_mtx);
    return m_value;
  }

  void store(T desired)
  {
    AutoMutex<> _lock(m_mtx);
    m_value = desired;
  }

  //! non lock-free implementation
  bool is_lock_free() { return false; }

private:
  T             m_value;
  mutable Mutex m_mtx;
};

//! Common types
typedef Atomic<bool>   atomic_bool;
typedef Atomic<int>    atomic_int;
typedef Atomic<int16>  atomic_int16;
typedef Atomic<uint16> atomic_uint16;
typedef Atomic<int32>  atomic_int32;
typedef Atomic<uint32> atomic_uint32;
typedef Atomic<int64>  atomic_int64;
typedef Atomic<uint64> atomic_uint64;
typedef Atomic<float>  atomic_float;
typedef Atomic<double> atomic_double;

} // yat

#endif //- _YAT_ATOMIC_H_
