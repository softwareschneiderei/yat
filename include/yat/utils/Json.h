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
//      Stephane Poirier
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------

#ifndef __YAT_JSON_H__
#define __YAT_JSON_H__

#include <yat/utils/PicoJson.h>
#include <yat/Exception.h>
#include <yat/utils/String.h>

namespace yat
{

  // ============================================================================
  //! \brief JSON value object
  // ============================================================================
  typedef picojson::value JSONvalue;

  // ============================================================================
  //! \class SimpleJSON
  //! \brief JSON parser using the picojson implementation
  //!
  //! Simple wrapper class on top on picojson implementation
  //! \verbatim
  //! SimpleJSONparser json;
  //! yat::String content("{\"a\":1}");
  //! JSONvalue value;
  //! try
  //! {
  //!   value = json.parse(content);
  //! }
  //! catch(yat::Exception& ex)
  //! {
  //! ...
  //! }
  //! \endverbatim
  // ============================================================================
  class YAT_DECL SimpleJSON
  {
    //! \brief Parse a JSON content
    //! \param value_p pointer to JSONvalue object that will contain the result
    //! \param input JSON content to parse
    //! \param throw_exeption if set can throw an exception, otherwise log on the ERROR stream
    //! \return boolean value 'true' if ok, otherwise 'false' (in throw_exception is not 'true')
    void parse(JSONvalue *value_p, const yat::String& input, bool throw_exception=true)
    {
      std::string err = picojson::parse(*value_p, input);
      if( !err.empty() )
        throw yat::Exception("JSON ERROR", std::string("JSON parsing error: ") + err, "yat::JsonValue::parse");
    }
  };

}

#endif
