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

#ifndef _YAT_PLUGIN_H_
#define _YAT_PLUGIN_H_

// ============================================================================
//! \page plugPage Plugin documentation
//! \tableofcontents
//! The plugin utilities provide basic dynamic library manipulation functions.
//!
//! \section secP1 Plugin utilities
//! The plugin utilities is a platform independent layer to manage dynamic libraries.
//! They provide factory classes to construct "dynamic library" objects, and a plugin
//! manager to load/unload these libraries.
//!
//! \section secP2 Plugin classes
//! Links to plugin classes :
//!   - yat::PlugIn
//!   - yat::IPlugInFactory
//!   - yat::GenericFactory
//!   - yat::IPlugInInfo
//!   - yat::IPlugInObject
//!   - yat::IPlugInObjectWithAttr
//!   - yat::PlugInManager
//!   - yat::PlugInAttrInfo
//!   - yat::CPlugIn
//!   - yat::CPlugInManager
// ============================================================================

#include <yat/plugin/PlugInBase.h>
#include <yat/plugin/IPlugInInfo.h>
#include <yat/plugin/IPlugInFactory.h>

namespace yat
{

// ============================================================================
//! \class PlugIn
//! \brief The YAT PlugIn class.
//!
//! The PlugIn class provides a platform independent way to work with
//! dynamic libraries. It loads a specific dynamic library, and can return
//! specific symbol exported by the dynamic library.
//!
//! It is inspired (when not copied) from the CppUnit implementation
//! (see http://sourceforge.net/projects/cppunit).
//!
//! It can return a factory that constructs IPlugInObject implementations.
//!
// ============================================================================
class YAT_DECL PlugIn : public PlugInBase
{
public:

  //! \brief Loads the specified library.
  //! \param library_file_name Name of the library to load.
  //! \exception SHAREDLIBRARY_ERROR Thrown if a failure occurs while loading
  //! the library (fails to find or load the library).
  PlugIn( const std::string &library_file_name );

  //! \brief Releases the loaded library.
  ~PlugIn();

  //! \brief Returns information about the plugin.
  IPlugInInfo* info();

  //! \brief Returns the factory used for constructing IPlugInObject implementation.
  IPlugInFactory* factory();

private:
  //- Loads the specified library.
  //- \param library_file_name Name of the library to load.
  //- \exception DynamicLibraryManagerException if a failure occurs while loading
  //- the library (fail to found or load the library).
  void load_library( const std::string &library_file_name );

  //- Releases the loaded library.
  //-
  //- \warning Must NOT throw any exceptions (called from destructor).
  void release_library();

  //- Prevents the use of the copy constructor.
  PlugIn( const PlugIn &copy );

  //- Prevents the use of the copy operator.
  void operator =( const PlugIn &copy );
};

} // namespace

#endif
