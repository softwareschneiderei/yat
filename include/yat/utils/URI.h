//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2012  The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// see AUTHORS file 
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
/*!
 * \author S.Poirier - Synchrotron SOLEIL
 */


#ifndef __YAT_URI_H__
#define __YAT_URI_H__

#include <map>
#include <yat/utils/String.h>

namespace yat
{
//===========================================================================
/// Unified Ressource Identifier
/// This class is aimed to allow parsing and building URLs
/// The URI
/// URI syntax is defined by RFC 3986
/// The basic syntax is:
///   scheme ":" hier-part [ "?" query ] [ "#" fragment ]
//===========================================================================
class YAT_DECL URI
{
public:
  enum Part
  {
    SCHEME = 0,
    AUTHORITY,
    USERINFO,
    HOST,
    PORT,
    PATH,
    QUERY,
    FRAGMENT
  };
  
  struct Fields
  {
    String scheme;
    String userinfo;
    String host;
    String port;
    String path;
    String query;
    String fragment;
  };
  
private:
  std::map<Part, String> m_part;
  
  void parse(const String& URI) throw ( Exception );
  String value(Part part) const;
  static bool check_value(const String& value, const String &accepted_chars, const String& error_desc, bool throw_exception) throw ( Exception );
  static void split_authority(const String& authority, String* userinfo_ptr, String* host_ptr, String* port_ptr);
  static bool check_authority(const String& authority, URI::Fields* fields_ptr, bool throw_exception) throw ( Exception );

public:
  /// c-tor using the regular representation of the URL: a string !
  URI(const String& uri_string) throw ( Exception );
  
  /// c-tor using each fields
  URI(const URI::Fields& fields) throw ( Exception );
  
  /// Empty c-tor
  URI() { }
  
  /// Get the URI as a string
  String get() const;
  
  /// Gets a specific part of the URI
  ///
  /// @param part The URI part looked for
  /// @return a string
  ///
  String get(Part part) const;

  /// Sets a specific part
  /// 
  /// @param part The URI part looked for
  /// @param associated value
  /// @throw yat::Exception if ther syntax of the value is wrong
  ///
  void set(Part part, const String &value) throw ( Exception ); 
  
  /// Sets value
  /// 
  /// @param associated value
  ///
  void set(const String &value) throw ( Exception ); 
  
  /// Check the syntax of a value associated with a URI part
  ///
  /// @param part
  /// @param associated value
  /// @return true if ok, otherwise false
  static bool check(Part part, const String &value, bool throw_exception=false) throw ( Exception );
};


} // namespace

#endif