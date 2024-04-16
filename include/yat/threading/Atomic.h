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

#ifdef YAT_HAS_C11ATOMIC
  #include <atomic>
#endif

#ifdef YAT_HAS_CSTDATOMIC
  #include <cstdatomic>
#endif

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Mutex.h>
#include <yat/utils/String.h>

namespace yat
{

// ============================================================================
//! class Atomic
// ============================================================================
template<typename T>
class Atomic
{
public:

  Atomic<T>()            { }
  Atomic<T>(const T& v)  { m_value = v; }
  Atomic<T>(const Atomic<T>& v)  { m_value = T(v); }

  operator const T() const
  {
    AutoMutex<> _lock(m_mtx);
    return m_value;
  }

  T operator=(const T& v)
  {
    AutoMutex<> _lock(m_mtx);
    m_value = v;
    return m_value;
  }

  T operator=(const Atomic<T>& v)
  {
    AutoMutex<> _lock(m_mtx);
    m_value = T(v);
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

#ifdef YAT_HAS_ATOMIC

typedef  std::atomic_bool atomic_bool;
typedef  std::atomic_int atomic_int;
typedef  std::atomic_char atomic_int8;
typedef  std::atomic_uchar atomic_uint8;
typedef  std::atomic_short atomic_int16;
typedef  std::atomic_ushort atomic_uint16;
typedef  std::atomic_int atomic_int32;
typedef  std::atomic_uint atomic_uint32;
typedef  std::atomic_llong atomic_int64;
typedef  std::atomic_ullong atomic_uint64;

#else

//! Atomic types
typedef Atomic<bool>   atomic_bool;
typedef Atomic<int>    atomic_int;
typedef Atomic<int8>   atomic_int8;
typedef Atomic<uint8>  atomic_uint8;
typedef Atomic<int16>  atomic_int16;
typedef Atomic<uint16> atomic_uint16;
typedef Atomic<int32>  atomic_int32;
typedef Atomic<uint32> atomic_uint32;
typedef Atomic<int64>  atomic_int64;
typedef Atomic<uint64> atomic_uint64;

#endif

#define __YAT_ATOMIC_FORMATTER__(T, dtype) \
template<> struct Format::Formatter<T> \
{ \
  void operator()(const T& v, std::ostream &oss, const Context& fc) \
  { \
    prepare_format(oss, fc); \
    if( 'B' == fc.type ) \
      output_binary_value(dtype(v), oss, fc); \
    else if( 'o' == fc.type ) \
      output_octal_value(dtype(v), oss, fc); \
    else if( 'x' == fc.type || 'X' == fc.type ) \
      output_hex_value(dtype(v), oss, fc); \
    else if( '%' == fc.type ) \
      output_percent_value(dtype(v), oss, fc); \
    else \
      oss << dtype(v); \
  } \
};

__YAT_ATOMIC_FORMATTER__(atomic_int8, int8)
__YAT_ATOMIC_FORMATTER__(atomic_uint8, uint8)
__YAT_ATOMIC_FORMATTER__(atomic_int16, int16)
__YAT_ATOMIC_FORMATTER__(atomic_uint16, uint16)
__YAT_ATOMIC_FORMATTER__(atomic_int32, int32)
__YAT_ATOMIC_FORMATTER__(atomic_uint32, uint32)
__YAT_ATOMIC_FORMATTER__(atomic_int64, int64)
__YAT_ATOMIC_FORMATTER__(atomic_uint64, uint64)

template<> struct Format::Formatter<atomic_bool>
{
  void operator()(const atomic_bool& v, std::ostream &oss, const Format::Context& fc)
  {
    prepare_format(oss, fc, 'b');
    oss << bool(v);
  }
};

} // yat

#endif //- _YAT_ATOMIC_H_
