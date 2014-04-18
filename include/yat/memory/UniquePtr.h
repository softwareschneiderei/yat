//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2014 The Tango Community
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
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */

#ifndef _YAT_UNIQUE_PTR_H_
#define _YAT_UNIQUE_PTR_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <iostream>
#include <yat/utils/ReferenceCounter.h>
    
namespace yat
{

// ============================================================================
//! \class _UPtr_DisposerBase
//! \brief A deleter manager for UniquePtr class.
//! \note For internal use only 
// ============================================================================
template <typename T>
class _UPtr_DisposerBase
{
public:
  //! \brief Object disposal method (i.e. delete function).
  virtual void dispose(T* data) = 0; // nothrow

  //! \brief A "do nothing" destructor.
  virtual ~_UPtr_DisposerBase()
  {
    PTR_DBG("DisposerBase::~DisposerBase");
  }
  
protected:
  //! \brief Constructor.
  //! \param p Object to manage.
  //! \param d Specific deleter.
  _UPtr_DisposerBase()
  {
    PTR_DBG("DisposerBase::DisposerBase");
  }
  
};

// ============================================================================
//! \class _UPtr_DisposerImpl 
//! \brief Deleter manager implementation for UniquePtr class.
//! \note For internal use only
// ============================================================================
template <typename T, typename D>
class _UPtr_DisposerImpl: public _UPtr_DisposerBase<T>
{
public:
  //! \brief Constructor.
  //! \param p Object to manage.
  //! \param d Specific deleter.
  _UPtr_DisposerImpl(D d) : m_deleter(d)
  {
    PTR_DBG("UniquePtrDisposerImpl::UniquePtrDisposerImpl");
  }
  
  //! \brief A "do nothing" destructor.
  ~_UPtr_DisposerImpl()
  {
    PTR_DBG("UniquePtrDisposerImpl::~UniquePtrDisposerImpl");
  }
  
  //! \brief The specific deletion function.
  //!
  //! Calls for the specified deleter on the specified object.
  void dispose(T* data)
  {
    m_deleter(data);
  }
  
private:
  D m_deleter;
};

// ============================================================================
//! \class UniquePtr 
//! \brief A smart pointer abstraction class.
//!
//! This template class provides a smart pointer on type \<T\> data with selectable 
//!
//! \verbatim myPointer = new UniquePtr<mySimpleObjectType>(ptr); // defines a "simple" object pointer \endverbatim
//! 
//! If a specific deleter is necessary, like with yat::Task lock is necessary, use a deleter type, for example :
//! \verbatim myPointer = new UniquePtr<mySharedObjectType, MyDeleter>(ptr); \endverbatim
//! 
// ============================================================================
template <typename T>
class UniquePtr
{
  typedef UniquePtr<T> ThisType;

public:
  //! \brief Default constructor.
  //! 
  //! Data pointer and reference counter are initialized to null value.
  //! Default yat::Mutex type used for locking strategy.
  UniquePtr () : m_data(0), m_disposer(0)
  {
    PTR_DBG("UniquePtr::UniquePtr()");
  }

  //! \brief Constructor from data pointer and specific deleter.
  //! \param p Pointer to type \<T\> data.
  //! \param d Specific deleter. See yat::SharedCounter definition.
  //! Default yat::Mutex type used for locking strategy.
  UniquePtr (T* p) : m_data(p)
  {
    PTR_DBG("UniquePtr::UniquePtr(" << std::hex << (void*)p << ")");
    m_disposer = new _UPtr_DisposerImpl<T, DefaultDeleter<T> >(DefaultDeleter<T>());
  }

  //! \brief Constructor from data pointer and specific deleter.
  //! \param p Pointer to type \<T\> data.
  //! \param d Specific deleter. See yat::SharedCounter definition.
  //! Default yat::Mutex type used for locking strategy.
  template<typename D>
  UniquePtr (T* p, D d) : m_data(p)
  {
    PTR_DBG("UniquePtr::UniquePtr(" << std::hex << (void*)p << ")");
    m_disposer = new _UPtr_DisposerImpl<T, D>(d);
  }

  //! \brief Copy constructor from same data type \<T\>.
  //! \param s The source pointer.
  UniquePtr (ThisType & s) : m_data(s.m_data), m_disposer(s.m_disposer)
  {
    s.m_data = 0;
    s.m_disposer = 0;
    PTR_DBG("UniquePtr::UniquePtr(const ThisType & s) - m_data: " << std::hex << (void*)m_data << ")");
  }

  //! Destructor.
  ~UniquePtr()
  {
    PTR_DBG("UniquePtr::~UniquePtr()");
    dispose();
  }

  //! \brief Operator=.
  //! \param s The source pointer.
  ThisType& operator= (ThisType& s)
  {
    if( this != &s )
    {
      PTR_DBG("UniquePtr::operator=(const ThisType & s) - m_data: " << std::hex << (void*)m_data << ")");
      dispose();
      m_data = s.m_data;
      m_disposer = s.m_disposer;
      s.m_data = 0;
      s.m_disposer = 0;
    }
    return *this;
  }

  //! \brief Operator= from pointer to type \<T\> data.
  //! \param p The source pointer.
  const ThisType& operator= (T* p)
  {
    PTR_DBG("UniquePtr::operator=(" << std::hex << (void*)p << ")");
    if (p)
      reset(p);
    else
      reset();
    return *this;
  }

  //! \brief Operator*. 
  //!
  //! Returns the pointed data (type \<T\>).
  T& operator* () const
  {
    YAT_ASSERT(m_data);
    return *m_data;
  }

  //! \brief Operator->.
  //!
  //! Returns a pointer to the data (type \<T\>*).
  T * operator-> () const
  {
    return m_data;
  }

  //! \brief Returns a pointer to the managed object
  //!
  //! Returns a pointer to the managed object (type \<T\>*).
  T * get () const
  {
    return m_data;
  }

  //! \brief Returns a pointer to the managed object and releases the ownership .
  //!
  //! Returns a pointer to the managed object and releases the ownership  (type \<T\>*).
  T * release () const
  {
    T* data = m_data;
    m_data = 0;
    return data;
  }

  //! \brief Resets pointer to null value.
  void reset ()
  {
    PTR_DBG("UniquePtr::reset()");
    ThisType().swap(*this);
  } 

  //! \brief Resets pointer with new type \<T\> data.
  //! \param p New pointed data.
  void reset (T * p)
  {
    PTR_DBG("UniquePtr::reset(" << std::hex << (void*)p << ")");
    ThisType(p).swap(*this);
  } 

  //! \brief Resets pointer with new type \<T\> data and new type \<D\> deleter.
  //! \param p New pointed data.
  //! \param d New deleter.
  template <typename D>
  void reset (T * p, D d)
  {
    PTR_DBG("UniquePtr::reset(" << std::hex << (void*)p << ", d)");
    ThisType(p, d).swap(*this);
  } 

  //! \brief Swaps content with the specified pointer.
  //! \param s The source pointer.
  void swap (ThisType & s)
  {
    std::swap(m_data, s.m_data);
    std::swap(m_disposer, s.m_disposer);
  }

  typedef T* ThisType::*anonymous_bool_type;

  //! \brief Implicit conversion to bool.
  operator anonymous_bool_type () const
  {
    return m_data == 0 ? 0 : &ThisType::m_data;
  }

  //! \brief Does *this* point to something ?
  //!
  //! Returns true if *this* points to a non allocated memory area, 
  //! false otherwise.
  bool is_null () const
  {
    return m_data ? false : true;
  }

  //! \brief Internal implementation of the '<' operator.
  //!
  //! Returns true if *this* pointed data is "less than" source pointed data, 
  //! false otherwise.
  //! \param s The source pointer.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template<class Y> 
  bool less_than (UniquePtr<Y> const & s) const
  {
    return m_data < s.m_data;
  }

private:

  //- Destroy owned object.
  void dispose()
  {
    if( m_data && m_disposer )
      m_disposer->dispose(m_data);
    if( m_disposer )
      delete m_disposer;
  }

  //- Pointed data.
  T * m_data;

  //- Deleter manager
  _UPtr_DisposerBase<T>* m_disposer;
};

//! \brief Operator< on SharedPtr type, i.e. is \<a\> less than \<b\> ?
//! \param a Left shared pointer.
//! \param b Right shared pointer.
template <typename T> 
inline bool operator<(UniquePtr<T> const & a, UniquePtr<T> const & b)
{
 return a.less_than(b);
}


} //- namespace

#endif //- _YAT_UNIQUE_PTR_H_
