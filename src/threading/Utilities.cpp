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
// Copyright (C) 2006-2022  S. Poirier & The Tango Community
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
//      Stephane Poirier
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */

// ----------------------------------------------------------------------------
// DEPENDENCIES
// ----------------------------------------------------------------------------
#include <yat/threading/Utilities.h>

// ----------------------------------------------------------------------------
// SOME PSEUDO CONSTs
// ----------------------------------------------------------------------------
#define MAX_NSECS 1000000000
#define MAX_USECS 1000000
#define MAX_MSECS 1000

namespace yat {

// ----------------------------------------------------------------------------
// ThreadingUtilities::sleep
// ----------------------------------------------------------------------------
void ThreadingUtilities::sleep (long _secs, long _nano_secs)
{
  sleep_impl(_secs, _nano_secs);
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::sleep
// ----------------------------------------------------------------------------
void ThreadingUtilities::sleep(double _secs)
{
  if( _secs < 0 )
    throw yat::Exception("BAD_ARGUMENT", "sleep time can't be negative!",
                         "ThreadingUtilities::sleep");

  long secs = long(_secs);
  long nanos = (_secs - secs) * MAX_NSECS;
  sleep_impl(secs, nanos);
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::sleep_millis
// ----------------------------------------------------------------------------
void ThreadingUtilities::sleep_millis(uint64 _millis)
{
  long secs = long(_millis / MAX_MSECS);
  long nanos = (_millis % MAX_MSECS) * MAX_USECS;
  sleep_impl(secs, nanos);
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::sleep_micros
// ----------------------------------------------------------------------------
void ThreadingUtilities::sleep_micros(uint64 _micros)
{
  long secs = long(_micros / MAX_USECS);
  long nanos = (_micros % MAX_USECS) * MAX_MSECS;
  sleep_impl(secs, nanos);
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::sleep_nanos
// ----------------------------------------------------------------------------
void ThreadingUtilities::sleep_nanos(uint64 _nanos)
{
  long secs = long(_nanos / MAX_NSECS);
  long nanos = _nanos % MAX_NSECS;
  sleep_impl(secs, nanos);
}

} // namespace yat
