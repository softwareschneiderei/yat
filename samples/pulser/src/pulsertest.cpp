/*!
 * \file     
 * \brief    A yat::Pulser example.
 * \author   N. Leclercq - Synchrotron SOLEIL
 */

#include <iostream>
#include <limits>

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
   p.start();

   yat::Thread::sleep(30000);

   std::cout << "done!" << std::endl;
  }
  catch (...)
  {
    std::cout << "Unknown exception caught" << std::endl;
  }

	return 0;  
}

