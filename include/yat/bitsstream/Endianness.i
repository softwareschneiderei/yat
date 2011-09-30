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

namespace yat 
{

#if defined (YAT_HAS_INTRINSIC_BYTESWAP)
//- take advantage of MSVC++ byte swapping compiler intrinsics (found in <stdlib.h>)
# pragma intrinsic (_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)
#elif defined (YAT_HAS_BSWAP_16) || defined (YAT_HAS_BSWAP_32) || defined (YAT_HAS_BSWAP_64)
//- take advantage of local swapping implementation
# include <byteswap.h>
#endif

//- the number of bytes in a long
#include <limits.h>
#if ! defined (YAT_SIZEOF_LONG)
# if ((ULONG_MAX) == 65535UL)
#   define YAT_SIZEOF_LONG 2
# elif ((ULONG_MAX) == 4294967295UL)
#   define YAT_SIZEOF_LONG 4
# elif ((ULONG_MAX) == 18446744073709551615UL)
#   define YAT_SIZEOF_LONG 8
# else
#   error: unsupported long size, must be updated for this platform!
# endif
#endif

//=============================================================================
// Endianness::swap_2
//=============================================================================
inline void
Endianness::swap_2 (const char *orig, char* target)
{
#if defined (YAT_HAS_INTRINSIC_BYTESWAP)
  //- take advantage of MSVC++ compiler intrinsic byte swapping function
  *reinterpret_cast<unsigned short *> (target) =
    _byteswap_ushort (*reinterpret_cast<unsigned short const *> (orig));
#elif defined (YAT_HAS_BSWAP16)
  *reinterpret_cast<uint16_t *> (target) =
    bswap16 (*reinterpret_cast<uint16_t const *> (orig));
#elif defined(YAT_HAS_INTEL_ASSEMBLY)
  unsigned short a =
    *reinterpret_cast<const unsigned short*> (orig);
  asm( "rolw $8, %0" : "=r" (a) : "0" (a) );
  *reinterpret_cast<unsigned short*> (target) = a;
#elif defined (YAT_HAS_PENTIUM) && defined(_MSC_VER)
  __asm mov ebx, orig;
  __asm mov ecx, target;
  __asm mov ax, [ebx];
  __asm rol ax, 8;
  __asm mov [ecx], ax;
#else
  register yat::uint16 usrc = * reinterpret_cast<const yat::uint16*> (orig);
  register yat::uint16* udst = reinterpret_cast<yat::uint16*> (target);
  *udst = (usrc << 8) | (usrc >> 8);
#endif
}

//=============================================================================
// Endianness::swap_4
//=============================================================================
inline void
Endianness::swap_4 (const char* orig, char* target)
{
#if defined (YAT_HAS_INTRINSIC_BYTESWAP)
  //- take advantage of MSVC++ compiler intrinsic byte swapping function
  *reinterpret_cast<unsigned long *> (target) =
    _byteswap_ulong (*reinterpret_cast<unsigned long const *> (orig));
#elif defined (YAT_HAS_BSWAP32)
  *reinterpret_cast<uint32_t *> (target) =
    bswap32 (*reinterpret_cast<uint32_t const *> (orig));
#elif defined(YAT_HAS_INTEL_ASSEMBLY)
  //- we have YAT_HAS_PENTIUM, so we know the sizeof's.
  register unsigned int j =
    *reinterpret_cast<const unsigned int*> (orig);
  asm ("bswap %1" : "=r" (j) : "0" (j));
  *reinterpret_cast<unsigned int*> (target) = j;
#elif defined(YAT_HAS_PENTIUM) && defined(_MSC_VER)
  __asm mov ebx, orig;
  __asm mov ecx, target;
  __asm mov eax, [ebx];
  __asm bswap eax;
  __asm mov [ecx], eax;
#else
  register yat::uint32 x = * reinterpret_cast<const yat::uint32*> (orig);
  x = (x << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | (x >> 24);
  * reinterpret_cast<yat::uint32*> (target) = x;
#endif
}

//=============================================================================
// Endianness::swap_8
//=============================================================================
inline void
Endianness::swap_8 (const char* orig, char* target)
{
#if defined (YAT_HAS_INTRINSIC_BYTESWAP)
  //- take advantage of MSVC++ compiler intrinsic byte swapping function
  *reinterpret_cast<unsigned __int64 *> (target) =
    _byteswap_uint64 (*reinterpret_cast<unsigned __int64 const *> (orig));
#elif defined (YAT_HAS_BSWAP64)
  *reinterpret_cast<uint64_t *> (target) =
    bswap64 (*reinterpret_cast<uint64_t const *> (orig));
#elif (defined (__amd64__) || defined (__x86_64__)) && defined(__GNUG__)
  register unsigned long x =
    * reinterpret_cast<const unsigned long*> (orig);
  asm ("bswapq %1" : "=r" (x) : "0" (x));
  *reinterpret_cast<unsigned long*> (target) = x;
#elif defined(YAT_HAS_PENTIUM) && defined(__GNUG__)
  register unsigned int i =
    *reinterpret_cast<const unsigned int*> (orig);
  register unsigned int j =
    *reinterpret_cast<const unsigned int*> (orig + 4);
  asm ("bswap %1" : "=r" (i) : "0" (i));
  asm ("bswap %1" : "=r" (j) : "0" (j));
  *reinterpret_cast<unsigned int*> (target + 4) = i;
  *reinterpret_cast<unsigned int*> (target) = j;
#elif defined(YAT_HAS_PENTIUM) && defined(_MSC_VER)
  __asm mov ecx, orig;
  __asm mov edx, target;
  __asm mov eax, [ecx];
  __asm mov ebx, 4[ecx];
  __asm bswap eax;
  __asm bswap ebx;
  __asm mov 4[edx], eax;
  __asm mov [edx], ebx;
#elif YAT_SIZEOF_LONG == 8
  //- 64 bit architecture.
  register unsigned long x =
    * reinterpret_cast<const unsigned long*> (orig);
  register unsigned long x84 = (x & 0x000000ff000000ffUL) << 24;
  register unsigned long x73 = (x & 0x0000ff000000ff00UL) << 8;
  register unsigned long x62 = (x & 0x00ff000000ff0000UL) >> 8;
  register unsigned long x51 = (x & 0xff000000ff000000UL) >> 24;
  x = (x84 | x73 | x62 | x51);
  x = (x << 32) | (x >> 32);
  *reinterpret_cast<unsigned long*> (target) = x;
#else
  register yat::uint32 x =
    * reinterpret_cast<const yat::uint32*> (orig);
  register yat::uint32 y =
    * reinterpret_cast<const yat::uint32*> (orig + 4);
  x = (x << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | (x >> 24);
  y = (y << 24) | ((y & 0xff00) << 8) | ((y & 0xff0000) >> 8) | (y >> 24);
  * reinterpret_cast<yat::uint32*> (target) = y;
  * reinterpret_cast<yat::uint32*> (target + 4) = x;
#endif
}

//=============================================================================
// Endianness::swap_16
//=============================================================================
inline void
Endianness::swap_16 (const char* orig, char* target)
{
  Endianness::swap_8(orig + 8, target);
  Endianness::swap_8(orig, target + 8);
}

} //- namespace 

