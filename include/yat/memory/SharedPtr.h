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
#include <map>

    
namespace yat
{


// ============================================================================
// forward declaration class: WeakPtr
// ============================================================================
template <typename T, typename L = yat::NullMutex> class WeakPtr;

// ============================================================================
// class: SharedPtr
// ============================================================================
template <typename T, typename L = yat::NullMutex>
class SharedPtr
{
  template<typename U, typename V> friend class WeakPtr;
  template<typename U, typename V> friend class SharedPtr;

  typedef SharedPtr<T,L> ThisType;
  typedef SharedCounter<counter_t, L> ThisTypeRefCnt;

public:
  //! default constructor
  SharedPtr () 
    : m_data(0), m_ref_count((T*)0)
  {
    PTR_DBG("SharedPtr::SharedPtr()");
  }

  //! constructor
  SharedPtr (T* p) 
    : m_data(p), m_ref_count(p)
  {
    PTR_DBG("SharedPtr::SharedPtr(" << std::hex << (void*)p << ")");
  }

  //! constructor
  template <typename D>
  SharedPtr (T* p, D d) 
    : m_data(p), m_ref_count(p, d)
  {
    PTR_DBG("SharedPtr::SharedPtr(" << std::hex << (void*)p << ")");
  }

  //! copy constructor
  SharedPtr (const ThisType & s) 
    : m_data(s.m_data), m_ref_count(s.m_ref_count) 
  {
    PTR_DBG("SharedPtr::SharedPtr(const ThisType & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("SharedPtr::SharedPtr(const ThisType & s) - use_count: " << use_count());
  }

  //! copy constructor
  //! Throw an exception if Y is not compatible with T
  template<typename Y>
  explicit SharedPtr (const SharedPtr<Y,L> & s): m_ref_count(s.m_ref_count) 
  {
    cast_copy_data(s.m_data);
    PTR_DBG("SharedPtr::SharedPtr(const SharedPtr<Y,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("SharedPtr::SharedPtr(const SharedPtr<Y,L> & s) - use_count: " << use_count());
  }

  //! constructor from WeakPtr<T,L>
  explicit SharedPtr(const WeakPtr<T,L>& s): m_ref_count(s.m_ref_count)
  {
    PTR_DBG("SharedPtr::SharedPtr(const WeakPtr<T,L>&) - use_count: " << use_count());
    if( m_ref_count.use_count() == 0 )
    { // pointer has expired -> returns a null pointer
      m_data = 0;
    }
    else
    {
      m_data = s.m_data;
    }
    PTR_DBG("Use_count: " << use_count());
  }

  //! constructor from WeakPtr<Y,L>
  //! Throw an exception si Y is not compatible with T
  template<typename Y>
  SharedPtr(const WeakPtr<Y,L>& s)
  {
    PTR_DBG("SharedPtr::SharedPtr(const WeakPtr<Y,L>&) - use_count: " << use_count());
    if( s.m_ref_count.use_count() == 0 )
    { // pointer has expired -> returns a null pointer
      m_data = 0;
    }
    else
    {
      copy_cast_data(s.m_data);
      m_ref_count = s.m_ref_count;
    }
    PTR_DBG("Use_count: " << use_count());
  }

  //! destructor
  ~SharedPtr()
  {
    PTR_DBG("SharedPtr::~SharedPtr()");
  }

  //! operator=
  const ThisType& operator= (const ThisType& s)
  {
    if (this != &s)
    {
      PTR_DBG("SharedPtr::operator=(const ThisType & s) - m_data: " << std::hex << (void*)m_data << ")");
      m_ref_count = s.m_ref_count;
      m_data = s.m_data;
      PTR_DBG("SharedPtr::SharedPtr(const ThisType & s) - use_count: " << use_count());
    }
    return *this;
  }

  //! operator=
  const ThisType& operator= (T* p)
  {
    PTR_DBG("SharedPtr::operator=(" << std::hex << (void*)p << ")");
    if (p)
      reset(p);
    else
      reset();
    return *this;
  }

  //! operator*
  T& operator* () const
  {
    YAT_ASSERT(m_data);
    return *m_data;
  }

  //! operator->
  T * operator-> () const
  {
    return m_data;
  }

  //! operator->
  T * get () const
  {
    return m_data;
  }

  //! reset
  void reset ()
  {
    PTR_DBG("SharedPtr::reset()");
    ThisType().swap(*this);
  } 

  //! reset
  void reset (T * p)
  {
    PTR_DBG("SharedPtr::reset(" << std::hex << (void*)p << ")");
    ThisType(p).swap(*this);
  } 

  //! reset (Y must be T convertible)
  template <typename Y> 
  void reset (Y * p)
  {
    PTR_DBG("template <typename Y>SharedPtr::reset(" << std::hex << (void*)p << ")");
    ThisType(p).swap(*this);
  } 

  //- swap content
  void swap (ThisType & s)
  {
    std::swap(m_data, s.m_data);
    m_ref_count.swap(s.m_ref_count);
  }

  //! unique
  bool unique () const
  {
    return m_ref_count->unique();
  }

  //! use count
  unsigned long use_count () const
  {
    return m_ref_count.use_count();
  }

  //! implicit conversion to bool
  typedef T* ThisType::*anonymous_bool_type;
  operator anonymous_bool_type () const
  {
    return m_data == 0 ? 0 : &ThisType::m_data;
  }

  //! does this point to something?  
  bool is_null () const
  {
    return m_data ? false : true;
  }

  //! internal impl of the '<' operator
  template<class Y> 
  bool less_than (SharedPtr<Y,L> const & s) const
  {
    return m_data < s.m_data;
  }

private:

  // Try to copy data pointer of foreign type
  template<typename Y>
  void cast_copy_data(Y* data)
  {
    PTR_DBG("SharedPtr::cast_copy_data()");
    m_data = dynamic_cast<T*>(data);
    if( data != m_data )
    {
      throw Exception("BAD_CAST", "Trying to cast SharedPtr<T,L> to a uncompatible SharedPtr<Y,L>", "SharedPtr::cast_copy_data");
    }
  }

  //- pointed data
  T * m_data;
  //- reference counter
  ThisTypeRefCnt m_ref_count;
};


//! comparison operator
template <typename T, typename U, typename L> 
inline bool operator<(SharedPtr<T, L> const & a, SharedPtr<U, L> const & b)
{
 return a.less_than(b);
}

// ============================================================================
// class: WeakPtr
// ============================================================================
template <typename T, typename L> class WeakPtr
{
  typedef WeakPtr<T,L> ThisType;
  typedef WeakCounter<counter_t, L> ThisTypeRefCnt;

  template<class U, class V> friend class WeakPtr;
  template<class U, class V> friend class SharedPtr;

public:
  //! constructor
  WeakPtr () 
    : m_data(0)
  {
    PTR_DBG("WeakPtr::WeakPtr()");
  }

  //! copy constructor
  WeakPtr (const WeakPtr<T,L> & s) 
    : m_data(s.m_data), m_ref_count(s.m_ref_count) 
  {
    PTR_DBG("WeakPtr::WeakPtr(const ThisType & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("WeakPtr::WeakPtr(const ThisType & s) - use_count: " << use_count());
  }

  //! copy constructor
  template <typename Y>
  WeakPtr (const WeakPtr<Y,L> & s) 
  {
    cast_copy_data(s.m_data);
    m_ref_count = s.m_ref_count;
    PTR_DBG("WeakPtr::WeakPtr(const WeakPtr<Y,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("WeakPtr::WeakPtr(const WeakPtr<Y,L> & s) - use_count: " << use_count());
  }

  //! constructor
  WeakPtr (const SharedPtr<T,L> & s) 
    : m_data(s.m_data), m_ref_count(s.m_ref_count) 
  {
    PTR_DBG("WeakPtr::WeakPtr(const ThisType & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("WeakPtr::WeakPtr(const ThisType & s) - use_count: " << use_count());
  }

  //! constructor
  template <typename Y>
  WeakPtr (const SharedPtr<Y,L> & s) 
  {
    cast_copy_data(s.m_data);
    m_ref_count = s.m_ref_count;
    PTR_DBG("WeakPtr::WeakPtr(const SharedPtr<Y,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("WeakPtr::WeakPtr(const SharedPtr<Y,L> & s) - use_count: " << use_count());
  }

  //! destructor
  ~WeakPtr()
  {
    PTR_DBG("WeakPtr::~WeakPtr()");
  }

  //! operator=
  const WeakPtr<T,L>& operator= (const WeakPtr<T,L>& s)
  {
    if (this != &s)
    {
      PTR_DBG("WeakPtr::operator=(const WeakPtr<T,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
      m_ref_count = s.m_ref_count;
      m_data = s.m_data;
      if( m_data )
      {
        PTR_DBG("WeakPtr::operator=(const WeakPtr<T,L> & s) - use_count: " << use_count());
      }
    }
    return *this;
  }

  //! operator=
  //! Throw an exception si Y is not compatible with T
  template <class Y>
  const WeakPtr<T,L>& operator= (const WeakPtr<Y,L>& s)
  {
    PTR_DBG("WeakPtr::operator=(const WeakPtr<Y,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    cast_copy_data(s.m_data);
    m_ref_count = s.m_ref_count;
    PTR_DBG("WeakPtr::operator=(const WeakPtr<Y,L> & s) ref counter copied");
    if( m_data )
    {
      PTR_DBG("WeakPtr::operator=(const WeakPtr<Y,L> & s) - use_count: " << use_count());
    }
    return *this;
  }

  //! operator=
  //! Throw an exception si Y is not compatible with T
  const WeakPtr<T,L>& operator= (const SharedPtr<T,L>& s)
  {
    PTR_DBG("WeakPtr::operator=(const SharedPtr<T,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    m_data = s.m_data;
    m_ref_count = s.m_ref_count;
    if( m_data )
    {
      PTR_DBG("WeakPtr::operator=(const SharedPtr<T,L> & s) - use_count: " << use_count());
    }
    return *this;
  }

  //! operator=
  //! Throw an exception si Y is not compatible with T
  template <class Y>
  const WeakPtr<T,L>& operator= (const SharedPtr<Y,L>& s)
  {
    PTR_DBG("WeakPtr::operator=(const SharedPtr<Y,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    cast_copy_data(s.m_data);
    m_ref_count = s.m_ref_count;
    PTR_DBG("WeakPtr::operator=(const SharedPtr<Y,L> & s) ref counter copied");
    if( m_data )
    {
      PTR_DBG("WeakPtr::operator=(const SharedPtr<Y,L> & s) - use_count: " << use_count());
    }
    return *this;
  }

  //! reset
  void reset ()
  {
    PTR_DBG("WeakPtr::reset()");
    ThisType().swap(*this);
  } 

  //- swap content
  void swap (ThisType & s)
  {
    std::swap(m_data, s.m_data);
    std::swap(m_ref_count, s.m_ref_count);
  }

  //! use count
  unsigned long use_count () const
  {
    return m_ref_count.use_count();
  }

  //! safe access only
  SharedPtr<T,L> lock() const
  {
    PTR_DBG("WeakPtr::lock()");      
    // Returns a sharedptr in order to get access to the pointed object
    return SharedPtr<T,L>(*this);
  }
  
  //! implicit conversion to bool
  typedef T* ThisType::*anonymous_bool_type;
  operator anonymous_bool_type () const
  {
    return m_data == 0 || expired() ? 0 : &ThisType::m_data;
  }

  //! does this point to something?  
  bool is_null () const
  {
    return m_data ? false : true;
  }

  //! does this point to a valid reference?  
  bool expired () const
  {
    return m_ref_count.use_count() == 0;
  }

  //! internal impl of the '<' operator
  template<class Y> 
  bool less_than (WeakPtr<Y,L> const & s) const
  {
    return m_data < s.m_data;
  }

private:
  //- pointed data
  T * m_data;
  //- reference counter
  ThisTypeRefCnt m_ref_count;

  //- try to copy foreign data pointer
  template<typename Y>
  void cast_copy_data(Y* data)
  {
    PTR_DBG("WeakPtr::cast_copy_data()");
    m_data = dynamic_cast<T*>(data);
    PTR_DBG("WeakPtr::cast_copy_data(): copied");
    if( data != m_data )
    {
      throw Exception("BAD_CAST", "Trying to cast WeakPtr<T,L> to a uncompatible WeakPtr<Y,L>", "WeakPtr::cast_copy_data");
    }
  }
};

//! comparison operator
template <typename T, typename U, typename L> 
inline bool operator<(WeakPtr<T, L> const & a, WeakPtr<U, L> const & b)
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
    reset();
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
