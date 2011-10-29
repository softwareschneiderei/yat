/*!
 * \file     
 * \brief    An example of yat::CachedAllocator usage
 * \author   N. Leclerc - Synchrotron SOLEIL
 */

#include <iostream>
#include <deque>
#include <yat/memory/Allocator.h>
#include <yat/time/Timer.h>

#define USE_YAT_ALLOCATOR        1
#define USE_YAT_CACHED_ALLOCATOR 1

//-----------------------------------------------------------------------------
// MyClass
//-----------------------------------------------------------------------------
class MyClass
{  
public:
  
#if USE_YAT_ALLOCATOR
  
#if USE_YAT_CACHED_ALLOCATOR
  //- specialization of the yat::NewAllocator
  typedef yat::CachedAllocator<MyClass> Allocator;
#else
  //- specialization of the yat::CachedAllocator
  typedef yat::NewAllocator<MyClass> Allocator;
#endif
  
  //- overwrites the new operator
  inline void * operator new (size_t)
  {
    return (void *) cache->malloc();
  }
  
  //- overwrites the delete operator
  inline void operator delete (void * p)
  {
    cache->free(reinterpret_cast<MyClass*>(p));
  }
  
  //- init allocator
  static void init (size_t n = 100000)
  {
#if USE_YAT_CACHED_ALLOCATOR
  	cache = new Allocator(n);
#else
    cache = new Allocator;
#endif
  }
  
  //- release allocator
  static void fini ()
  {
  	delete cache;
    cache = 0;
  }
  
#endif //- USE_YAT_ALLOCATOR
  
  MyClass ()
  {
  	//- noop
  }

  ~MyClass()
  {
    //- noop 
  }

  //-------------------------------------------------
  // Members
  //-------------------------------------------------
  unsigned char mem_chunk[64];
  
  //-------------------------------------------------
  // Allocator
  //-------------------------------------------------
#if USE_YAT_ALLOCATOR
  static Allocator * cache;
#endif
};

#if USE_YAT_ALLOCATOR
MyClass::Allocator * MyClass::cache = 0;
#endif

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  size_t n = 10000000;
  
  yat::Timer t;
  
#if USE_YAT_ALLOCATOR
  
#if USE_YAT_CACHED_ALLOCATOR
  
  std::cout << "preallocating " 
            << n
            << " instances"
            << std::endl;
  
  t.restart();
#endif
  
  MyClass::init(n);
  
#if USE_YAT_CACHED_ALLOCATOR
  
  std::cout << "preallocation took " 
            << t.elapsed_sec()
            << " sec"
            << std::endl;
#endif
  
#endif //- USE_YAT_ALLOCATOR
  
  for (size_t it = 0; it < 10; ++it)
  {  
    std::deque<MyClass*> objs_repository;
  
    t.restart();
    
    for (size_t i = 0; i < n; ++i)
      objs_repository.push_back(new MyClass);
  
    std::cout << "new/push took " 
              << t.elapsed_usec() / n
              << " usec/instance"
              << std::endl;
  
    t.restart();
  
    for (size_t i = 0; i < n; ++i)
      delete objs_repository[i];
    
    objs_repository.clear();
  
    std::cout << "delete took " 
              << t.elapsed_usec() / n
              << " usec/instance"
              << std::endl;
  }
  
#if USE_YAT_ALLOCATOR
  
  t.restart();
  
  MyClass::fini();
  
  std::cout << "delete allocator " 
            << t.elapsed_sec()
            << " sec"
            << std::endl;
#endif
  
  return 0;
}
