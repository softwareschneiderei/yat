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

#ifndef _YAT_C_PLUGIN_H_
#define _YAT_C_PLUGIN_H_

#include <yat/utils/String.h>
#include <yat/memory/SharedPtr.h>
#include <yat/plugin/PlugInBase.h>

namespace yat
{

// ============================================================================
//! \class CPlugIn
//! \brief The YAT CPlugIn class.
//!
//! The CPlugIn class provides a platform independent way to work with
//! pure C dynamic libraries. It loads a specific dynamic library, and can return
//! specific symbol exported by the dynamic library.
//!
//! Pure C plugins must implemement those functions to be recognized as plugins
//! by the YAT library:
//!
//! void on_load(void)
//! void on_unload(void)
//! const char* interface_name(void)
//! const char* plugin_id(void)
//! const char* version(void)
// ============================================================================
class YAT_DECL CPlugIn : public PlugInBase
{
public:

  //! \brief Loads the specified library.
  //! \param library_file_name Name of the library to load.
  //! \exception SHAREDLIBRARY_ERROR Thrown if a failure occurs while loading
  //! the library (fails to find or load the library).
  CPlugIn(const std::string& library_file_name);

  //! \brief Releases the loaded library.
  ~CPlugIn();

  //! \brief return the interface name
  String interface_name();

  //! \brief return the plugin id
  String plugin_id();

  //! \brief return the version string
  String version();

private:
  //- Loads the specified library.
  //- \param library_file_name Name of the library to load.
  //- \exception DynamicLibraryManagerException if a failure occurs while loading
  //- the library (fail to found or load the library).
  void load_library(const std::string& library_file_name);

  //- Releases the loaded library.
  //-
  //- \warning Must NOT throw any exceptions (called from destructor).
  void release_library();

  //- Prevents the use of the copy constructor.
  CPlugIn(const CPlugIn& copy);

  //- Prevents the use of the copy operator.
  void operator =(const CPlugIn& copy);
};

typedef yat::SharedPtr<CPlugIn> CPlugInPtr;

} // namespace

#endif
