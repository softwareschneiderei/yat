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
// Copyright (C) 2006-2023 The Tango Community
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
#include <yat/plugin/CPlugIn.h>
#include <yat/plugin/CPlugInSymbols.h>

namespace yat
{
//==============================================================================
// PlugIn::PlugIn
//==============================================================================
CPlugIn::CPlugIn( const std::string &library_file_name ) : PlugInBase(library_file_name)
{
  load_library(library_file_name);
}

//==============================================================================
// CPlugIn::~CPlugIn
//==============================================================================
CPlugIn::~CPlugIn()
{
  release_library();
}

//==============================================================================
// PlugIn::interface_name
//==============================================================================
yat::String CPlugIn::interface_name()
{
  Symbol symbol;
  try
  {
    symbol = find_symbol(kGetInterfaceNameCSymbol);
  }
  catch(yat::Exception& ex)
  {
    RETHROW_YAT_ERROR(ex,
                      "SHAREDLIBRARY_ERROR",
                      "Unable to find the 'get_interface_name' symbol",
                      "CPlugIn::interface_name");
  }

  get_interface_name_func_t get_interface_name_func = (get_interface_name_func_t)(symbol);

  const char* name = (*get_interface_name_func)();
  return yat::String(name);
}

//==============================================================================
// PlugIn::plugin_id
//==============================================================================
yat::String CPlugIn::plugin_id()
{
  Symbol symbol;
  try
  {
    symbol = find_symbol(kGetPlugInIdCSymbol);
  }
  catch(yat::Exception& ex)
  {
    RETHROW_YAT_ERROR(ex,
                      "SHAREDLIBRARY_ERROR",
                      "Unable to find the 'plugin_id' symbol",
                      "CPlugIn::interface_name");
  }

  get_plugin_id_func_t get_plugin_id_func = (get_plugin_id_func_t)(symbol);

  const char* id = (*get_plugin_id_func)();
  return yat::String(id);
}

//==============================================================================
// PlugIn::version
//==============================================================================
yat::String CPlugIn::version()
{
  Symbol symbol;
  try
  {
    symbol = find_symbol(kGetVersionCSymbol);
  }
  catch(yat::Exception& ex)
  {
    RETHROW_YAT_ERROR(ex,
                      "SHAREDLIBRARY_ERROR",
                      "Unable to find the 'version' symbol",
                      "CPlugIn::interface_name");
  }

  get_version_func_t get_version_func = (get_version_func_t)(symbol);

  const char* version = (*get_version_func)();
  return yat::String(version);
}

//==============================================================================
// CPlugIn::load_library
//==============================================================================
void CPlugIn::load_library(const std::string &library_file_name)
{
  try
  {
    release_library();
    m_libraryHandle = do_load_library( library_file_name );

    Symbol sym = find_symbol(kOnLoadCSymbol);

    on_load_func_t load_func = (on_load_func_t)(sym);
    load_func();
  }
  catch(yat::Exception& ex)
  {
    RETHROW_YAT_ERROR(ex,
                      "SHAREDLIBRARY_ERROR",
                      "Error while loading library",
                      "CPlugIn::load_library");
  }
  catch (...)
  {
    THROW_YAT_ERROR("SHAREDLIBRARY_ERROR",
                    "Unknown error while loading library",
                    "CPlugIn::load_library");
  }
}

//==============================================================================
// CPlugIn::release_library
//==============================================================================
void CPlugIn::release_library()
{
  try
  {
    if ( m_libraryHandle != NULL )
    {
      Symbol sym = find_symbol(kOnUnLoadCSymbol);

      on_unload_func_t unload_func = (on_unload_func_t)(sym);
      unload_func();

      do_release_library();
      m_libraryHandle = NULL;
    }
  }
  catch(yat::Exception& ex)
  {
    RETHROW_YAT_ERROR(ex,
                      "SHAREDLIBRARY_ERROR",
                      "Error while releasing library",
                      "PlugIn::release_library");
  }
  catch (...)
  {
    THROW_YAT_ERROR("SHAREDLIBRARY_ERROR",
                    "Unknown error while releasing library",
                    "PlugIn::release_library");
  }
}

}
