//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2011  N.Leclercq & The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// See AUTHORS file 
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
 * \author See AUTHORS file
 */

#ifndef _YAT_BITS_STREAM_H_
#define _YAT_BITS_STREAM_H_


//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <streambuf>
#include <iostream>
#include <bitset>
#if defined(WIN32) && ! defined(NOMINMAX)
# define NOMINMAX
#endif
#include <limits>

#include <yat/bitsstream/Endianness.h>

namespace yat 
{

//=============================================================================
// BitsStorage type
//=============================================================================
#if defined(YAT_64BITS)
  typedef yat::uint64 BitsStorage;
#else
  typedef yat::uint32 BitsStorage;
#endif

//=============================================================================
// A table of masks to ease 0 to {32 or 64} bits masking
//=============================================================================
#if defined(YAT_64BITS)
  extern YAT_DECL BitsStorage bit_masks[65];
#else
  extern YAT_DECL BitsStorage bit_masks[33];
#endif

//=============================================================================
// class: BitsStream 
//=============================================================================
class YAT_DECL BitsStream;

//=============================================================================
// class: BitsSet (set of bits abstraction)
//=============================================================================
template <size_t _n, typename T> 
class YAT_DECL BitsSet
{
public:
  //---------------------------------------------
  BitsSet (const T& _value = 0) 
  //---------------------------------------------
    : m_value (_value)
  {
    //- noop ctor
  }
  //---------------------------------------------
  ~BitsSet ()
  //---------------------------------------------
  {
    //- noop dtor
  }
  //---------------------------------------------
  BitsSet<_n, T> & operator= (const BitsSet<_n, T>& _src)
  //---------------------------------------------
  {
    if (this == &_src) return *this;
    m_value = _src.m_value;
    return *this;
  }
  //---------------------------------------------
  BitsSet<_n, T> & operator= (const T& _src)
  //---------------------------------------------
  {
    m_value = _src; 
    return *this;
  }
  //---------------------------------------------
  BitsSet<_n, T> & operator|= (const BitsSet<_n, T>& _src)
  //---------------------------------------------
  {
    m_value |= _src.m_value;
    return *this;
  }
  //---------------------------------------------
  BitsSet<_n, T> & operator|= (const T& _src)
  //---------------------------------------------
  {
    m_value |= _src; 
    return *this;
  }
  //---------------------------------------------
  BitsSet<_n, T> & operator&= (const BitsSet<_n, T>& _src)
  //---------------------------------------------
  {
    m_value &= _src.m_value;
    return *this;
  }
  //---------------------------------------------
  BitsSet<_n, T> & operator&= (const T& _src)
  //---------------------------------------------
  {
    m_value &= _src; 
    return *this;
  }
  //---------------------------------------------
  const T& value () const
  //---------------------------------------------
  {
    return m_value;
  }
  //---------------------------------------------
  T& value ()
  //---------------------------------------------
  {
    return m_value;
  }
  //---------------------------------------------
  const T& operator() () const
  //---------------------------------------------
  {
    return m_value;
  }
  //---------------------------------------------
  std::string to_string () const
  //---------------------------------------------
  {
#if defined(WIN32) && _HAS_CPP0X
# if defined (YAT_64BITS)
    std::bitset<_n> bs(static_cast<_ULONGLONG>(m_value));
# else
    std::bitset<_n> bs(static_cast<int>(m_value));
# endif
#else
    std::bitset<_n> bs(static_cast<unsigned long>(m_value));
#endif

#if ! defined(WIN32) && (__GNUC__ < 4)
#warning "BitsSet<_n, T>.to_string() doesn't compile properly using gcc 3.x - will print out <xxxxxxxxxxxxxxxx>"
    return "xxxxxxxx";
#else
    return bs.to_string();
#endif
  }

private:
  T m_value;
};

//=============================================================================
// class: BitsStream (input stream of bits abstraction)
//=============================================================================
class YAT_DECL BitsStream
{
public:
  BitsStream (unsigned char * _data = 0, 
              size_t _size = 0,
              const Endianness::ByteOrder& _endianness = Endianness::BO_LITTLE_ENDIAN);

  virtual ~BitsStream ();

  //-------------------------------------------------------------------------
  void input_data (unsigned char * _data, size_t _size)
  //-------------------------------------------------------------------------
  {
    if (_data)
    {
      m_ibuffer = _data;
      m_ibuffer_size = _size;
      m_ibuffer_ptr = 0;
      m_current_byte = 0;
      m_bits_in_current_byte = 0;
    }
  }

  //-------------------------------------------------------------------------
  bool read_bits (unsigned int _num_bits, yat::BitsStorage & _bits)
  //-------------------------------------------------------------------------
  {
    _bits = 0;
    return read_bits_i (_num_bits, &_bits);
  }

  //-------------------------------------------------------------------------
  bool skip_bits (unsigned int _num_bits)
  //-------------------------------------------------------------------------
  {
    return read_bits_i (_num_bits, 0);
  }

  //-------------------------------------------------------------------------
  bool peek (unsigned char& _bits) // TODO make const and inbbits mutable ?
  //-------------------------------------------------------------------------
  {
    _bits = 0;

    bool retval = true;

    if (m_ibuffer != 0)
    {
      if (8 > m_bits_in_current_byte)
      {
        if (m_ibuffer_ptr != m_ibuffer_size)
        {
          m_current_byte <<= 8;
          m_current_byte |= static_cast<yat::BitsStorage>(m_ibuffer[m_ibuffer_ptr++]);
          m_bits_in_current_byte += 8;
        }
        else 
        {
          m_current_byte = 0;
          m_bits_in_current_byte = 0;
          retval = false;
        }
      }

      if (retval)
        _bits = static_cast<unsigned char>(m_current_byte >> (m_bits_in_current_byte - 8));
    }
    return retval;
  }

  //-------------------------------------------------------------------------
  const Endianness::ByteOrder & endianness () const
  //-------------------------------------------------------------------------
  {
    return m_endianness;
  }

  //-------------------------------------------------------------------------
  bool is_big_endian () const
  //-------------------------------------------------------------------------
  {
    return m_endianness == Endianness::BO_BIG_ENDIAN;
  }

  //-------------------------------------------------------------------------
  bool is_little_endian () const
  //-------------------------------------------------------------------------
  {
    return m_endianness == Endianness::BO_LITTLE_ENDIAN;
  }

private:

  //- the <tmp> input buffer (use internally for bits reading)
  yat::BitsStorage m_current_byte;
  
  //- the current number of bits in m_current_byte
  unsigned int m_bits_in_current_byte;

  //- the data buffer
  unsigned char * m_ibuffer;

  //- the data buffer size
  size_t m_ibuffer_size;

  //- the current idx in the data buffer 
  size_t m_ibuffer_ptr;

  //- is the input stream big or little endian?
  const Endianness::ByteOrder m_endianness;

  //- this is how to extract the bits, or skip the bits if bits == 0
  //-------------------------------------------------------------------------
  inline bool read_bits_i (unsigned int _num_bits, yat::BitsStorage * _bits)
  //-------------------------------------------------------------------------
  {
    //**********************************************************************
    //                          IMPORTANT NOTE 
    //----------------------------------------------------------------------
    // WHATEVER IS THE INPUT ENDIANESS, THIS ALGO. PRODUCES BIG ENDIAN DATA 
    //**********************************************************************
    bool retval = true;

    if (m_ibuffer != 0) 
    {
      while (_num_bits > m_bits_in_current_byte)
      {
        if (_bits)
        {
          *_bits |= m_current_byte << (_num_bits - m_bits_in_current_byte);
        }

        _num_bits -= m_bits_in_current_byte;

        if (m_ibuffer_ptr != m_ibuffer_size)
        {
          m_current_byte = static_cast<yat::BitsStorage>(m_ibuffer[m_ibuffer_ptr++]);
          m_bits_in_current_byte = 8;
        }
        else
        {
          m_bits_in_current_byte = 0;
          m_current_byte = 0;
          retval = false;
          break;
        }
      }

      if (_num_bits > 0 && retval)
      {
        if (_bits)
        {
          *_bits |= m_current_byte >> (m_bits_in_current_byte - _num_bits);
        }

        m_current_byte &= yat::bit_masks[m_bits_in_current_byte - _num_bits];

        m_bits_in_current_byte -= _num_bits;
      }
    }
    return retval;
  }
};

//=============================================================================
// specialization of operator>> for bitset<n> with [0...{sizeof(long)*8}] bits 
//=============================================================================
#define _IBSTREAM_READFUNC(_n, _T) \
inline BitsStream& operator>> (BitsStream& _source, BitsSet<_n, _T>& _dest) \
{\
  yat::BitsStorage tmp = 0; \
  _source.read_bits(_n, tmp); \
  if (_source.endianness() == Endianness::BO_LITTLE_ENDIAN) \
  { \
    switch (sizeof(_T)) \
    { \
      case 2: \
      { \
        Endianness::swap_2(reinterpret_cast<const char*>(&tmp), \
                           reinterpret_cast<char*>(&tmp)); \
      } \
      break; \
      case 4: \
      { \
        Endianness::swap_4(reinterpret_cast<const char*>(&tmp), \
                           reinterpret_cast<char*>(&tmp)); \
      } \
      break; \
      case 8: \
      { \
        Endianness::swap_8(reinterpret_cast<const char*>(&tmp), \
                           reinterpret_cast<char*>(&tmp)); \
      } \
      break; \
      case 16: \
      { \
        Endianness::swap_16(reinterpret_cast<const char*>(&tmp), \
                            reinterpret_cast<char*>(&tmp)); \
      } \
      break; \
    } \
  } \
  _dest.value() = static_cast<_T>(tmp); \
  return _source ; \
} 
  
//-----------------------------------------------------------------------------
// specializations
//-----------------------------------------------------------------------------
_IBSTREAM_READFUNC( 1, bool)
_IBSTREAM_READFUNC( 8, char)
_IBSTREAM_READFUNC( 8, unsigned char)
_IBSTREAM_READFUNC(16, short)
_IBSTREAM_READFUNC(16, unsigned short)
_IBSTREAM_READFUNC(32, int)
_IBSTREAM_READFUNC(32, unsigned int)
#if ! defined(YAT_64BITS)
_IBSTREAM_READFUNC(32, long)
_IBSTREAM_READFUNC(32, unsigned long)
#elif defined(YAT_WIN64) 
_IBSTREAM_READFUNC(32, long)
_IBSTREAM_READFUNC(32, unsigned long)
_IBSTREAM_READFUNC(64, __int64)
_IBSTREAM_READFUNC(64, unsigned __int64)
#else
_IBSTREAM_READFUNC(64, long)
_IBSTREAM_READFUNC(64, unsigned long)
#endif
  
//=============================================================================
// operator>> for an array of elements
//=============================================================================
template <typename T>
BitsStream& operator>> (BitsStream& _source, T _dest[])
{
  for (size_t i = 0; i < sizeof(_dest)/sizeof(_dest[0]); ++i)
    _source >> _dest[i];
  return _source;
}
  
} //- namespace

#endif //- _BITS_STREAM_H_
