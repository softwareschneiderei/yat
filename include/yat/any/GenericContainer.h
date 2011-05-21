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

#ifndef _YAT_GENERIC_CONTAINER_H_
#define _YAT_GENERIC_CONTAINER_H_

#include <typeinfo>
#include <yat/CommonHeader.h> 

namespace yat 
{

// ============================================================================
//  class: Container
// ============================================================================
class YAT_DECL Container
{
public:
  Container ()
  {
    //- noop
  };

  virtual ~Container () 
  {
    //- noop
  };
};

// ============================================================================
//  template class: GenericContainer - class T must have a copy ctor
// ============================================================================
template <typename T> 
class GenericContainer : public Container
{
public:

  //- default ctor
  GenericContainer () 
    : ptr_(0), own_(false)
  {
    //- noop
  }

  //- ctor - no copy
  //- points to <_data> and optionnally gets <_ownership> 
  GenericContainer (T* _data, bool _transfer_ownership = true) 
    : ptr_(0), own_(false)
  {
    this->set_content(_data, _transfer_ownership);
  }

  //- ctor - makes a copy
  GenericContainer (const T& _data) 
    : ptr_(0), own_(false)
  {
    this->set_content(_data);
  }

  //- copy ctor  - makes a copy
  GenericContainer (const GenericContainer<T>& _src) 
    : ptr_(0), own_(false)
  {
    *this = _src;
  }

  //- dtor - delete data according to the ownership flag
  virtual ~GenericContainer ()
  {
    if (own_) delete ptr_;
  }

  //- changes content - makes a copy
  const GenericContainer& operator= (const GenericContainer<T>& _src)
  {  
    if (&_src == this)
      return *this;
    if (! ptr_ || ! own_)
    {
      try
      {
        ptr_ = new (std::nothrow) T(_src.content());
        if (! ptr_)
          throw std::bad_alloc();
      }
      catch (const std::bad_alloc&)
      {
        ptr_ = 0;
        own_ = false;
        THROW_YAT_ERROR("OUT_OF_MEMORY",
                        "memory allocation failed",
                        "GenericContainer:operator=");
      }
    }
    else 
    {
      *ptr_ = _src.content();
    }
    own_ = true;
    return *this;
  }

  //- changes content - makes a copy
  const GenericContainer& operator= (T& _src)
  {
    return this->operator=(static_cast<const T&>(_src));
  }
    
  //- changes content - makes a copy
  const GenericContainer& operator= (const T& _src)
  {
    if (! ptr_ || ! own_)
    {
      try
      {
        ptr_ = new (std::nothrow) T(_src);
        if (! ptr_)
          throw std::bad_alloc();
      }
      catch (const std::bad_alloc&)
      {
        ptr_ = 0;
        own_ = false;
        THROW_YAT_ERROR("OUT_OF_MEMORY",
                        "memory allocation failed",
                        "GenericContainer:operator=");
      }
    }
    else 
    {
      *ptr_ = _src;
    }
    own_ = true;
    return *this;
  }

  //- changes content but does NOT gets ownership of the data
  const GenericContainer& operator= (T* _data)
  { 
    if (_data == ptr_)
      return *this;
    if (own_)
      delete ptr_;
    ptr_ = _data;
    own_ = false;
    return *this;
  }

  //- changes content and set ownership
  void set_content (T* _data, bool _transfer_ownership)
  {
    *this = _data;
    own_ = _transfer_ownership;
  }

  //- changes content (makes a copy)
  void set_content (const T& _data)
  {
    *this = _data;
  }

  //- changes content (makes a copy)
  void set_content (T& _data)
  {
    *this = _data;
    own_ = true;
  }
  
  //- returns content 
  //- does NOT transfer data ownership to the caller
  T * operator-> ()
  {
    return ptr_;
  }
	
  //- returns content 
  //- optionally transfers data ownership to the caller
  T * get_content (bool transfer_ownership)
  {
    if (transfer_ownership)
      own_ = false;
    return ptr_;
  }

  //- returns content
  T & get_content ()
    throw (Exception)
  {
    if (! ptr_)
    {
      THROW_YAT_ERROR("RUNTIME_ERROR",
                      "could not extract data from GenericContainer [empty]",
                      "GenericContainer::get_content");
    }
    return *ptr_;
  }

  //- does this container have ownership of the underlying data?
  bool get_ownership () const
  {
    return own_;
  }
  
  //- returns true if the container is empty, return false otherwise
  bool empty () const 
  {
    return ptr_ ? false : true;
  }

private:
  //- actual container content
  T * ptr_;
  //- do we have data ownership?
  bool own_;
};

// ============================================================================
//  template method: any_cast
// ============================================================================
template<typename T>
T * any_cast (Container * _c, bool _transfer_ownership = false)
{
  GenericContainer<T> * gc = dynamic_cast<GenericContainer<T>*>(_c);
  return gc 
       ? gc->get_content(_transfer_ownership)
       : 0;
}

// ============================================================================
//  template method: any_cast
// ============================================================================
template<typename T>
const T * any_cast (const Container * c)
{
  return any_cast<T>(const_cast<Container*>(c), false);
}

// ============================================================================
//  template method: any_cast
// ============================================================================
template<typename T>
const T & any_cast (const Container & c) 
  throw (yat::Exception)
{
  const T * t = any_cast<T>(&c);
  if (! t)
  {
    THROW_YAT_ERROR("RUNTIME_ERROR",
                    "could not extract data from GenericContainer [attached data type is not of specified type]",
                    "yat::any_cast");
  }
  return *t;
}

// ============================================================================
//  template method: any_cast
// ============================================================================
template<typename T>
T & any_cast (Container & c) 
  throw (yat::Exception)
{
  T * t = any_cast<T>(&c, false);
  if (! t)
  {
    THROW_YAT_ERROR("RUNTIME_ERROR",
                    "could not extract data from GenericContainer [attached data type is not of specified type]",
                    "yat::any_cast");
  }
  return *t;
}

} // namespace yat

#endif



