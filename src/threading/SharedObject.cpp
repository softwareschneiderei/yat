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
// Copyright (C) 2006-2021 The Tango Community
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

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/SharedObject.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/threading/SharedObject.i>
#endif

namespace yat
{

// ============================================================================
// SharedObject::SharedObject
// ============================================================================
SharedObject::SharedObject ()
 : reference_count_ (1)
{
  YAT_TRACE("SharedObject::SharedObject");
}

// ============================================================================
// SharedObject::~SharedObject
// ============================================================================
SharedObject::~SharedObject ()
{
  YAT_TRACE("SharedObject::~SharedObject");

  DEBUG_ASSERT(this->reference_count_ == 0);
}

// ============================================================================
// SharedObject::duplicate
// ============================================================================
SharedObject * SharedObject::duplicate ()
{
  YAT_TRACE("SharedObject::duplicate");

  MutexLock guard(this->lock_);

  this->reference_count_++;

  return this;
}

// ============================================================================
// SharedObject::release
// ============================================================================
int SharedObject::release (bool _commit_suicide)
{
  YAT_TRACE("SharedObject::release");

  SharedObject * more_ref = this->release_i ();

  if (! more_ref && _commit_suicide)
    delete this;

  return more_ref ? 1 : 0;
}

// ============================================================================
// SharedObject::release_i
// ============================================================================
SharedObject *SharedObject::release_i ()
{
  MutexLock guard (this->lock_);

  DEBUG_ASSERT(this->reference_count_ > 0);

  this->reference_count_--;

  return (this->reference_count_ == 0) ? 0 : this;
}

} //- namespace
