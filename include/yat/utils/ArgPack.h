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
// Copyright (C) 2006-2024 The Tango Community
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

#ifndef _YAT_ARGPACK_H_
#define _YAT_ARGPACK_H_

// ----------------------------------------------------------------------------
// DEPENDENCIES
// ----------------------------------------------------------------------------
#include <vector>
#include <yat/any/Any.h>
#include <yat/memory/SharedPtr.h>

namespace yat {

// =============================================================================
//! The ArgPack class provides the means to store a set of function arguments
// =============================================================================
class YAT_DECL ArgPack
{
public:
  //! construct an argument pack
  //!
  //! \param args any number of arguments of any type
  template<typename... Args>
  ArgPack(Args&&... args)
  {
    if( sizeof...(args) > 0 )
      add(args...);
  }

  //! Swap content with another ArgPack
  //!
  //! \param other the other ArgPack
  void swap(ArgPack& other)
  {
    m_args_p.swap(other.m_args_p);
  }

  //! Retreive an argument from the pack
  //! may throw in case of index out of bounds or bad template type
  //!
  //! \param idx argument index
  //! \return the argument value
  template<typename T>
  const T& get(std::size_t idx=0) const
  {
    if( idx < m_args_p.size() )
    {
      return any_cast_ext<T>(*(m_args_p)[idx]);
    }
    throw Exception("OUT_OF_BOUNDS",
                    yat::SFormat("Can't retreive argument #{}. ArgPack contains "
                                 "only {} argument(s)") % idx % m_args_p.size(),
                    "yat::ArgPack::get<T>");
  }

  //! returns the number of arguments stored in the pack
  std::size_t count() const { return m_args_p.size(); }

private:

  void add() { }

  template<class T>
  void add(T arg)
  {
    Any* any = new Any(arg);
    m_args_p.push_back(any);
  }

  template<class T, class... Args>
  void add(T arg, Args&&... args)
  {
    m_args_p.push_back(new Any(arg));
    if( sizeof...(args) > 0 )
      add(args...);
  }

  std::vector<SharedPtr<Any>> m_args_p;
};

} // namespace yat

#endif //- _YAT_ARGPACK_H_
