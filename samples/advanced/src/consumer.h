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

#pragma once

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <iostream>
#include <yat/threading/Task.h>

// ============================================================================
// class: Consumer
// ============================================================================
class Consumer: public yat::Task
{
public:

	//- ctor ---------------------------------
	Consumer ();

	//- dtor ---------------------------------
	virtual ~Consumer (void);

protected:
	//- handle_message -----------------------
	virtual void handle_message (yat::Message& msg);

private:
  size_t index;
  
  void parse_objects (size_t no);
};
