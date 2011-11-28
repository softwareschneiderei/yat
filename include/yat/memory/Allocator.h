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

#ifndef _YAT_ALLOCATOR_H_
#define _YAT_ALLOCATOR_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <deque>
#include <yat/threading/Mutex.h>
#include <yat/CommonHeader.h>

namespace yat 
{

// ============================================================================
//! The NewAllocator class 
// ============================================================================
//!  
// ============================================================================
template <typename T> class NewAllocator
{
public:
  //- Ctor
  NewAllocator ();

  //- Dtor
  virtual ~NewAllocator ();
  
  //- memory allocation - can't allocate more than sizeof(T)
  virtual T * malloc ();

  //- memory release - <p> must have been allocated by <this> CachedAllocator
  virtual void free (T * p);
};

/*
// ============================================================================
//! The CachedAllocator class  
// ============================================================================
//! Implements an unbounded memory pool of T with selectable locking strategy 
//! ok but for "cachable" classes only!
// ============================================================================
#define CACHABLE_CLASS_DECL(CLASS_NAME, LOCKING_STRATEGY)
  typedef CachedAllocator<CLASS_NAME, LOCKING_STRATEGY> CLASS_NAME#Cache; \
  static CLASS_NAME#::Cache m_cache; \
  void * operator new (size_t); \
  void operator delete (void *); \
  static void pre_alloc (size_t _nobjs); \
  static void release_pre_alloc ();
#endif
  //-TODO: not usable for the moment - to be finished...
*/

// ============================================================================
//! The CachedAllocator class  
// ============================================================================
//! Implements an unbounded memory pool of T with selectable locking strategy 
//! ok... for "cachable" classes only!
// ============================================================================
template <typename T, typename L = yat::NullMutex> 
class CachedAllocator : public NewAllocator<T>
{
  //- memory pool (or cache) implementation
  typedef std::deque<T*> CacheImpl; 

public: 
  //- Ctor - preallocates <nb_bunches * nb_objs_per_bunch> 
  //- A bunch of <nb_objs_per_bunch> instances of T will be preallocated each time the cache gets empty
  CachedAllocator (size_t nb_bunches = 0, size_t nb_objs_per_bunch = 0);

  //- Dtor
  virtual ~CachedAllocator();
  
  //- allocates an instance of T
  virtual T * malloc ();

  //- releases an instance of T 
  //- <p> must have been allocated by <this> allocator
  //- <p> returns to the cache 
  virtual void free (T * p);
  
  //- releases an instance of T
  //- <p> must have been allocated by <this> allocator
  //- <p> does NOT return to the cache
  virtual void release (T * p);
  
  //- returns the number of T instances currently stored into the cache
  inline size_t length () const 
  {
    return m_cache.size();
  }

  //- release <p> % of the cache
  inline void clear (const double p = 100.);

  //- returns the number of bytes currently stored into the cache
  inline size_t size () const 
  {
    return m_cache.size() * sizeof(T);
  }
  
  //- returns the number of T instances (pre)allocated each time the cache gets empty 
  inline size_t bunch_length () const 
  {
    return m_nb_objs_per_bunch;
  }
  
  //- sets the number of T instances (pre)allocated each time the cache gets empty 
  inline void bunch_length (size_t n) 
  {
    m_nb_objs_per_bunch = n;
  }
  
protected:
  //- locking (i.e. thread safety) strategy
  L m_lock;

  //- number T instance per bunch
  size_t m_nb_objs_per_bunch;

  //- the memory cache (i.e. memory pool) 
  CacheImpl m_cache;
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/memory/Allocator.i>
#endif // YAT_INLINE_IMPL

#include <yat/memory/Allocator.tpp>

#endif // _YAT_ALLOCATOR_H_


