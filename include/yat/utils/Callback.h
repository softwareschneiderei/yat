//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2011  N.Leclercq & The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// See AUTHORS file 
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
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */

#ifndef _YAT_CALLBACK_H_
#define _YAT_CALLBACK_H_

#include <yat/CommonHeader.h>

namespace yat 
{

// ============================================================================
// class: CallbackContainer (abstract)
// ============================================================================
template <typename P>
class CallbackContainer
{
public:
  //! destructor
  virtual ~CallbackContainer()
  {};

  //! do/exec/... operator with T as argument
  virtual void operator() (P) const = 0;

  //! deep copy
  virtual CallbackContainer<P>* clone () const = 0;

  //! comparation
  virtual bool is_equal (const CallbackContainer<P> *) const = 0;
};

// ============================================================================
// class: Callback
// ============================================================================
template <typename P> 
class Callback
{
public:
  //! constructor
  Callback (CallbackContainer<P>* c)
    : container(c)
  {};

  //! constructor
  Callback (const Callback<P>& c)
    : container(c.container ? c.container->clone() : 0)
  {};

  //! destructor
  ~Callback ()
  {
    delete this->container;
    this->container = 0;
  };

  //! assignment operator
  Callback<P>& operator= (const Callback<P>& callback)
  {
    if (this != &callback)
    {
      delete this->container;
      this->container =  callback.container ? callback.container->clone() : 0;
    }
    return *this;
  };

  //! do operation defined in body
  void operator() (P p) 
  {
    if (this->container)
      (*this->container)(p);
    else
      THROW_YAT_ERROR("NULL_POINTER", "null/empty callback called", "Callback::operator()");
  }
  
  //! empty callback?  
  bool is_empty () const
  {
    return this->container == 0;
  };

  //! comparation
  bool operator== (const Callback<P>& s) const
  {
    if (this->container == 0 && s.container == 0)
      return true;

    if (this->container == 0 || s.container == 0)
      return false;

    return this->container->is_equal(s.container);
  }

private:
  CallbackContainer<P>* container;
};

// ============================================================================
// class: FunctionCallbackContainer (CallbackContainer for 'free' functions)
// ============================================================================
template <typename P, class F>
class FunctionCallbackContainer : public CallbackContainer<P>
{
public:
  //! constructor taking a function pointer
  FunctionCallbackContainer (const F & function)
    : function_(function)
  {}

  //! execute operation: call the function
  void operator() (P p) const 
  { 
    if (function_) 
      function_(p);
  }

  //! comparation
  bool _is_equal (const FunctionCallbackContainer* s) const
  {
    return this->function_ == s->function_;
  }

  //! comparation
  bool is_equal (const yat::CallbackContainer<P>* s) const
  {
    const FunctionCallbackContainer* fcc = dynamic_cast<const FunctionCallbackContainer*>(s);
    
    return fcc ? this->_is_equal(fcc) : false;
  }

private:
  //! the callback function
  const F function_;

  //! no assignemt operator
  FunctionCallbackContainer& operator=(FunctionCallbackContainer&);
};

// ============================================================================
// class: MemberCallbackContainer (CallbackContainer for 'member' functions)
// ============================================================================
template <typename P, class C, class M>
class MemberCallbackContainer : public CallbackContainer<P>
{
public:
  //! Member function type
  typedef void (C::*PMEMBERFUNC)(P);


  //! constructor taking a function pointer
  MemberCallbackContainer (C& client, M member)
    : client_(client), member_(member)
  {}

  //! execute operation: call the function
  void operator() (P p) const 
  { 
    if (member_) 
      (client_.*member_)(p);
  }

  //! comparation
  bool _is_equal (const MemberCallbackContainer* s) const
  {
    if (&this->client_ != &s->client_)
      return false;

    return member_ == s->member_;
  }

  //! comparation
  bool is_equal (const yat::CallbackContainer<P>* s) const
  {
    const MemberCallbackContainer* mcc = dynamic_cast<const MemberCallbackContainer*>(s);

    return mcc ? this->_is_equal(mcc) : false;
  }

private:
  //! The object the method function belongs to
  C& client_;

  //! The method to call
  PMEMBERFUNC member_;

  //! no assignement operator
  MemberCallbackContainer& operator=(MemberCallbackContainer&);
};

// ============================================================================
// YAT_DEFINE_CALLBACK: the magical macro 
// ============================================================================
#define YAT_DEFINE_CALLBACK(CallbackClass, CallbackArg) \
  template <typename Function> \
  class Function_##CallbackClass##Container : public yat::FunctionCallbackContainer<CallbackArg, Function> \
  { \
    typedef yat::FunctionCallbackContainer<CallbackArg, Function> InHerited; \
  public: \
    Function_##CallbackClass##Container (Function f) \
     : InHerited(f) \
    {} \
    yat::CallbackContainer<CallbackArg>* clone() const \
    { \
      return new Function_##CallbackClass##Container(*this); \
    } \
  }; \
  template <typename Client, class Member> \
  class Member_##CallbackClass##Container : public yat::MemberCallbackContainer<CallbackArg, Client, Member> \
  { \
    typedef yat::MemberCallbackContainer<CallbackArg, Client, Member> InHerited; \
  public: \
    Member_##CallbackClass##Container (Client& c, Member m) \
      : InHerited(c, m) \
    {}\
    yat::CallbackContainer<CallbackArg>* clone() const \
    { \
      return new Member_##CallbackClass##Container(*this); \
    } \
  }; \
  class CallbackClass : public yat::Callback<CallbackArg> \
  { \
    typedef yat::Callback<CallbackArg> InHerited; \
  public: \
    CallbackClass (yat::CallbackContainer<CallbackArg>* cc = 0) \
      : InHerited(cc) \
    {} \
    CallbackClass (const CallbackClass& cb) \
      : InHerited(cb) \
    {} \
    CallbackClass& operator= (const CallbackClass& cb) \
    { \
      InHerited::operator =(cb); \
      return *this; \
    } \
    template <typename Function> \
    static CallbackClass instanciate (Function f) \
    { \
      return new Function_##CallbackClass##Container<Function>(f); \
    } \
    template <typename Client, class Member> \
    static CallbackClass instanciate (Client& c, Member m) \
    { \
      return new Member_##CallbackClass##Container<Client, Member>(c, m); \
    } \
  }; 

} // namespace

#endif



