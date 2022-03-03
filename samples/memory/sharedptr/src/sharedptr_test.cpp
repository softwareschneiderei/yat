//----------------------------------------------------------------------------
// Copyright (c) 2004-2021 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
/*!
 * \file
 * \brief    An example of yat::SharedPtr usage
 * \author   N. Leclercq, J. Malik - Synchrotron SOLEIL
 */

#include <iostream>
#include <yat/memory/SharedPtr.h>

//-----------------------------------------------------------------------------
// MyObject
//-----------------------------------------------------------------------------
class MyObject
{
public:
  MyObject ( const std::string& s )
    : some_attribute(s)
  {
  	std::cout << "MyObject::calling ctor for " << some_attribute << std::endl;
  }

  ~MyObject()
  {
    std::cout << "MyObject::calling dtor for " << some_attribute << std::endl;
  }

  std::string some_attribute;
};

class MyDerivedObject : public MyObject
{
public:
  MyDerivedObject ( const std::string& s ) : MyObject(s)
  {
  	std::cout << "MyDerivedObject::calling ctor" << std::endl;
  }
  ~MyDerivedObject()
  {
    std::cout << "MyDerivedObject::calling dtor" << std::endl;
  }
};


//-----------------------------------------------------------------------------
// DUMP MACRO
//-----------------------------------------------------------------------------
#define DUMP( ptr ) \
  std::cout << "SharedPtr " \
            << #ptr \
            << " -- points to --> " \
            << (ptr ? ptr->some_attribute : "xxxxxx") \
            << " [its ref. count is " \
            << ptr.use_count() \
            << "]" \
            << std::endl

//-----------------------------------------------------------------------------
// DUMP MACRO
//-----------------------------------------------------------------------------
#define DUMPW( ptr, T ) \
  std::cout << "SharedPtr " \
  					<< #ptr \
            << " -- points to --> " \
            << (ptr ? T(ptr)->some_attribute : "xxxxxx") \
            << " [its ref. count is " \
            << ptr.use_count() \
            << "]" \
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
  typedef yat::SharedPtr<MyObject> MyObjectPtr;
  typedef yat::WeakPtr<MyObject> MyObjectWPtr;

  typedef yat::SharedPtr<MyDerivedObject> MyDerivedObjectPtr;
  typedef yat::WeakPtr<MyDerivedObject> MyDerivedObjectWPtr;

  std::cout << STR(VERSION) << std::endl;

  MyObjectPtr foo ( new MyObject("foo-sp") );
  MyObjectPtr bar ( new MyObject("bar-sp") );
  MyObjectPtr tmp;
  MyObjectWPtr wfoo(foo);

  MyDerivedObjectPtr bu(new MyDerivedObject("bu-sp"));
  MyDerivedObjectWPtr wbu (bu);
  MyObjectPtr guzo(wbu);

  DUMP(guzo);

  {
    yat::AutoMutex<MyObjectPtr> _lock(foo);
  }


  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "initial state:" << std::endl;
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute 'tmp = foo' :" << std::endl;
  tmp = foo;
  std::cout << "\nafter 'tmp = foo' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute 'tmp = bar' :" << std::endl;
  tmp = bar;

  std::cout << "\nafter 'tmp = bar' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute 'tmp.reset()' :" << std::endl;
  tmp.reset();

  std::cout << "\nafter 'tmp.reset()' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  tmp = foo;

  std::cout << "execute 'tmp = foo; foo.reset( new MyObject(\"oof-so\") )' :" << std::endl;
  foo.reset( new MyObject("oof-so") );

  std::cout << "\nafter 'tmp = foo; foo.reset( new MyObject(\"oof-so\") )' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute 'foo.reset()' :" << std::endl;
  foo.reset();

  std::cout << "\nafter 'foo.reset()' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute 'MyObjectWPtr wzo(wfoo)' :" << std::endl;
  MyObjectWPtr wzo(wfoo);

  std::cout << "\nafter 'MyObjectWPtr wzo(wfoo)' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );
  DUMPW( wzo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute 'wfoo.reset()' :" << std::endl;
  wfoo.reset();

  std::cout << "\nafter 'wfoo.reset()' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );
  DUMPW( wzo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute 'tmp.reset()' :" << std::endl;
  tmp.reset();

  std::cout << "\nafter 'tmp.reset()' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );
  DUMPW( wzo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute 'wzo.reset()' :" << std::endl;
  wzo.reset();

  std::cout << "\nafter 'wzo.reset()' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );
  DUMPW( wzo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute 'MyDerivedObjectPtr foo2( new MyDerivedObject(\"foo2-sp\"))' :" << std::endl;
  MyDerivedObjectPtr foo2( new MyDerivedObject("foo2-sp") );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute 'foo = foo2' :" << std::endl;
  foo = foo2;
  std::cout << "\nafter 'MyDerivedObjectPtr foo2( new MyDerivedObject(\"foo2-sp\")); foo = foo2' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( foo2 );
  DUMP( bar );
  DUMPW( wfoo, MyObjectPtr );

  std::cout << std::endl; //-----------------------------------------------------

  std::cout << "execute MyObjectWPtr meu( foo2 )' :" << std::endl;
  MyObjectWPtr meu( foo2 );

  std::cout << "\nafter MyObjectWPtr meu( foo2 )' :" << std::endl;

  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );
  DUMP( foo2 );
  DUMPW( wfoo, MyObjectPtr );
  DUMPW( meu, MyObjectPtr );

  return 0;
}
