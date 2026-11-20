//----------------------------------------------------------------------------
// Copyright (c) 2004-2024 Synchrotron SOLEIL
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
#ifndef _YAT_FLOAT_H_
#define _YAT_FLOAT_H_

#include <yat/Portability.h>
#include <yat/utils/String.h>

namespace yat
{

//----------------------------------------------------------------------------
// Absolute equality test
//----------------------------------------------------------------------------
template<typename T>
inline bool _f_eq_(T a, T b)
{
  if( ::fabs(a-b) < std::numeric_limits<T>::epsilon() ) // work better near zero
    return true;

  if( ::fabs(a) > ::fabs(b) )
    return ::fabs(a-b) <= ( std::numeric_limits<T>::epsilon() * ::fabs(a) );
  else
    return ::fabs(b-a) <= ( std::numeric_limits<T>::epsilon() * ::fabs(b) );
}

//----------------------------------------------------------------------------
// Equality test with a given precision
//----------------------------------------------------------------------------
template<typename T>
inline bool _f_eq_p_(T a, T b, T p)
{
  p = ::fabs(p);

  if( ::fabs(a-b) < p ) // work better near zero
    return true;

  if( ::fabs(a) > ::fabs(b) )
    return ::fabs(a-b) <= ( p * ::fabs(a) );
  else
    return ::fabs(b-a) <= ( p * ::fabs(b) );
}

// forward decl
class Float64;

// because casting to integer truncate instead of rounding
// there might be some case where 1 for instance is stored as 0.9999999999999...
// using epsilon prevent from wrong cast conversion
#define __FLOAT_2_INTEGER__ \
  do { \
    if( m_f > 0 ) \
    { \
      return m_f + std::numeric_limits<float>::epsilon(); \
    } \
    else if ( m_f < 0) \
      return m_f - std::numeric_limits<float>::epsilon(); \
    else \
      return 0; \
  } while(0)

#define __DOUBLE_2_INTEGER__ \
  do { \
    if( m_f > 0 ) \
    { \
      return m_f + std::numeric_limits<double>::epsilon(); \
    } \
    else if ( m_f < 0)  \
      return m_f - std::numeric_limits<double>::epsilon(); \
    else \
      return 0; \
  } while(0)

//==============================================================================
// 32 bits floating point number with fuzzy comparaison in order to eliminate
// the comparison issue due to precision loss
//==============================================================================
class Float32
{
friend bool operator==(const Float64& x, const Float32& y);
friend bool operator==(const Float32& x, const Float32& y);
friend bool operator==(const Float32& x, const Float64& y);
friend bool operator==(const Float32& x, float f);
friend bool operator==(const Float32& x, double d);
friend bool operator==(const Float32& x, int16 i);
friend bool operator==(const Float32& x, uint16 ui);
friend bool operator==(const Float32& x, int32 i);
friend bool operator==(const Float32& x, uint32 ui);
friend bool operator==(const Float32& x, int64 i);
friend bool operator==(const Float32& x, uint64 ui);
friend bool operator==(float f, const Float32& y);
friend bool operator==(double d, const Float32& y);
friend bool operator==(int16 i, const Float32& y);
friend bool operator==(uint16 ui, const Float32& y);
friend bool operator==(int32 i, const Float32& y);
friend bool operator==(uint32 ui, const Float32& y);
friend bool operator==(int64 i, const Float32& y);
friend bool operator==(uint64 ui, const Float32& y);

friend bool operator!=(const Float64& x, const Float32& y);
friend bool operator!=(const Float32& x, const Float32& y);
friend bool operator!=(const Float32& x, const Float64& y);
friend bool operator!=(const Float32& x, float f);
friend bool operator!=(const Float32& x, double d);
friend bool operator!=(const Float32& x, int16 i);
friend bool operator!=(const Float32& x, uint16 ui);
friend bool operator!=(const Float32& x, int32 i);
friend bool operator!=(const Float32& x, uint32 ui);
friend bool operator!=(const Float32& x, int64 i);
friend bool operator!=(const Float32& x, uint64 ui);
friend bool operator!=(float f, const Float32& y);
friend bool operator!=(double d, const Float32& y);
friend bool operator!=(int16 i, const Float32& y);
friend bool operator!=(uint16 ui, const Float32& y);
friend bool operator!=(int32 i, const Float32& y);
friend bool operator!=(uint32 ui, const Float32& y);
friend bool operator!=(int64 i, const Float32& y);
friend bool operator!=(uint64 ui, const Float32& y);

friend bool operator<(const Float64& x, const Float32& y);
friend bool operator<(const Float32& x, const Float32& y);
friend bool operator<(const Float32& x, const Float64& y);
friend bool operator<(const Float32& x, float f);
friend bool operator<(const Float32& x, double d);
friend bool operator<(const Float32& x, int16 i);
friend bool operator<(const Float32& x, uint16 ui);
friend bool operator<(const Float32& x, int32 i);
friend bool operator<(const Float32& x, uint32 ui);
friend bool operator<(const Float32& x, int64 i);
friend bool operator<(const Float32& x, uint64 ui);
friend bool operator<(float f, const Float32& x);
friend bool operator<(double d, const Float32& x);
friend bool operator<(int16 i, const Float32& x);
friend bool operator<(uint16 ui, const Float32& x);
friend bool operator<(int32 i, const Float32& x);
friend bool operator<(uint32 ui, const Float32& x);
friend bool operator<(int64 i, const Float32& x);
friend bool operator<(uint64 ui, const Float32& x);

friend bool operator<=(const Float64& x, const Float32& y);
friend bool operator<=(const Float32& x, const Float32& y);
friend bool operator<=(const Float32& x, const Float64& y);
friend bool operator<=(const Float32& x, float f);
friend bool operator<=(const Float32& x, double d);
friend bool operator<=(const Float32& x, int16 i);
friend bool operator<=(const Float32& x, uint16 ui);
friend bool operator<=(const Float32& x, int32 i);
friend bool operator<=(const Float32& x, uint32 ui);
friend bool operator<=(const Float32& x, int64 i);
friend bool operator<=(const Float32& x, uint64 ui);
friend bool operator<=(float f, const Float32& x);
friend bool operator<=(double d, const Float32& x);
friend bool operator<=(int16 i, const Float32& x);
friend bool operator<=(uint16 ui, const Float32& x);
friend bool operator<=(int32 i, const Float32& x);
friend bool operator<=(uint32 ui, const Float32& x);
friend bool operator<=(int64 i, const Float32& x);
friend bool operator<=(uint64 ui, const Float32& x);

friend bool operator>(const Float64& x, const Float32& y);
friend bool operator>(const Float32& x, const Float32& y);
friend bool operator>(const Float32& x, const Float64& y);
friend bool operator>(const Float32& x, float f);
friend bool operator>(const Float32& x, double d);
friend bool operator>(const Float32& x, int16 i);
friend bool operator>(const Float32& x, uint16 ui);
friend bool operator>(const Float32& x, int32 i);
friend bool operator>(const Float32& x, uint32 ui);
friend bool operator>(const Float32& x, int64 i);
friend bool operator>(const Float32& x, uint64 ui);
friend bool operator>(float f, const Float32& x);
friend bool operator>(double d, const Float32& x);
friend bool operator>(int16 i, const Float32& x);
friend bool operator>(uint16 ui, const Float32& x);
friend bool operator>(int32 i, const Float32& x);
friend bool operator>(uint32 ui, const Float32& x);
friend bool operator>(int64 i, const Float32& x);
friend bool operator>(uint64 ui, const Float32& x);

friend bool operator>=(const Float64& x, const Float32& y);
friend bool operator>=(const Float32& x, const Float32& y);
friend bool operator>=(const Float32& x, const Float64& y);
friend bool operator>=(const Float32& x, float f);
friend bool operator>=(const Float32& x, double d);
friend bool operator>=(const Float32& x, int16 i);
friend bool operator>=(const Float32& x, uint16 ui);
friend bool operator>=(const Float32& x, int32 i);
friend bool operator>=(const Float32& x, uint32 ui);
friend bool operator>=(const Float32& x, int64 i);
friend bool operator>=(const Float32& x, uint64 ui);
friend bool operator>=(float f, const Float32& x);
friend bool operator>=(double d, const Float32& x);
friend bool operator>=(int16 i, const Float32& x);
friend bool operator>=(uint16 ui, const Float32& x);
friend bool operator>=(int32 i, const Float32& x);
friend bool operator>=(uint32 ui, const Float32& x);
friend bool operator>=(int64 i, const Float32& x);
friend bool operator>=(uint64 ui, const Float32& x);

public:
  /// c-tor
  Float32(float f) : m_f(f) {}

  /// default c-tor
  Float32() { m_f = 0.f; }

  /// explicit conversion
  operator float() const { return m_f; }
  operator yat::int16() const { __FLOAT_2_INTEGER__; }
  operator yat::int32() const { __FLOAT_2_INTEGER__; }
  operator yat::int64() const { __FLOAT_2_INTEGER__; }

  /// comparison with other
  /// \param f other number
  /// \param precision relative precision for comparison
  /// returns:
  ///   1 if > other
  ///   0 if == other
  ///  -1 if < other
  int comp(float other, float precision = std::numeric_limits<float>::epsilon()) const
  {
    if( _f_eq_p_(m_f, other, precision) )
      return 0;

    if( m_f < other )
      return -1;

    return 1;
  }

  /// Return the Not a Number value
  static Float32 nan() { return std::numeric_limits<float>::quiet_NaN(); }

  /// is a number or not
  bool is_nan() const { return std::isnan(m_f); }

  /// set as 'not a number'
  void set_nan() { m_f = nan(); }

private:
  float m_f;
};

//==============================================================================
// 64 bits floating point number with fuzzy comparaison in order to eliminate
// the comparison issue due to precision loss
//==============================================================================
class Float64
{
friend bool operator==(const Float32& x, const Float64& y);
friend bool operator==(const Float64& x, const Float64& y);
friend bool operator==(const Float64& x, const Float32& y);
friend bool operator==(const Float64& x, float f);
friend bool operator==(const Float64& x, double d);
friend bool operator==(const Float64& x, int32 i);
friend bool operator==(const Float64& x, uint32 ui);
friend bool operator==(const Float64& x, int16 i);
friend bool operator==(const Float64& x, uint16 ui);
friend bool operator==(const Float64& x, int64 i);
friend bool operator==(const Float64& x, uint64 ui);
friend bool operator==(float f, const Float64& y);
friend bool operator==(double d, const Float64& y);
friend bool operator==(int32 i, const Float64& y);
friend bool operator==(uint32 ui, const Float64& y);
friend bool operator==(int16 i, const Float64& y);
friend bool operator==(uint16 ui, const Float64& y);
friend bool operator==(int64 i, const Float64& y);
friend bool operator==(uint64 ui, const Float64& y);

friend bool operator!=(const Float32& x, const Float64& y);
friend bool operator!=(const Float64& x, const Float64& y);
friend bool operator!=(const Float64& x, const Float32& y);
friend bool operator!=(const Float64& x, float f);
friend bool operator!=(const Float64& x, double d);
friend bool operator!=(const Float64& x, int32 i);
friend bool operator!=(const Float64& x, uint32 ui);
friend bool operator!=(const Float64& x, int16 i);
friend bool operator!=(const Float64& x, uint16 ui);
friend bool operator!=(const Float64& x, int64 i);
friend bool operator!=(const Float64& x, uint64 ui);
friend bool operator!=(float f, const Float64& y);
friend bool operator!=(double d, const Float64& y);
friend bool operator!=(int32 i, const Float64& y);
friend bool operator!=(uint32 ui, const Float64& y);
friend bool operator!=(int16 i, const Float64& y);
friend bool operator!=(uint16 ui, const Float64& y);
friend bool operator!=(int64 i, const Float64& y);
friend bool operator!=(uint64 ui, const Float64& y);

friend bool operator<(const Float32& x, const Float64& y);
friend bool operator<(const Float64& x, const Float64& y);
friend bool operator<(const Float64& x, const Float32& y);
friend bool operator<(const Float64& x, float f);
friend bool operator<(const Float64& x, double d);
friend bool operator<(const Float64& x, int32 i);
friend bool operator<(const Float64& x, uint32 ui);
friend bool operator<(const Float64& x, int16 i);
friend bool operator<(const Float64& x, uint16 ui);
friend bool operator<(const Float64& x, int64 i);
friend bool operator<(const Float64& x, uint64 ui);
friend bool operator<(float f, const Float64& x);
friend bool operator<(double d, const Float64& x);
friend bool operator<(int32 i, const Float64& x);
friend bool operator<(uint32 ui, const Float64& x);
friend bool operator<(int16 i, const Float64& x);
friend bool operator<(uint16 ui, const Float64& x);
friend bool operator<(int64 i, const Float64& x);
friend bool operator<(uint64 ui, const Float64& x);

friend bool operator<=(const Float32& x, const Float64& y);
friend bool operator<=(const Float64& x, const Float64& y);
friend bool operator<=(const Float64& x, const Float32& y);
friend bool operator<=(const Float64& x, float f);
friend bool operator<=(const Float64& x, double d);
friend bool operator<=(const Float64& x, int32 i);
friend bool operator<=(const Float64& x, uint32 ui);
friend bool operator<=(const Float64& x, int16 i);
friend bool operator<=(const Float64& x, uint16 ui);
friend bool operator<=(const Float64& x, int64 i);
friend bool operator<=(const Float64& x, uint64 ui);
friend bool operator<=(float f, const Float64& x);
friend bool operator<=(double d, const Float64& x);
friend bool operator<=(int32 i, const Float64& x);
friend bool operator<=(uint32 ui, const Float64& x);
friend bool operator<=(int16 i, const Float64& x);
friend bool operator<=(uint16 ui, const Float64& x);
friend bool operator<=(int64 i, const Float64& x);
friend bool operator<=(uint64 ui, const Float64& x);

friend bool operator>(const Float32& x, const Float64& y);
friend bool operator>(const Float64& x, const Float64& y);
friend bool operator>(const Float64& x, const Float32& y);
friend bool operator>(const Float64& x, float f);
friend bool operator>(const Float64& x, double d);
friend bool operator>(const Float64& x, int32 i);
friend bool operator>(const Float64& x, uint32 ui);
friend bool operator>(const Float64& x, int16 i);
friend bool operator>(const Float64& x, uint16 ui);
friend bool operator>(const Float64& x, int64 i);
friend bool operator>(const Float64& x, uint64 ui);
friend bool operator>(float f, const Float64& x);
friend bool operator>(double d, const Float64& x);
friend bool operator>(int32 i, const Float64& x);
friend bool operator>(uint32 ui, const Float64& x);
friend bool operator>(int16 i, const Float64& x);
friend bool operator>(uint16 ui, const Float64& x);
friend bool operator>(int64 i, const Float64& x);
friend bool operator>(uint64 ui, const Float64& x);

friend bool operator>=(const Float32& x, const Float64& y);
friend bool operator>=(const Float64& x, const Float64& y);
friend bool operator>=(const Float64& x, const Float32& y);
friend bool operator>=(const Float64& x, float f);
friend bool operator>=(const Float64& x, double d);
friend bool operator>=(const Float64& x, int32 i);
friend bool operator>=(const Float64& x, uint32 ui);
friend bool operator>=(const Float64& x, int16 i);
friend bool operator>=(const Float64& x, uint16 ui);
friend bool operator>=(const Float64& x, int64 i);
friend bool operator>=(const Float64& x, uint64 ui);
friend bool operator>=(float f, const Float64& x);
friend bool operator>=(double d, const Float64& x);
friend bool operator>=(int32 i, const Float64& x);
friend bool operator>=(uint32 ui, const Float64& x);
friend bool operator>=(int16 i, const Float64& x);
friend bool operator>=(uint16 ui, const Float64& x);
friend bool operator>=(int64 i, const Float64& x);
friend bool operator>=(uint64 ui, const Float64& x);

public:
  /// c-tor
  Float64(double d) : m_f(d) {}

  /// default c-tor
  Float64() { m_f = 0.; }

  /// explicit conversions
  operator double() const { return m_f; }
  operator yat::int16() const { __DOUBLE_2_INTEGER__; }
  operator yat::int32() const { __DOUBLE_2_INTEGER__; }
  operator yat::int64() const { __DOUBLE_2_INTEGER__; }

  /// comparison with other
  /// f other number
  /// precision relative precision for comparison
  /// returns:
  ///   1 if > other
  ///   0 if == other
  ///  -1 if < other
  int comp(double other, double precision = std::numeric_limits<double>::epsilon()) const
  {
    if( _f_eq_p_(m_f, other, precision) )
      return 0;

    if( m_f < other )
      return -1;

    return 1;
  }

  /// Return the Not a Number value
  static Float64 nan() { return std::numeric_limits<double>::quiet_NaN(); }

  /// is a number or not
  bool is_nan() const { return std::isnan(m_f); }

  /// set as 'not a number'
  void set_nan() { m_f = nan(); }

private:
  double m_f;
};

bool operator==(const Float32& x, const Float32& y) { return _f_eq_(x.m_f, y.m_f); }
bool operator==(const Float32& x, const Float64& y) { return _f_eq_(double(x.m_f), y.m_f); }
bool operator==(const Float32& x, float f)          { return _f_eq_(x.m_f, f); }
bool operator==(const Float32& x, double d)         { return _f_eq_(double(x.m_f), d); }
bool operator==(const Float32& x, int16 i)          { return _f_eq_(x.m_f, float(i)); }
bool operator==(const Float32& x, uint16 ui)        { return _f_eq_(x.m_f, float(ui)); }
bool operator==(const Float32& x, int32 i)          { return _f_eq_(x.m_f, float(i)); }
bool operator==(const Float32& x, uint32 ui)        { return _f_eq_(x.m_f, float(ui)); }
bool operator==(const Float32& x, int64 i)          { return _f_eq_(x.m_f, float(i)); }
bool operator==(const Float32& x, uint64 ui)        { return _f_eq_(x.m_f, float(ui)); }
bool operator==(const Float64& x, const Float64& y) { return _f_eq_(x.m_f, y.m_f); }
bool operator==(const Float64& x, const Float32& y) { return _f_eq_(x.m_f, double(y.m_f)); }
bool operator==(const Float64& x, float f)          { return _f_eq_(x.m_f, double(f)); }
bool operator==(const Float64& x, double d)         { return _f_eq_(x.m_f, d); }
bool operator==(const Float64& x, int32 i)          { return _f_eq_(x.m_f, double(i)); }
bool operator==(const Float64& x, uint32 ui)        { return _f_eq_(x.m_f, double(ui)); }
bool operator==(const Float64& x, int16 i)          { return _f_eq_(x.m_f, double(i)); }
bool operator==(const Float64& x, uint16 ui)        { return _f_eq_(x.m_f, double(ui)); }
bool operator==(const Float64& x, int64 i)          { return _f_eq_(x.m_f, double(i)); }
bool operator==(const Float64& x, uint64 ui)        { return _f_eq_(x.m_f, double(ui)); }
bool operator==(float f, const Float32& y)          { return _f_eq_(f, y.m_f); }
bool operator==(double d, const Float32& y)         { return _f_eq_(d, double(y.m_f)); }
bool operator==(int16 i, const Float32& y)          { return _f_eq_(float(i), y.m_f); }
bool operator==(uint16 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f); }
bool operator==(int32 i, const Float32& y)          { return _f_eq_(float(i), y.m_f); }
bool operator==(uint32 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f); }
bool operator==(int64 i, const Float32& y)          { return _f_eq_(float(i), y.m_f); }
bool operator==(uint64 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f); }
bool operator==(float f, const Float64& y)          { return _f_eq_(double(f), y.m_f); }
bool operator==(double d, const Float64& y)         { return _f_eq_(d, y.m_f); }
bool operator==(int32 i, const Float64& y)          { return _f_eq_(double(i), y.m_f); }
bool operator==(uint32 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f); }
bool operator==(int16 i, const Float64& y)          { return _f_eq_(double(i), y.m_f); }
bool operator==(uint16 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f); }
bool operator==(int64 i, const Float64& y)          { return _f_eq_(double(i), y.m_f); }
bool operator==(uint64 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f); }

bool operator!=(const Float32& x, const Float32& y) { return !_f_eq_(x.m_f, y.m_f); }
bool operator!=(const Float32& x, const Float64& y) { return !_f_eq_(double(x.m_f), y.m_f); }
bool operator!=(const Float32& x, float f)          { return !_f_eq_(x.m_f, f); }
bool operator!=(const Float32& x, double d)         { return !_f_eq_(double(x.m_f), d); }
bool operator!=(const Float32& x, int16 i)          { return !_f_eq_(x.m_f, float(i)); }
bool operator!=(const Float32& x, uint16 ui)        { return !_f_eq_(x.m_f, float(ui)); }
bool operator!=(const Float32& x, int32 i)          { return !_f_eq_(x.m_f, float(i)); }
bool operator!=(const Float32& x, uint32 ui)        { return !_f_eq_(x.m_f, float(ui)); }
bool operator!=(const Float32& x, int64 i)          { return !_f_eq_(x.m_f, float(i)); }
bool operator!=(const Float32& x, uint64 ui)        { return !_f_eq_(x.m_f, float(ui)); }
bool operator!=(const Float64& x, const Float64& y) { return !_f_eq_(x.m_f, y.m_f); }
bool operator!=(const Float64& x, const Float32& y) { return !_f_eq_(x.m_f, double(y.m_f)); }
bool operator!=(const Float64& x, float f)          { return !_f_eq_(x.m_f, double(f)); }
bool operator!=(const Float64& x, double d)         { return !_f_eq_(x.m_f, d); }
bool operator!=(const Float64& x, int32 i)          { return !_f_eq_(x.m_f, double(i)); }
bool operator!=(const Float64& x, uint32 ui)        { return !_f_eq_(x.m_f, double(ui)); }
bool operator!=(const Float64& x, int16 i)          { return !_f_eq_(x.m_f, double(i)); }
bool operator!=(const Float64& x, uint16 ui)        { return !_f_eq_(x.m_f, double(ui)); }
bool operator!=(const Float64& x, int64 i)          { return !_f_eq_(x.m_f, double(i)); }
bool operator!=(const Float64& x, uint64 ui)        { return !_f_eq_(x.m_f, double(ui)); }
bool operator!=(float f, const Float32& y)          { return !_f_eq_(f, y.m_f); }
bool operator!=(double d, const Float32& y)         { return !_f_eq_(d, double(y.m_f)); }
bool operator!=(int16 i, const Float32& y)          { return !_f_eq_(float(i), y.m_f); }
bool operator!=(uint16 ui, const Float32& y)        { return !_f_eq_(float(ui), y.m_f); }
bool operator!=(int32 i, const Float32& y)          { return !_f_eq_(float(i), y.m_f); }
bool operator!=(uint32 ui, const Float32& y)        { return !_f_eq_(float(ui), y.m_f); }
bool operator!=(int64 i, const Float32& y)          { return !_f_eq_(float(i), y.m_f); }
bool operator!=(uint64 ui, const Float32& y)        { return !_f_eq_(float(ui), y.m_f); }
bool operator!=(float f, const Float64& y)          { return !_f_eq_(double(f), y.m_f); }
bool operator!=(double d, const Float64& y)         { return !_f_eq_(d, y.m_f); }
bool operator!=(int32 i, const Float64& y)          { return !_f_eq_(double(i), y.m_f); }
bool operator!=(uint32 ui, const Float64& y)        { return !_f_eq_(double(ui), y.m_f); }
bool operator!=(int16 i, const Float64& y)          { return !_f_eq_(double(i), y.m_f); }
bool operator!=(uint16 ui, const Float64& y)        { return !_f_eq_(double(ui), y.m_f); }
bool operator!=(int64 i, const Float64& y)          { return !_f_eq_(double(i), y.m_f); }
bool operator!=(uint64 ui, const Float64& y)        { return !_f_eq_(double(ui), y.m_f); }

bool operator<=(const Float32& x, const Float32& y) { return _f_eq_(x.m_f, y.m_f) ? true : x.m_f < y.m_f; }
bool operator<=(const Float32& x, const Float64& y) { return _f_eq_(double(x.m_f), y.m_f) ? true : x.m_f < y.m_f; }
bool operator<=(const Float32& x, float f)          { return _f_eq_(x.m_f, f) ? true : x.m_f < f; }
bool operator<=(const Float32& x, double d)         { return _f_eq_(double(x.m_f), d) ? true : x.m_f < d; }
bool operator<=(const Float32& x, int16 i)          { return _f_eq_(x.m_f, float(i)) ? true : x.m_f < i; }
bool operator<=(const Float32& x, uint16 ui)        { return _f_eq_(x.m_f, float(ui)) ? true : x.m_f < ui; }
bool operator<=(const Float32& x, int32 i)          { return _f_eq_(x.m_f, float(i)) ? true : x.m_f < i; }
bool operator<=(const Float32& x, uint32 ui)        { return _f_eq_(x.m_f, float(ui)) ? true : x.m_f < ui; }
bool operator<=(const Float32& x, int64 i)          { return _f_eq_(x.m_f, float(i)) ? true : x.m_f < i; }
bool operator<=(const Float32& x, uint64 ui)        { return _f_eq_(x.m_f, float(ui)) ? true : x.m_f < ui; }
bool operator<=(const Float64& x, const Float64& y) { return _f_eq_(x.m_f, y.m_f) ? true : x.m_f < y.m_f; }
bool operator<=(const Float64& x, const Float32& y) { return _f_eq_(x.m_f, double(y.m_f)) ? true : x.m_f < y.m_f; }
bool operator<=(const Float64& x, float f)          { return _f_eq_(x.m_f, double(f)) ? true : x.m_f < f; }
bool operator<=(const Float64& x, double d)         { return _f_eq_(x.m_f, d) ? true : x.m_f < d; }
bool operator<=(const Float64& x, int32 i)          { return _f_eq_(x.m_f, double(i)) ? true : x.m_f < i; }
bool operator<=(const Float64& x, uint32 ui)        { return _f_eq_(x.m_f, double(ui)) ? true : x.m_f < ui; }
bool operator<=(const Float64& x, int16 i)          { return _f_eq_(x.m_f, double(i)) ? true : x.m_f < i; }
bool operator<=(const Float64& x, uint16 ui)        { return _f_eq_(x.m_f, double(ui)) ? true : x.m_f < ui; }
bool operator<=(const Float64& x, int64 i)          { return _f_eq_(x.m_f, double(i)) ? true : x.m_f < i; }
bool operator<=(const Float64& x, uint64 ui)        { return _f_eq_(x.m_f, double(ui)) ? true : x.m_f < ui; }
bool operator<=(float f, const Float32& y)          { return _f_eq_(f, y.m_f) ? true : f < y.m_f; }
bool operator<=(double d, const Float32& y)         { return _f_eq_(d, double(y.m_f)) ? true : d < y.m_f; }
bool operator<=(int16 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? true : i < y.m_f; }
bool operator<=(uint16 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? true : ui < y.m_f; }
bool operator<=(int32 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? true : i < y.m_f; }
bool operator<=(uint32 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? true : ui < y.m_f; }
bool operator<=(int64 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? true : i < y.m_f; }
bool operator<=(uint64 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? true : ui < y.m_f; }
bool operator<=(float f, const Float64& y)          { return _f_eq_(double(f), y.m_f) ? true : f < y.m_f; }
bool operator<=(double d, const Float64& y)         { return _f_eq_(d, y.m_f) ? true : d < y.m_f; }
bool operator<=(int32 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? true : i < y.m_f; }
bool operator<=(uint32 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? true : ui < y.m_f; }
bool operator<=(int16 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? true : i < y.m_f; }
bool operator<=(uint16 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? true : ui < y.m_f; }
bool operator<=(int64 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? true : i < y.m_f; }
bool operator<=(uint64 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? true : ui < y.m_f; }

bool operator>=(const Float32& x, const Float32& y) { return _f_eq_(x.m_f, y.m_f) ? true : x.m_f > y.m_f; }
bool operator>=(const Float32& x, const Float64& y) { return _f_eq_(double(x.m_f), y.m_f) ? true : x.m_f > y.m_f; }
bool operator>=(const Float32& x, float f)          { return _f_eq_(x.m_f, f) ? true : x.m_f > f; }
bool operator>=(const Float32& x, double d)         { return _f_eq_(double(x.m_f), d) ? true : x.m_f > d; }
bool operator>=(const Float32& x, int16 i)          { return _f_eq_(x.m_f, float(i)) ? true : x.m_f > i; }
bool operator>=(const Float32& x, uint16 ui)        { return _f_eq_(x.m_f, float(ui)) ? true : x.m_f > ui; }
bool operator>=(const Float32& x, int32 i)          { return _f_eq_(x.m_f, float(i)) ? true : x.m_f > i; }
bool operator>=(const Float32& x, uint32 ui)        { return _f_eq_(x.m_f, float(ui)) ? true : x.m_f > ui; }
bool operator>=(const Float32& x, int64 i)          { return _f_eq_(x.m_f, float(i)) ? true : x.m_f > i; }
bool operator>=(const Float32& x, uint64 ui)        { return _f_eq_(x.m_f, float(ui)) ? true : x.m_f > ui; }
bool operator>=(const Float64& x, const Float64& y) { return _f_eq_(x.m_f, y.m_f) ? true : x.m_f > y.m_f; }
bool operator>=(const Float64& x, const Float32& y) { return _f_eq_(x.m_f, double(y.m_f)) ? true : x.m_f > y.m_f; }
bool operator>=(const Float64& x, float f)          { return _f_eq_(x.m_f, double(f)) ? true : x.m_f > f; }
bool operator>=(const Float64& x, double d)         { return _f_eq_(x.m_f, d) ? true : x.m_f > d; }
bool operator>=(const Float64& x, int32 i)          { return _f_eq_(x.m_f, double(i)) ? true : x.m_f > i; }
bool operator>=(const Float64& x, uint32 ui)        { return _f_eq_(x.m_f, double(ui)) ? true : x.m_f > ui; }
bool operator>=(const Float64& x, int16 i)          { return _f_eq_(x.m_f, double(i)) ? true : x.m_f > i; }
bool operator>=(const Float64& x, uint16 ui)        { return _f_eq_(x.m_f, double(ui)) ? true : x.m_f > ui; }
bool operator>=(const Float64& x, int64 i)          { return _f_eq_(x.m_f, double(i)) ? true : x.m_f > i; }
bool operator>=(const Float64& x, uint64 ui)        { return _f_eq_(x.m_f, double(ui)) ? true : x.m_f > ui; }
bool operator>=(float f, const Float32& y)          { return _f_eq_(f, y.m_f) ? true : f > y.m_f; }
bool operator>=(double d, const Float32& y)         { return _f_eq_(d, double(y.m_f)) ? true : d > y.m_f; }
bool operator>=(int16 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? true : i > y.m_f; }
bool operator>=(uint16 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? true : ui > y.m_f; }
bool operator>=(int32 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? true : i > y.m_f; }
bool operator>=(uint32 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? true : ui > y.m_f; }
bool operator>=(int64 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? true : i > y.m_f; }
bool operator>=(uint64 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? true : ui > y.m_f; }
bool operator>=(float f, const Float64& y)          { return _f_eq_(double(f), y.m_f) ? true : f > y.m_f; }
bool operator>=(double d, const Float64& y)         { return _f_eq_(d, y.m_f) ? true : d > y.m_f; }
bool operator>=(int32 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? true : i > y.m_f; }
bool operator>=(uint32 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? true : ui > y.m_f; }
bool operator>=(int16 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? true : i > y.m_f; }
bool operator>=(uint16 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? true : ui > y.m_f; }
bool operator>=(int64 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? true : i > y.m_f; }
bool operator>=(uint64 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? true : ui > y.m_f; }

bool operator<(const Float32& x, const Float32& y) { return _f_eq_(x.m_f, y.m_f) ? false : x.m_f < y.m_f; }
bool operator<(const Float32& x, const Float64& y) { return _f_eq_(double(x.m_f), y.m_f) ? false : x.m_f < y.m_f; }
bool operator<(const Float32& x, float f)          { return _f_eq_(x.m_f, f) ? false : x.m_f < f; }
bool operator<(const Float32& x, double d)         { return _f_eq_(double(x.m_f), d) ? false : x.m_f < d; }
bool operator<(const Float32& x, int16 i)          { return _f_eq_(x.m_f, float(i)) ? false : x.m_f < i; }
bool operator<(const Float32& x, uint16 ui)        { return _f_eq_(x.m_f, float(ui)) ? false : x.m_f < ui; }
bool operator<(const Float32& x, int32 i)          { return _f_eq_(x.m_f, float(i)) ? false : x.m_f < i; }
bool operator<(const Float32& x, uint32 ui)        { return _f_eq_(x.m_f, float(ui)) ? false : x.m_f < ui; }
bool operator<(const Float32& x, int64 i)          { return _f_eq_(x.m_f, float(i)) ? false : x.m_f < i; }
bool operator<(const Float32& x, uint64 ui)        { return _f_eq_(x.m_f, float(ui)) ? false : x.m_f < ui; }
bool operator<(const Float64& x, const Float64& y) { return _f_eq_(x.m_f, y.m_f) ? false : x.m_f < y.m_f; }
bool operator<(const Float64& x, const Float32& y) { return _f_eq_(x.m_f, double(y.m_f)) ? false : x.m_f < y.m_f; }
bool operator<(const Float64& x, float f)          { return _f_eq_(x.m_f, double(f)) ? false : x.m_f < f; }
bool operator<(const Float64& x, double d)         { return _f_eq_(x.m_f, d) ? false : x.m_f < d; }
bool operator<(const Float64& x, int32 i)          { return _f_eq_(x.m_f, double(i)) ? false : x.m_f < i; }
bool operator<(const Float64& x, uint32 ui)        { return _f_eq_(x.m_f, double(ui)) ? false : x.m_f < ui; }
bool operator<(const Float64& x, int16 i)          { return _f_eq_(x.m_f, double(i)) ? false : x.m_f < i; }
bool operator<(const Float64& x, uint16 ui)        { return _f_eq_(x.m_f, double(ui)) ? false : x.m_f < ui; }
bool operator<(const Float64& x, int64 i)          { return _f_eq_(x.m_f, double(i)) ? false : x.m_f < i; }
bool operator<(const Float64& x, uint64 ui)        { return _f_eq_(x.m_f, double(ui)) ? false : x.m_f < ui; }
bool operator<(float f, const Float32& y)          { return _f_eq_(f, y.m_f) ? false : f < y.m_f; }
bool operator<(double d, const Float32& y)         { return _f_eq_(d, double(y.m_f)) ? false : d < y.m_f; }
bool operator<(int16 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? false : i < y.m_f; }
bool operator<(uint16 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? false : ui < y.m_f; }
bool operator<(int32 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? false : i < y.m_f; }
bool operator<(uint32 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? false : ui < y.m_f; }
bool operator<(int64 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? false : i < y.m_f; }
bool operator<(uint64 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? false : ui < y.m_f; }
bool operator<(float f, const Float64& y)          { return _f_eq_(double(f), y.m_f) ? false : f < y.m_f; }
bool operator<(double d, const Float64& y)         { return _f_eq_(d, y.m_f) ? false : d < y.m_f; }
bool operator<(int32 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? false : i < y.m_f; }
bool operator<(uint32 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? false : ui < y.m_f; }
bool operator<(int16 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? false : i < y.m_f; }
bool operator<(uint16 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? false : ui < y.m_f; }
bool operator<(int64 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? false : i < y.m_f; }
bool operator<(uint64 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? false : ui < y.m_f; }

bool operator>(const Float32& x, const Float32& y) { return _f_eq_(x.m_f, y.m_f) ? false : x.m_f > y.m_f; }
bool operator>(const Float32& x, const Float64& y) { return _f_eq_(double(x.m_f), y.m_f) ? false : x.m_f > y.m_f; }
bool operator>(const Float32& x, float f)          { return _f_eq_(x.m_f, f) ? false : x.m_f > f; }
bool operator>(const Float32& x, double d)         { return _f_eq_(double(x.m_f), d) ? false : x.m_f > d; }
bool operator>(const Float32& x, int16 i)          { return _f_eq_(x.m_f, float(i)) ? false : x.m_f > i; }
bool operator>(const Float32& x, uint16 ui)        { return _f_eq_(x.m_f, float(ui)) ? false : x.m_f > ui; }
bool operator>(const Float32& x, int32 i)          { return _f_eq_(x.m_f, float(i)) ? false : x.m_f > i; }
bool operator>(const Float32& x, uint32 ui)        { return _f_eq_(x.m_f, float(ui)) ? false : x.m_f > ui; }
bool operator>(const Float32& x, int64 i)          { return _f_eq_(x.m_f, float(i)) ? false : x.m_f > i; }
bool operator>(const Float32& x, uint64 ui)        { return _f_eq_(x.m_f, float(ui)) ? false : x.m_f > ui; }
bool operator>(const Float64& x, const Float64& y) { return _f_eq_(x.m_f, y.m_f) ? false : x.m_f > y.m_f; }
bool operator>(const Float64& x, const Float32& y) { return _f_eq_(x.m_f, double(y.m_f)) ? false : x.m_f > y.m_f; }
bool operator>(const Float64& x, float f)          { return _f_eq_(x.m_f, double(f)) ? false : x.m_f > f; }
bool operator>(const Float64& x, double d)         { return _f_eq_(x.m_f, d) ? false : x.m_f > d; }
bool operator>(const Float64& x, int32 i)          { return _f_eq_(x.m_f, double(i)) ? false : x.m_f > i; }
bool operator>(const Float64& x, uint32 ui)        { return _f_eq_(x.m_f, double(ui)) ? false : x.m_f > ui; }
bool operator>(const Float64& x, int16 i)          { return _f_eq_(x.m_f, double(i)) ? false : x.m_f > i; }
bool operator>(const Float64& x, uint16 ui)        { return _f_eq_(x.m_f, double(ui)) ? false : x.m_f > ui; }
bool operator>(const Float64& x, int64 i)          { return _f_eq_(x.m_f, double(i)) ? false : x.m_f > i; }
bool operator>(const Float64& x, uint64 ui)        { return _f_eq_(x.m_f, double(ui)) ? false : x.m_f > ui; }
bool operator>(float f, const Float32& y)          { return _f_eq_(f, y.m_f) ? false : f > y.m_f; }
bool operator>(double d, const Float32& y)         { return _f_eq_(d, double(y.m_f)) ? false : d > y.m_f; }
bool operator>(int16 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? false : i > y.m_f; }
bool operator>(uint16 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? false : ui > y.m_f; }
bool operator>(int32 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? false : i > y.m_f; }
bool operator>(uint32 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? false : ui > y.m_f; }
bool operator>(int64 i, const Float32& y)          { return _f_eq_(float(i), y.m_f) ? false : i > y.m_f; }
bool operator>(uint64 ui, const Float32& y)        { return _f_eq_(float(ui), y.m_f) ? false : ui > y.m_f; }
bool operator>(float f, const Float64& y)          { return _f_eq_(double(f), y.m_f) ? false : f > y.m_f; }
bool operator>(double d, const Float64& y)         { return _f_eq_(d, y.m_f) ? false : d > y.m_f; }
bool operator>(int32 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? false : i > y.m_f; }
bool operator>(uint32 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? false : ui > y.m_f; }
bool operator>(int16 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? false : i > y.m_f; }
bool operator>(uint16 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? false : ui > y.m_f; }
bool operator>(int64 i, const Float64& y)          { return _f_eq_(double(i), y.m_f) ? false : i > y.m_f; }
bool operator>(uint64 ui, const Float64& y)        { return _f_eq_(double(ui), y.m_f) ? false : ui > y.m_f; }

//==================================================================================================
//! \struct Format::Formatter<Float32>
//! \brief for using with yat::Format class
//==================================================================================================
template<>
struct Format::Formatter<Float32>
{
  void operator()(const Float32& f, std::ostream &oss, const Format::Context& fc)
  {
    if( f.is_nan() )
      oss << "NaN";
    else
      oss << float(f);
  }
};

//==================================================================================================
//! \struct Format::Formatter<Float64>
//! \brief for using with yat::Format class
//==================================================================================================
template<>
struct Format::Formatter<Float64>
{
  void operator()(const Float64& f, std::ostream &oss, const Format::Context& fc)
  {
    if( f.is_nan() )
      oss << "NaN";
    else
      oss << double(f);
  }
};


} // namespace

#endif
