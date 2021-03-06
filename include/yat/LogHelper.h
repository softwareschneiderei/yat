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

#ifndef _YAT_LOGHELPER_H_
#define _YAT_LOGHELPER_H_

/*!
 *  Define the YAT_LOG & YAT_TRACE helper macro depending on the YAT_ENABLE_LOG macro
 *  These macros can be used by yat users to log in their applications
 */
#if defined (YAT_ENABLE_LOG)

#  include <iostream>


# if defined (YAT_ENABLE_TRACE)
    namespace yat
    {
      class YAT_DECL TraceHelper
      {
      public:
        TraceHelper(const char* _func_name, const void * _this = 0 )
          :  instance(_this), func_name(_func_name)
        {
          std::cout << func_name
                    << " [this::"
                    << std::hex
                    << instance
                    << std::dec
                    << "] <-"
                    << std::endl;
        };

        ~TraceHelper()
        {
          std::cout << func_name
                    << " [this::"
                    << std::hex
                    << instance
                    << std::dec
                    << "] ->"
                    << std::endl;
        };

      private:
        const void * instance;
        const char* func_name;
      public:
      };
    }
#   define YAT_TRACE(func_name) \
      yat::TraceHelper yat_trace_helper( (func_name), this )
#   define YAT_TRACE_STATIC(func_name) \
      yat::TraceHelper yat_trace_helper( (func_name) )
# else
#   define YAT_TRACE(func_name)
#   define YAT_TRACE_STATIC(func_name)
# endif

#  define YAT_LOG(s) \
    do \
    { \
      std::cout << "[this:" \
                 << std::hex \
                 << (void *)this \
                 << std::dec \
                 << "]::" \
                 << __FUNCTION__ \
                 << "::" \
                 << s \
                 << std::endl; \
    } while (0)

#  define YAT_LOG_STATIC(s) \
    do \
    { \
      std::cout << s << std::endl; \
    } while(0)

#else

# define YAT_LOG(x)
# define YAT_LOG_STATIC(x)
# define YAT_TRACE(x)
# define YAT_TRACE_STATIC(x)

#endif


#endif

