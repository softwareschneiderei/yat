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

//=============================================================================
// original authors..Aniruddha Gokhale, Carlos O'Ryan, ... (ACE lib)
// hackers...........N. Leclercq & J. Malik - SOLEIL
//=============================================================================
// The current implementation assumes that the host has  1-byte, 2-byte and 
// 4-byte integral types, and that it has single precision and double  precision 
// IEEE floats. Those assumptions are pretty  good these days, with Crays being
// the only known exception.
//=============================================================================

#ifndef  _YAT_ENDIANNESS_H_
#define  _YAT_ENDIANNESS_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat/CommonHeader.h>

namespace yat
{

// ============================================================================
// class: Endianness
// ============================================================================
class YAT_DECL Endianness
{
public:
  //- endianness
  typedef enum
  {
    BO_LITTLE_ENDIAN,
    BO_BIG_ENDIAN
  } ByteOrder;
  
  //- host bytes ordering
  static const Endianness::ByteOrder host_endianness;
  static const Endianness::ByteOrder not_host_endianness;

  //- bytes ordering required for host platform
  inline static bool data_needs_bytes_reordering (const Endianness::ByteOrder & _data_bo)
  {
    return _data_bo != Endianness::host_endianness;
  }

  //- byte swapping for scalar 
  static void swap_2  (const char *orig, char *target);
  static void swap_4  (const char *orig, char *target);
  static void swap_8  (const char *orig, char *target);
  static void swap_16 (const char *orig, char *target);

  //- byte swapping for array 
  static void swap_2_array  (const char *orig, char *target, size_t length);
  static void swap_4_array  (const char *orig, char *target, size_t length);
  static void swap_8_array  (const char *orig, char *target, size_t length);
  static void swap_16_array (const char *orig, char *target, size_t length);
};

} //- namespace 


#endif //- _ENDIANNESS_H_
