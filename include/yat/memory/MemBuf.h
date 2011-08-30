//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2011  The Tango Community
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
 * \author S.Poirier - Synchrotron SOLEIL
 */


#ifndef __YAT_MEMBUF_H__
#define __YAT_MEMBUF_H__

#include <yat/CommonHeader.h>

namespace yat
{

// CRC computation
uint32 crc( const byte *pBuf, uint32 uiLen, uint32 *pulInitValue = NULL );

//===========================================================================
// endian management
//===========================================================================

inline void invert_short(short *pS)
{
  // AB -> BA
  char *p = (char*)pS;
  char c = *p;
  *p = *(p+1);
  *(p+1) = c;
}

inline void invert_long(long *pL)
{
  // ABCD -> DCBA
  char *p = (char*)pL;
  char c = *p;
  *p = *(p+3);
  *(p+3) = c;
  c = *(p+1);
  *(p+1) = *(p+2);
  *(p+2) = c;
}

inline void invert_float(float *pF)
{
  // ABCD -> DCBA
  invert_long((long*)pF);
}

inline void invert_double(double *pD)
{
  // ABCDEFGH -> HGFEDCBA
  char *p = (char*)pD;
  char c = *p;
  *p = *(p+7);
  *(p+7) = c;

  c = *(p+1);
  *(p+1) = *(p+6);
  *(p+6) = c;

  c = *(p+2);
  *(p+2) = *(p+5);
  *(p+5) = c;

  c = *(p+3);
  *(p+3) = *(p+4);
  *(p+4) = c;
}

inline void invert_int64(int64 *pi64)
{
  // ABCDEFGH -> HGFEDCBA
  invert_double((double*)pi64);
}

//===================================================================
// Constantes
//===================================================================

// Latest crypying version
#define REVCRYPT_LATEST_VERSION -1
#define VARLENGTH_CRYPT          2

#define OFFSET(type,field) \
  (int)&(((type*)NULL)->field)

//===========================================================================
// Class MemBuf
//
// Auto-sized binary buffer with cryptographic capabilities
//===========================================================================
class YAT_DECL MemBuf
{
private:
  uint32  m_uiPos;             ///< Position (for reading)
  uint32  m_uiLen;             ///< Buffer size
  uint32  m_uiLenBuf;          ///< Allocated size
  char* m_pBuf;             ///< The buffer!
  bool  m_bOwner;            ///< if true the instance own the buffer

  // Re-allocation
  void realloc_with_margin(uint32 uiNewSize) ;

public:
  /// Constructor
  MemBuf(uint32 uiLenBuf=0) ;
  MemBuf(const MemBuf& buf) ;
  /// Destructor
  ~MemBuf() ;

  /// Copy operator
  MemBuf& operator=(const MemBuf& buf);

  // Comparaison operator
  bool operator==(const MemBuf &mb) const;
  
  /// Attachment from a external buffer
  ///
  /// @param pBuf memory area to attach
  /// @param uiLen area size
  /// @param bOwner if true this instance will own the buffer
  ///
  void attach(char* pBuf, uint32 uiLen, bool bOwner = false);

  /// Length
  uint32 len() const  { return m_uiLen; }

  /// Is empty ?
  int is_empty() const { return m_uiLen==0; }

  // Buffer size
  uint32 buf_len() const { return m_uiLenBuf; }

  /// buffer pointer
  char* buf() const   { return m_pBuf; }

  /// buffer pointer in *bytes* data type
  byte *bytes() const { return (byte *)m_pBuf; }
   
  /// Position in buffer
  uint32 pos() const    { return m_uiPos; }

  /// Set length
  void set_len(uint32 ui);

  /// reallocation
  void realloc(uint32 uiNewLenBuf);

  /// Add a binary bloc
  void put_bloc(const void* p, uint32 uiNb);

  /// Get a binary bloc from the buffer
  int get_bloc(void* p, uint32 uiNb);

  /// Insert a binary bloc at given offset
  void insert_bloc(const void* p, uint32 uiNb, uint32 uiPos);

  /// Move a part of the buffer
  void move_bloc(uint32 uiDst, uint32 uiSrc, uint32 uiSize);

  /// Set current read point to the begining
  void rewind()  { m_uiPos = 0; }

  /// Set the current read point
  void set_pos(uint32 ui)
    {
//##      ASSERT(ui <= m_uiLen)
      m_uiPos = ui;
    }

  /// Reset without free buffer memory
  void empty()
  {
    m_uiPos = 0 ;
    m_uiLen = 0 ;
  }

  /// Give buffer ownership to *this* instance
  void set_owner(bool bOwner) { m_bOwner = bOwner; }

  /// Give buffer ownership to another instance
  int give_ownership(MemBuf* pToHaveOwnership);

  /// Reset with memory freeing
  void reset();

  /// Compute CRC
  uint32 get_crc() const;

  /// Pointer to current position
  char *cur_pointer() const  { return (m_pBuf + m_uiPos); }

  /// Stream methods
  MemBuf& operator<<(bool b);
  MemBuf& operator>>(bool &b);
  MemBuf& operator<<(char c);
  MemBuf& operator>>(char &c);
  MemBuf& operator<<(byte uc);
  MemBuf& operator>>(byte &uc);
  MemBuf& operator<<(int16 s);
  MemBuf& operator>>(int16 &s);
  MemBuf& operator<<(uint16 us);
  MemBuf& operator>>(uint16 &us);
  MemBuf& operator<<(int32 l);
  MemBuf& operator>>(int32 &l);
  MemBuf& operator<<(uint32 ul);
  MemBuf& operator>>(uint32 &ul);
  MemBuf& operator<<(int64 i64);
  MemBuf& operator>>(int64 &i64);
  MemBuf& operator<<(uint64 i64);
  MemBuf& operator>>(uint64 &i64);
  MemBuf& operator<<(float f);
  MemBuf& operator>>(float &f);
  MemBuf& operator<<(double d);
  MemBuf& operator>>(double &d);
  MemBuf& operator<<(const char* psz);
  MemBuf& operator<<(const std::string& string);
  MemBuf& operator>>(std::string& string);
  MemBuf& operator<<(const MemBuf& membuf);
};

} // namespace

#endif // __MEMBUF_H___
