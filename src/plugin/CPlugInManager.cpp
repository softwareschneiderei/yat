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
#include <yat/plugin/CPlugInManager.h>

namespace yat
{

//==============================================================================
// CPlugInManager::CPlugInManager
//==============================================================================
CPlugInManager::CPlugInManager()
{
}

//==============================================================================
// CPlugInManager::~CPlugInManager
//==============================================================================
CPlugInManager::~CPlugInManager()
{
  unload_all();
}

//==============================================================================
// CPlugInManager::load (new version)
//==============================================================================
void CPlugInManager::load(const std::string &library_file_name, Entry* entry_p)
{
  if( !entry_p )
    THROW_YAT_ERROR("NULL_POINTER",
                    "Caller must pass a valid pointer to CPlugInManager::PlugInEntry "
                    "structure",
                    "CPlugInManager::load");

  //- first verify that the plugin is not already loaded
  for( CPlugIns::iterator it = m_plugIns.begin(); it != m_plugIns.end(); ++it )
  {
    if( (*it).fileName == library_file_name )
    {
      // Already loaded !
      *entry_p = *it;
      return;
    }
  }

  //- ok, does not already exist : load it
  (*entry_p).fileName = library_file_name;
  (*entry_p).plugin_ptr = new CPlugIn(library_file_name);
  (*entry_p).interface_name = (*entry_p).plugin_ptr->interface_name();
  (*entry_p).plugin_id = (*entry_p).plugin_ptr->plugin_id();
  (*entry_p).version = (*entry_p).plugin_ptr->version();

  m_plugIns.push_back( *entry_p );
}

//==============================================================================
// CPlugInManager::unload
//==============================================================================
void CPlugInManager::unload(const std::string &library_file_name)
{
  if( m_plugIns.empty() )
    return;

  for( CPlugIns::iterator it = m_plugIns.begin(); it != m_plugIns.end(); ++it )
  {
    if ( (*it).fileName == library_file_name )
    {
      unload( *it );
      m_plugIns.erase( it );
      break;
    }
  }
}

//==============================================================================
// CPlugInManager::unload_all
//==============================================================================
void CPlugInManager::unload_all()
{
  if( m_plugIns.empty() )
    return;

  for( CPlugIns::iterator it = m_plugIns.begin(); it != m_plugIns.end(); ++it )
  {
    unload( *it );
  }
  m_plugIns.clear();
}

//==============================================================================
// CPlugInManager::unload
//==============================================================================
void CPlugInManager::unload(Entry &plugin_info)
{
  plugin_info.plugin_ptr.reset();
}

}
