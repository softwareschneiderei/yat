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
// DEPENDENCIES
//=============================================================================
#include <yat/bitsstream/Endianness.h>
#include <yat/bitsstream/BitsStream.h>

namespace yat 
{

//=============================================================================
// Global var. for BitsStream
//=============================================================================
YAT_DECL size_t kINDENT_COUNTER = 0;

#if defined(YAT_64BITS)
//=============================================================================
// structure to aid in masking bits (0 to 64 bits mask)
//=============================================================================
YAT_DECL yat::BitsStorage bit_masks[65] =
{
    0x00, 
    0x01,              0x03,              0x07,              0x0f,              0x1f,               0x3f,               0x7f,               0xff,
    0x1ff,             0x3ff,             0x7ff,             0xfff,             0x1fff,             0x3fff,             0x7fff,             0xffff,
    0x1ffff,           0x3ffff,           0x7ffff,           0xfffff,           0x1fffff,           0x3fffff,           0x7fffff,           0xffffff, 
    0x1ffffff,         0x3ffffff,         0x7ffffff,         0xfffffff,         0x1fffffff,         0x3fffffff,         0x7fffffff,         0xffffffff,
    0x1ffffffff,       0x3ffffffff,       0x7ffffffff,       0xfffffffff,       0x1fffffffff,       0x3fffffffff,       0x7fffffffff,       0xffffffffff,
    0x1ffffffffff,     0x3ffffffffff,     0x7ffffffffff,     0xfffffffffff,     0x1fffffffffff,     0x3fffffffffff,     0x7fffffffffff,     0xffffffffffff,
    0x1ffffffffffff,   0x3ffffffffffff,   0x7ffffffffffff,   0xfffffffffffff,   0x1fffffffffffff,   0x3fffffffffffff,   0x7fffffffffffff,   0xffffffffffffff,
    0x1ffffffffffffff, 0x3ffffffffffffff, 0x7ffffffffffffff, 0xfffffffffffffff, 0x1fffffffffffffff, 0x3fffffffffffffff, 0x7fffffffffffffff, 0xffffffffffffffff
};
#else
//=============================================================================
// structure to aid in masking bits (0 to 32 bits mask)
//=============================================================================
YAT_DECL yat::BitsStorage bit_masks[33] =
{
    0x00,   
    0x01,      0x03,      0x07,      0x0f,     0x1f,      0x3f,      0x7f,      0xff,
    0x1ff,     0x3ff,     0x7ff,     0xfff,    0x1fff,    0x3fff,    0x7fff,    0xffff,
    0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,  0x1fffff,  0x3fffff,  0x7fffff,  0xffffff, 
    0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff
};
#endif

//=============================================================================
// BitsStream::BitsStream
//=============================================================================
BitsStream::BitsStream (unsigned char * _data, 
                        size_t _size, 
                        const Endianness::ByteOrder& _endianness)
 : m_current_byte (0),
   m_bits_in_current_byte (0),
   m_ibuffer (_data),
   m_ibuffer_size (_size),
   m_ibuffer_ptr (0),
   m_endianness (_endianness)
{ 
  //- noop ctor
}

//=============================================================================
// BitsStream::~BitsStream
//=============================================================================
BitsStream::~BitsStream ()
{
  //- noop dtor
}

} //- namespace



