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

#ifndef _STRING_TPP_
#define _STRING_TPP_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include  <yat/utils/String.h>

namespace yat
{

//----------------------------------------------------------------------------
// Format::output_percent_value
//----------------------------------------------------------------------------
template<typename T>
void Format::output_percent_value(const T& v, std::ostream &oss, const Format::Context& fc)
{
  if( 0 == fc.precision )
    oss << int(100. * v) << '%';
  else
    oss << 100. * v << '%';
}

//----------------------------------------------------------------------------
// Format::output_binary_value
//----------------------------------------------------------------------------
template<typename T>
void Format::output_binary_value(const T& v, std::ostream &oss, const Format::Context& fc)
{
  char setbit = '1';
  char unsetbit = '0';
  std::size_t length = 0, nbits = 8 * sizeof(T), padding = 0, base = 0;
  T vv = v;

  if( fc.show_base ) base = 2; // reserved for '0b'

  if( !(vv > 0) && vv != 0 ) // negative value test, without compiler warning!
  {
    setbit = '0';
    unsetbit = '1';
    vv = -v - 1; // Two's complement to invert bits
  }
  else if( !(0 == fc.width && fc.zero_padding) )
  {
    nbits = 0;
    for( std::size_t i = 0; vv > 0; ++i )
    {
      if( vv & 1 ) nbits = i + 1;
      vv = vv >> 1;
    }
    vv = v; // reset field value
  }
  if( nbits < 1 ) nbits = 1;
  if( fc.width > 0 && fc.zero_padding && fc.width + base > nbits )
    padding = fc.width - nbits - base;
  length = nbits + padding + base;
  char* sz = new char[length+1];
  sz[length] = 0;
  if( padding > 0 )
    std::memset(sz + base, '0', padding);
  std::memset(sz + base + padding, unsetbit, length - padding - base);
  if( fc.show_base ) { sz[0] = '0'; sz[1] = 'b'; }

  for( std::size_t idx = 0; vv && idx < nbits; ++idx )
  {
    if( vv & 1 ) sz[length - idx - 1] = setbit;
    vv = vv >> 1;
  }
  oss << sz;
  delete [] sz;
}

//----------------------------------------------------------------------------
// Format::output_octal_value
//----------------------------------------------------------------------------
template<typename T>
void Format::output_octal_value(const T& v, std::ostream &oss, const Format::Context& fc)
{
  static char odigits[8]  = {'0', '1', '2', '3', '4', '5', '6', '7'};
  static char ordigits[8] = {'7', '6', '5', '4', '3', '2', '1', '0'};
  char *digits_p = odigits;
  std::size_t length = 0, ndigits = 0, padding = 0, base = 0;
  T vv = v;
  bool negative = false;

  if( fc.show_base ) base = 2; // reserved for '0o'

  if( !(vv > 0) && vv != 0 ) // negative value test, without compiler warning!
  {
    negative = true;
    digits_p = ordigits;
    vv = -v - 1; // Two's complement to invert bits
  }

  if( negative || (0 == fc.width && fc.zero_padding) )
    ndigits = (sizeof(T) << 3) / 3 + 1;
  else
  {
    for( std::size_t i = 0; vv > 0; ++i )
    {
      if( vv & 1 || vv & 2 || vv & 4 ) ndigits = i + 1;
      vv = vv >> 3;
    }
  }
  // reset field value
  if( negative )
    vv = -v - 1;
  else
    vv = v;

  if( ndigits < 1 ) ndigits = 1;
  if( fc.width > 0 && fc.zero_padding && fc.width + base > ndigits )
    padding = fc.width - ndigits - base;
  length = ndigits + padding + base;
  char* sz = new char[length+1];
  sz[length] = 0;
  std::memset(sz, '0', length);
  if( negative ) ndigits--;
  if( fc.show_base )  { sz[0] = '0'; sz[1] = 'o'; }

  unsigned int o = 0, idx = 0;
  for( ; idx < ndigits; ++idx )
  {
    sz[length - idx - 1] = digits_p[vv & 7];
    vv = vv >> 3;
  }
  if( negative )
  {                               // last bit   // two last bits
    o = (sizeof(T) << 3) % 3 == 1 ? (vv & 1) + 6 : (vv & 3) + 4;
    sz[length - idx - 1] = digits_p[o];
  }
  oss << sz;
  delete [] sz;
}

//----------------------------------------------------------------------------
// Format::output_hex_value
//----------------------------------------------------------------------------
template<typename T>
void Format::output_hex_value(const T& v, std::ostream &oss, const Format::Context& fc)
{
  static char xdigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                             'a', 'b', 'c', 'd', 'e', 'f'};
  static char Xdigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                             'A', 'B', 'C', 'D', 'E', 'F'};
  static char xrdigits[16] = {'f', 'e', 'd', 'c', 'b', 'a',
                              '9', '8', '7', '6', '5', '4', '3', '2', '1', '0'};
  static char Xrdigits[16] = {'F', 'E', 'D', 'C', 'B', 'A',
                              '9', '8', '7', '6', '5', '4', '3', '2', '1', '0'};
  std::size_t length = 0, ndigits = (sizeof(T) << 1), padding = 0, base = 0;
  T vv = v;
  bool negative = false, uppercase = false;
  char *digits_p = xdigits;


  if( fc.show_base ) base = 2; // reserved for '0x' or '0X'

  if( !(vv > 0) && vv != 0 ) // negative value test, without compiler warning!
  {
    negative = true;
    vv = -v - 1; // Two's complement to invert bits
  }

  if( negative )
  {
    if( fc.type == 'X' )
    {
      digits_p = Xrdigits;
      uppercase = true;
    }
    else
      digits_p = xrdigits;
  }
  else
  {
    if( fc.type == 'X' )
    {
      digits_p = Xdigits;
      uppercase = true;
    }
  }

  if( !negative && !(0 == fc.width && fc.zero_padding) )
  {
    ndigits = 0;
    for( std::size_t i = 0; vv > 0; ++i )
    {
      if( vv & 1 || vv & 2 || vv & 4 || vv & 8 ) ndigits = i + 1;
      vv = vv >> 4;
    }
  }
  // reset field value
  if( negative )
    vv = -v - 1;
  else
    vv = v;

  if( ndigits < 1 ) ndigits = 1;
  if( fc.width > 0 && fc.zero_padding && fc.width + base > ndigits )
    padding = fc.width - ndigits - base;
  length = ndigits + padding + base;
  char* sz = new char[length+1];
  sz[length] = 0;
  if( padding > 0 )
    std::memset(sz, '0', base + padding);
  std::memset(sz + base + padding, negative ? (uppercase ? 'F' : 'f') : '0', length - padding - base);
  if( fc.show_base )
  {
     sz[0] = '0';
     sz[1] = 'x';
     if( uppercase )
       sz[1] = 'X';
   }

  for( std::size_t idx = 0; vv && idx < ndigits; ++idx )
  {
    sz[length - idx - 1] = digits_p[vv & 15];
    vv = vv >> 4;
  }
  oss << sz;
  delete [] sz;
}

} // namespace

#endif
