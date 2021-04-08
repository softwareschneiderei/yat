//----------------------------------------------------------------------------
// Copyright (c) 2004-2021 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
/*!
 * \file
 * \brief    An example of yat::UniquePtr usage
 * \author   N. Leclercq, J. Malik - Synchrotron SOLEIL
 */

#include <iostream>
#include <yat/memory/UniquePtr.h>
#include <yat/utils/URI.h>

//-----------------------------------------------------------------------------
// MyObject
//-----------------------------------------------------------------------------
class MyObject
{
public:
  MyObject ( const std::string& s )
    : some_attribute(s)
  {
    std::cout << "MyObject::MyObject( " << some_attribute << " )" << std::endl;
  }

  virtual ~MyObject()
  {
    std::cout << "MyObject::~MyObject " << some_attribute << std::endl;
  }

  std::string some_attribute;
};

class MyDerivedObject : public MyObject
{
public:
  MyDerivedObject ( const std::string& s ) : MyObject(s)
  {
  	std::cout << "MyDerivedObject::MyDerivedObject( " << s << " )" << std::endl;
  }
  ~MyDerivedObject()
  {
    std::cout << "MyDerivedObject::~MyDerivedObject " << some_attribute << std::endl;
  }
};

// ============================================================================
//! \struct MyObjectDeleter
//! \brief 'Deleter' object to instanciate when using yat::SharedPtr<yat::Task>
// ============================================================================
struct MyObjectDeleter
{
  //! \brief operator().
  //! \param object The object to delete.
  void operator()(MyObject* p)
  {
    try
    {
      std::cout << "MyObject deleter called for " << p->some_attribute << std::endl;
      delete p;
    }
    catch(...) {}
  }
};



//-----------------------------------------------------------------------------
// DUMP MACRO
//-----------------------------------------------------------------------------
#define DUMP( ptr ) \
  std::cout << "UniquePtr " \
  					<< #ptr \
            << " -- points to --> " \
            << (ptr ? ptr->some_attribute : "xxxxxx") \
            << std::endl

// Expanding Macros into string constants
// The STR macro calls the STR_EXPAND macro with its argument. The parameter is checked for macro
// expansions and evaluated by the preprocessor before being passed to STR_EXPAND which quotes it
#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  std::string toto("_#1");
  std::cout << toto << std::endl;
  yat::URI::pct_encode(&toto);
  std::cout << toto << std::endl;
  yat::URI::pct_decode(&toto);
  std::cout << toto << std::endl;

  typedef yat::UniquePtr<MyObject> MyObjectPtr;
  typedef yat::UniquePtr<MyDerivedObject> MyDerivedObjectPtr;

  std::cout << "start ---" << std::endl;
  std::cout << "-> yat::UniquePtr<MyObject, MyObjectDeleter> ga( new MyObject( ga ) )" << std::endl;
  std::cout << "-> yat::UniquePtr<MyObject, MyObjectDeleter> bu" << std::endl;
  std::cout << "-> yat::UniquePtr<MyObject, MyObjectDeleter> zo" << std::endl;

  yat::UniquePtr<MyObject, MyObjectDeleter> ga( new MyObject( "ga" ) );
  yat::UniquePtr<MyObject, MyObjectDeleter> bu;
  yat::UniquePtr<MyObject, MyObjectDeleter> zo;

  std::cout << "initial state:" << std::endl;
  DUMP( ga );
  DUMP( bu );
  DUMP( zo );
  std::cout << std::endl;

  std::cout << "-> bu.reset( new MyObject( bu ) )" << std::endl;
  bu.reset( new MyObject("bu") );
  std::cout << "after bu.reset( new MyObject( bu ) )" << std::endl;
  DUMP( ga );
  DUMP( bu );
  DUMP( zo );
  std::cout << std::endl;

  std::cout << "-> yat::move(ga, zo)" << std::endl;
  yat::move(ga, zo);
  std::cout << "after yat::move(ga, zo)" << std::endl;
  DUMP( ga );
  DUMP( bu );
  DUMP( zo );
  std::cout << std::endl;

  std::cout << "-> std::vector< yat::UniquePtr<MyObject, MyObjectDeleter> > meu" << std::endl
            << "   meu.push_back(zo)" << std::endl;
  std::vector< yat::UniquePtr<MyObject, MyObjectDeleter> > meu;
  meu.push_back(zo);
  std::cout << "after std::vector< yat::UniquePtr<MyObject, MyObjectDeleter> > meu" << std::endl
            << "      meu.push_back(zo)" << std::endl;

  DUMP( ga );
  DUMP( bu );
  DUMP( zo );
  DUMP( meu[0] );
  std::cout << std::endl;

  std::cout << "-> yat::UniquePtr<MyObject> zomeu( new MyDerivedObject( zomeu ) )" << std::endl;
  yat::UniquePtr<MyObject> zomeu( new MyDerivedObject("zomeu") );
  std::cout << "after yat::UniquePtr<MyObject> zomeu( new MyDerivedObject( zomeu ) )" << std::endl;
  DUMP( ga );
  DUMP( bu );
  DUMP( zo );
  DUMP( meu[0] );
  DUMP( zomeu );
  std::cout << std::endl;

  std::cout << "end ---" << std::endl;
  return 0;
}
