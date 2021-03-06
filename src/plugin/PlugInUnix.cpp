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
// Copyright (C) 2006-2021  N.Leclercq & The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
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

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/plugin/PlugIn.h>


#if defined(YAT_LINUX) || defined(YAT_MACOSX)

#include <dlfcn.h>
#include <unistd.h>
#include <iostream>

namespace yat
{

//-----------------------------------------------------------------------------
// PlugIn::do_load_library
//-----------------------------------------------------------------------------
PlugIn::LibraryHandle PlugIn::do_load_library( const std::string &library_file_name )
{
  PlugIn::LibraryHandle handle = ::dlopen( library_file_name.c_str(), RTLD_NOW | RTLD_GLOBAL );
  if( NULL == handle )
  {
    THROW_YAT_ERROR( std::string("SHAREDLIBRARY_ERROR"),
                     std::string(::dlerror()),
                     std::string("PlugIn::do_load_library") );
  }
  return handle;
}

//-----------------------------------------------------------------------------
// PlugIn::do_release_library
//-----------------------------------------------------------------------------
void PlugIn::do_release_library()
{
  ::dlclose( m_libraryHandle);
}

//-----------------------------------------------------------------------------
// PlugIn::do_find_symbol
//-----------------------------------------------------------------------------
PlugIn::Symbol PlugIn::do_find_symbol( const std::string &symbol )
{
  return ::dlsym ( m_libraryHandle, symbol.c_str() );
}

//-----------------------------------------------------------------------------
// PlugIn::get_last_error_detail
//-----------------------------------------------------------------------------
std::string PlugIn::get_last_error_detail() const
{
  return ::dlerror();
}

}

#endif
