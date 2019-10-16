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
// Copyright (C) 2006-2016 The Tango Community
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

/// ------------------------------------------
/// Version::set
/// ------------------------------------------
void Version::set(const std::string& name, const std::string& version)
{
  instance().m_main.name = name;
  instance().m_main.version = version;

  clear();
  add_dependency( "YAT", YAT_XSTR(YAT_PROJECT_VERSION) );
};

/// ------------------------------------------
/// Version::add_dependency
/// ------------------------------------------
void Version::add_dependency(const std::string& name, const std::string& version)
{
  instance().m_dependencies.push_back(Dependency(name, version));
};

void Version::add_dependency_module(const std::string& name, const std::string& version)
{
  instance().m_dependencies.back().modules.push_back(Module(name, version));
};

/// ------------------------------------------
/// Version::get
/// ------------------------------------------
std::string Version::get()
{
  if( instance().m_main.name.empty() )
    return "No project information provided";

  std::ostringstream oss;
  std::size_t w = 16;
  oss.width(w);
  oss << std::left << "Project" << ": " << instance().m_main.name << std::endl;
  oss.width(w);
  oss << std::left << "Version" << ": " << instance().m_main.version << std::endl;

  for( std::size_t d = 0; d < instance().m_dependencies.size(); ++d )
  {
    oss << "-----" << std::endl;
    oss.width(w);
    oss << std::left << "Dependency" << ": " << instance().m_dependencies[d].name << std::endl;
    oss.width(w);
    oss << std::left << "Version" << ": " << instance().m_dependencies[d].version << std::endl;
    for( std::size_t m = 0; m < instance().m_dependencies[d].modules.size(); ++m )
    {
      oss.width(w);
      oss << std::left
          << instance().m_dependencies[d].modules[m].name << ": "
          << instance().m_dependencies[d].modules[m].version << std::endl;
    }
  }

  return oss.str();
};

/// ------------------------------------------
/// Version::clear
/// ------------------------------------------
void Version::clear()
{
  instance().m_dependencies.clear();
};

} // namespace
