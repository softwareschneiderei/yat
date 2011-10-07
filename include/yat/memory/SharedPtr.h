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
 * \author N.Leclercq - Synchrotron SOLEIL
 */


#ifndef _YAT_SHARED_PTR_H_
#define _YAT_SHARED_PTR_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/utils/ReferenceCounter.h>
#include <iostream>

namespace yat
{

// ============================================================================
// class: SharedPtr
// ============================================================================
template <typename T, typename L = yat::NullMutex> class SharedPtr
{
  typedef SharedPtr<T,L> ThisType;
  typedef ReferenceCounter<unsigned long, L> ThisTypeRefCnt;

public:
  //! constructor
  SharedPtr () 
    : m_data(0), m_ref_count(0) 
  {
  }

  //! constructor
  SharedPtr (T* p) 
    : m_data(p), m_ref_count(0) 
  {
    this->m_ref_count = new ThisTypeRefCnt(1);
    YAT_ASSERT(this->m_ref_count);
  }

  //! copy constructor
  SharedPtr (const ThisType & s) 
    : m_data(s.m_data), m_ref_count(s.m_ref_count) 
  {
    if( m_data )
    {
      YAT_ASSERT(this->m_ref_count);
      this->m_ref_count->increment();
    }
  }

  //! destructor
  ~SharedPtr()
  {
    this->release();    
  }

  //! operator=
  const ThisType& operator= (const ThisType& s)
  {
    if (this != &s)
    {
      this->release();
      this->m_data = s.m_data;
      this->m_ref_count = s.m_ref_count;
      if( m_data )
      {
        YAT_ASSERT(this->m_ref_count);
        this->m_ref_count->increment();
      }
    }
    return *this;
  }

  //! operator=
  const ThisType& operator= (T* p)
  {
    if (p)
      this->reset(p);
    else
      this->reset();
    return *this;
  }

  //! operator*
  T& operator* () const
  {
    YAT_ASSERT(this->m_data);
    return *this->m_data;
  }

  //! operator->
  T * operator-> () const
  {
    return this->m_data;
  }

  //! operator->
  T * get () const
  {
    return this->m_data;
  }

  //! reset
  void reset ()
  {
    ThisType().swap(*this);
  } 

  //! reset
  void reset (T * p)
  {
    ThisType(p).swap(*this);
  } 

  //! reset (Y must be T convertible)
  template <typename Y> 
  void reset (Y * p)
  {
    ThisType(p).swap(*this);
  } 

  //- swap content
  void swap (ThisType & s)
  {
    std::swap(this->m_data, s.m_data);
    std::swap(this->m_ref_count, s.m_ref_count);
  }

  //! unique
  bool unique () const
  {
    YAT_ASSERT(this->m_ref_count);
    return this->m_ref_count->unique();
  }

  //! use count
  unsigned long use_count () const
  {
    YAT_ASSERT(this->m_ref_count);
    return this->m_ref_count->use_count();
  }

  //! implicit conversion to bool
  typedef T* ThisType::*anonymous_bool_type;
  operator anonymous_bool_type () const
  {
    return this->m_data == 0 ? 0 : &ThisType::m_data;
  }

  //! does this point to something?  
  bool is_null () const
  {
    return this->m_data ? false : true;
  }

  //! internal impl of the '<' operator
  template<class Y> 
  bool less_than (SharedPtr<Y> const & s) const
  {
    return m_data < s.m_data;
  }

private:
  //! release underlying data if ref. counter reaches 0
  void release ()
  {
    if (this->m_data && this->m_ref_count->decrement() == 0)
    {
      try { delete this->m_data; } catch (...) {};
      this->m_data = 0;
      try { delete this->m_ref_count; } catch (...) {};
      this->m_ref_count = 0;
    }
  }

  //- pointed data
  T * m_data;
  //- reference counter
  ThisTypeRefCnt * m_ref_count;
};

// ============================================================================
// The SharedObjectPtr class 
// ============================================================================
template <typename T, typename U> 
inline bool operator<(SharedPtr<T> const & a, SharedPtr<U> const & b)
{
 return a.less_than(b);
}

// ============================================================================
//! The SharedObjectPtr class 
// ============================================================================
//! Template arg class <T> must inherit from yat::SharedObject
// ============================================================================
template <typename T> class SharedObjectPtr
{
public:

  /**
   * Default constructor. 
   */
  SharedObjectPtr() : m_so(0)
  {
    //- noop
  }

  /**
   * Constructor with initialization. 
   * Takes ownership of the specified yat::SharedObject. 
   * <T> must inherit from yat::SharedObject. 
   */
  template <typename Y> explicit SharedObjectPtr( Y * so ) : m_so(so)
  {
    //- noop
  }
  
  /**
   * Destructor.
   */
  ~SharedObjectPtr()
  {
    this->reset();
  }

  /**
   * Assignment operator.
   * Releases currently pointed SharedObject, then points to the specified one.
   */
  SharedObjectPtr& operator= (SharedObjectPtr& r) 
  {
    T * new_so = r ? r.m_so->duplicate() : 0;
    if (m_so)
      m_so->release();
    m_so = new_so;
    return *this;
  }

  /**
   * Releases the currently pointed yat::SharedObject then takes ownership of the specified one
   */
  void reset ( T* so = 0 )
  {
    if (m_so)
      m_so->release();
    m_so = so;
  }

  /**
   * Underlying yat::SharedObject accessor
   */
  T* get()
  {
    return m_so;
  }
  
  /**
   * Underlying yat::SharedObject accessor
   */
  T* operator-> ()
  {
    return m_so;
  }
  
  /**
   * Underlying yat::SharedObject accessor
   */
  T& operator* ()
  {
    return *m_so;
  }

  /**
   * Implicit conversion to bool
   */
  typedef T* SharedObjectPtr::*anonymous_bool_type;
  operator anonymous_bool_type() const
  {
    return m_so ? &SharedObjectPtr::m_so : 0;
  }

private:
  T * m_so;
};

} //- namespace

#endif //- _YAT_SHARED_PTR_H_
