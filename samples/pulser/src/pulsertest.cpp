//----------------------------------------------------------------------------
// Copyright (c) 2004-2021 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
/*!
 * \file
 * \brief    A yat::Pulser example.
 * \author   N. Leclercq - Synchrotron SOLEIL
 */

#include <iostream>
#include <limits>

#include "yat/utils/Logging.h"
#include "yat/threading/Pulser.h"

//-----------------------------------------------------------------------------
// MyClass
//------------------------------------------------------------------------
class MyClass
{
public:
  MyClass() : calls(0) {}

  void my_callback (yat::Thread::IOArg arg)
  {
    std::cout << "MyClass::my_callback::call #" << ++calls << std::endl;
  }

  size_t calls;
};

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------
int main (int, char**)
{
  try
  {
   //- MyClass instance
   MyClass mc;

   //- pulser's config
   yat::Pulser::Config cfg;
   cfg.period_in_msecs = 250;
   cfg.num_pulses = 10;
   cfg.callback = yat::PulserCallback::instanciate(mc, &MyClass::my_callback);
   cfg.user_data = 0;

   yat::Pulser p(cfg);
   std::cout << p.get_num_pulses() << std::endl;
   std::cout << p.get_period() << std::endl;
   p.start();

   do
   {
      yat::Thread::sleep(100);
   }
   while( p.is_running() );

   std::cout << "done!" << std::endl;
  }
  catch (const yat::Exception& e)
  {
    YAT_LOG_EXCEPTION(e);
  }
  catch (...)
  {
    std::cout << "Unknown exception caught" << std::endl;
  }

	return 0;
}

