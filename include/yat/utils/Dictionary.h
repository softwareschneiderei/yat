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
#ifndef _YAT_DICTIONARY_H_
#define _YAT_DICTIONARY_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <map>

#include <yat/utils/Logging.h>
#include <yat/utils/String.h>
#include <yat/utils/Optional.h>
#include <yat/memory/SharedPtr.h>
#include <yat/memory/UniquePtr.h>

namespace yat
{

// ============================================================================
//! \class Dictionary
//! \brief Map class with std::string as implicit key type and some
//! \brief useful specific methods
// ============================================================================
template<typename T>
class YAT_DECL Dictionary
{
public:

  typedef std::pair<const std::string, T> value_type;
  typedef typename std::map<std::string, T>::iterator iterator;
  typedef typename std::map<std::string, T>::const_iterator const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> reverse_const_iterator;

  //@{ std::map methods
  T& operator[]( const std::string& key )  { return m_map[key]; }
  const_iterator begin() const { return m_map.begin(); }
  iterator begin() { return m_map.begin(); }
  reverse_const_iterator rbegin() const { return m_map.rbegin(); }
  reverse_iterator rbegin() { return m_map.rbegin(); }
  const_iterator end() const { return m_map.end(); }
  iterator end() { return m_map.end(); }
  reverse_const_iterator rend() const { return m_map.rend(); }
  reverse_iterator rend() { return m_map.rend(); }
  bool empty() const { return m_map.empty(); }
  std::size_t size() const { return m_map.size(); }
  void clear() { m_map.clear(); }
  void erase(iterator it) { m_map.erase(it); }
  void erase(iterator first, iterator last) { m_map.erase(first, last); }
  std::size_t erase(const std::string& key) { return m_map.erase( key ); }
  std::size_t count(const std::string& key) { return m_map.count( key ); }
  iterator find(const std::string& key) { return m_map.find( key ); }
  const_iterator find(const std::string& key) const { return m_map.find( key ); }
  std::pair<iterator,bool> insert(const value_type& value) { return m_map.insert(value); }
  Dictionary<T>(const std::map<std::string, T>& other): m_map(other) {}
  Dictionary<T>(const Dictionary<T>& other): m_map(other.m_map) {}
  Dictionary<T>() {}
  //@}

  //@{ Specific methods

  /// d-tor
  virtual ~Dictionary() {}

  /// Returns a reference to the mapped value of the element with key equivalent to key.
  /// If no such element exists, an exception is thrown.
  T& at( const std::string& key )
  {
    iterator it = m_map.find( key );
    if( it != m_map.end() )
      return it->second;
    throw yat::Exception("NO_DATA", std::string("No such key: ") + key, "Dictionary::at");
  }
  const T& at( const std::string& key ) const
  {
    const_iterator cit = m_map.find( key );
    if( cit != m_map.end() )
      return cit->second;
    throw yat::Exception("NO_DATA", std::string("No such key: ") + key, "Dictionary::at");
  }

  /// Return an optional value
  /// \verbatim
  /// T value = my_dict.get(key).value_or(default_value);
  /// \endverbatim
  Optional<T> get(const std::string& key) const
  {
    const_iterator cit = m_map.find( key );
    if( cit != m_map.end() )
      return cit->second;
    return Optional<T>();
  }
  //@}

protected:
  std::map<std::string, T> m_map;
};

// ============================================================================
//! \class StringDictionary
//! Dictionary specialization with std::string value type
//! Define serialization/deserialization methods
// ============================================================================
class YAT_DECL StringDictionary: public Dictionary<std::string>
{
public:

  //! \brief constructor
  StringDictionary(): Dictionary<std::string>() { }

  //! \brief construct the dictionary from a vector
  StringDictionary(const std::vector<std::string>& vec, char sep)
  : Dictionary<std::string>()
  {
    from_vector(vec, sep);
  }

  //! \brief construct the dictionary from a single string
  StringDictionary(const std::string& s, char sep_pair, char sep_key)
  : Dictionary<std::string>()
  {
    from_string(s, sep_pair, sep_key);
  }

  //! \brief construct the dictionary from a <string, string> map
  StringDictionary(const std::map<std::string, std::string>& m)
  {
    m_map = m;
  }

  //! \brief initialize the dictionary from a vector
  void from_vector(const std::vector<std::string>& vec, char sep, bool key_lowercase=false)
  {
    for( std::size_t i = 0; i < vec.size(); ++i )
    {
      std::string k,v;
      yat::StringUtil::split( vec[i], sep, &k, &v );
      yat::StringUtil::trim( &v );
      yat::StringUtil::trim( &k );
      if( key_lowercase )
        yat::StringUtil::to_lower( &k );
      m_map[k] = v;
    }
  }

  //! \brief initialize the dictionary from a single string
  void from_string(const std::string& s, char sep_pair='\n', char sep_key=':', bool key_lowercase=false)
  {
    std::vector<std::string> vec;
    yat::StringUtil::split( s, sep_pair, &vec);
    from_vector( vec, sep_key, key_lowercase );
  }

  //! \brief serialization
  void to_vector(std::vector<std::string>* vec_p, char sep_key=':')
  {
    vec_p->clear();
    for( std::map<std::string, std::string>::const_iterator cit = m_map.begin();
         cit != m_map.end(); ++cit )
    {
      std::stringstream oss;
      oss << cit->first << sep_key << cit->second;
      vec_p->push_back(oss.str());
    }
  }

  //! \brief serialization
  void to_string(std::string* str_p, char sep_pair='\n', char sep_key=':')
  {
    str_p->clear();
    std::ostringstream oss;
    for( std::map<std::string, std::string>::const_iterator cit = m_map.begin();
         cit != m_map.end(); ++cit )
    {
      if( cit != m_map.begin() )
        oss << sep_pair;

      oss << cit->first << sep_key << cit->second;
    }
    *str_p = oss.str();
  }

  //| dump the dictionary content (for debug purposes)
  void dump()
  {
    for( std::map<std::string, std::string>::const_iterator cit = m_map.begin();
         cit != m_map.end(); ++cit )
    {
      YAT_VERBOSE_STREAM( cit->first << ": " << cit->second );
    }
  }

#ifdef YAT_WIN32
  // VC12 compilation bug work around: useless re-implementation of parent class method
  Optional<std::string> get(const std::string& key) const
  {
    const_iterator cit = m_map.find( key );
    if( cit != m_map.end() )
      return cit->second;
    return Optional<std::string>();
  }
#endif

  /// Return an optional value
  /// \verbatim
  /// int  i = my_dict.get_numeric<int>(key).value_or(default_value);
  /// bool b = my_dict.get_bool(key).value_or(default_value);
  /// \endverbatim
  template<typename T>
  Optional<T> get_numeric(const std::string& key) const
  {
    const_iterator cit = m_map.find( key );
    if( cit != m_map.end() )
    {
      return yat::String(cit->second).to_num<T>();
    }
    return Optional<T>();
  }
  Optional<bool> get_bool(const std::string& key) const
  {
    const_iterator cit = m_map.find( key );
    if( cit != m_map.end() )
    {
      if( yat::StringUtil::is_equal_no_case(cit->second, "true") ||
          yat::StringUtil::is_equal(cit->second, "1"))
        return true;
      if( yat::StringUtil::is_equal_no_case(cit->second, "false") ||
          yat::StringUtil::is_equal(cit->second, "0"))
        return false;
    }
    return Optional<bool>();
  }
  //@}
};

}

#endif // _YAT_DICTIONARY_H_

