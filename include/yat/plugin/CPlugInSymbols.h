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

#ifndef _YAT_C_PLUGIN_SYMBOLS_H_
#define _YAT_C_PLUGIN_SYMBOLS_H_

namespace yat
{

//! \brief Prototype for the exported "on load" symbol of a plugin.
typedef void        (*on_load_func_t)            (void);

//! \brief Prototype for the exported "on unload" symbol of a plugin.
typedef void        (*on_unload_func_t)          (void);

//! \brief Prototype for the exported "get interface name" symbol of a plugin.
typedef const char* (*get_interface_name_func_t) (void);

//! \brief Prototype for the exported "get plugin identifier" symbol of a plugin.
typedef const char* (*get_plugin_id_func_t)      (void);

//! \brief Prototype for the exported "get version" symbol of a plugin.
typedef const char* (*get_version_func_t)        (void);

//! \brief Name of the exported "on load" symbol of a C-plugin.
const std::string kOnLoadCSymbol           ("on_load");
//! \brief Name of the exported "on unload" symbol of a C-plugin.
const std::string kOnUnLoadCSymbol         ("on_unload");
//! \brief Name of the exported "get information" symbol of a C-plugin.
const std::string kGetInterfaceNameCSymbol ("get_interface_name");
//! \brief Name of the exported "get information" symbol of a C-plugin.
const std::string kGetPlugInIdCSymbol      ("get_plugin_id");
//! \brief Name of the exported "get version number" symbol of a C-plugin.
const std::string kGetVersionCSymbol       ("get_version");

} // namespace

#endif
