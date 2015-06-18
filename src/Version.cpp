//----------------------------------------------------------------------------
// Copyright (c) 2004-2015 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2012  N.Leclercq & The Tango Community
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
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------

#include <sstream>

#include <yat/Version.h>
#include <yat/Exception.h>

namespace yat
{

std::vector<Version::Module> Version::s_dependencies;
Version::Module Version::s_main_module;

/// ------------------------------------------
/// Version::set
/// ------------------------------------------
void Version::set(const std::string& name, const std::string& version)
{
  s_main_module.name = name;
  s_main_module.version = version;
 
  s_dependencies.push_back( Module("YAT", YAT_XSTR(YAT_PROJECT_VERSION)) );
};

/// ------------------------------------------
/// Version::add_dependency
/// ------------------------------------------
void Version::add_dependency(const std::string& name, const std::string& version)
{
  s_dependencies.push_back( Module(name, version) );
};

/// ------------------------------------------
/// Version::get
/// ------------------------------------------
std::string Version::get()
{
  if( s_main_module.name.empty() )
    return "No project information provided";

  std::ostringstream oss;

  oss << "Project     : " << s_main_module.name << std::endl;
  oss << "Version     : " << s_main_module.version << std::endl;

  for(std::size_t i = 0; i < s_dependencies.size(); ++i )
  {
    oss << "-----" << std::endl;
    oss << "Dependency  : " << s_dependencies[i].name << std::endl;
    oss << "Version     : " << s_dependencies[i].version << std::endl;
  }

  return oss.str();
};

} // namespace
