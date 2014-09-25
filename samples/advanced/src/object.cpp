//----------------------------------------------------------------------------
// Copyright (c) 2004-2014 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
/*!
 * \file     
 * \brief    An advanced yat example
 * \author   N. Leclercq - Synchrotron SOLEIL
 */

// ============================================================================
// Dependencies
// ============================================================================
#include "object.h"

// ============================================================================
// Object static members
// ============================================================================
size_t Object::inst_cnt = 0;
yat::CachedAllocator<Object> * Object::cache = 0;


