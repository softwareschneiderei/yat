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
  typedef yat::SharedPtr<MyDerivedObject> MyDerivedObjectPtr;

  std::cout << STR(VERSION) << std::endl;
  
  MyObjectPtr foo ( new MyObject("foo-sp") );
 
  MyObjectPtr bar ( new MyObject("bar-sp") );

  MyObjectPtr tmp;

  std::cout << std::endl; //-----------------------------------------------------
  
  std::cout << "initial state:" << std::endl;
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl; //-----------------------------------------------------
  
  tmp = foo;
  std::cout << "after 'tmp = foo' :" << std::endl;
  
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl; //-----------------------------------------------------
  
  tmp = bar;
  
  std::cout << "after 'tmp = bar' :" << std::endl;
  
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl; //-----------------------------------------------------
  
  tmp.reset();
  
  std::cout << "after 'tmp.reset()' :" << std::endl;
  
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl; //-----------------------------------------------------
  
  tmp = foo; 
  
  foo.reset( new MyObject("oof-so") );
  
  std::cout << "after 'tmp = foo; foo.reset( new MyObject(\"oof-so\") )' :" << std::endl;
  
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl; //-----------------------------------------------------
  
  foo.reset();

  std::cout << "after 'foo.reset()' :" << std::endl;
  
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl; //-----------------------------------------------------

  MyDerivedObjectPtr foo2( new MyDerivedObject("foo2-sp"));
  
  foo = foo2;
  
  if( foo == foo2 ) ;
  
  return 0;
}
