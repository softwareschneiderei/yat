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
#include <iostream>
#include <yat/plugin/PlugIn.h>
#include <yat/plugin/PlugInSymbols.h>
#include <yat/plugin/IPlugInObjectWithAttr.h>

namespace yat
{

//==============================================================================
// PlugInBase::PlugInBase
//==============================================================================
PlugInBase::PlugInBase(const std::string &library_file_name)
  : m_libraryHandle( NULL )
  , m_libraryName( library_file_name )
{
}

//==============================================================================
// PlugInBase::~PlugInBase
//==============================================================================
PlugInBase::~PlugInBase()
{
}

//==============================================================================
// PlugIn::find_symbol
//==============================================================================
PlugInBase::Symbol PlugInBase::find_symbol(const std::string &symbol)
{
  try
  {
    Symbol symbol_pointer = do_find_symbol(symbol);
    if ( symbol_pointer != NULL )
      return symbol_pointer;
  }
  catch ( ... )
  {
  }

  THROW_YAT_ERROR("SHAREDLIBRARY_ERROR",
                  "Unable to find the requested symbol",
                  "PlugInBase::find_symbol");
  return NULL;    // keep compiler happy
}

}
