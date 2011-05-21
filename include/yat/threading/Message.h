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

#ifndef _YAT_MESSAGE_H_
#define _YAT_MESSAGE_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/CommonHeader.h>
#if defined(YAT_DEBUG)
# include <yat/time/Timer.h>
#endif
#include <yat/any/GenericContainer.h>
#include <yat/threading/SharedObject.h>
#include <yat/threading/Condition.h>
#include <yat/threading/Mutex.h>
#if defined(_USE_MSG_CACHE_)
# include <yat/memory/Allocator.h>
#endif

namespace yat
{

// ============================================================================
//     MSG PRIORITIES - MSG PRIORITIES - MSG PRIORITIES - MSG PRIORITIES  
// ============================================================================
//-----------------------------------------------------------------------------
//- HIGHEST PRIORITY
//-----------------------------------------------------------------------------
#define HIGHEST_MSG_PRIORITY 0xFFFF
//-----------------------------------------------------------------------------
//- LOWEST PRIORITY
//-----------------------------------------------------------------------------
#define LOWEST_MSG_PRIORITY 0
//-----------------------------------------------------------------------------
//- INIT MSG PRIORITY
//-----------------------------------------------------------------------------
#define INIT_MSG_PRIORITY HIGHEST_MSG_PRIORITY
//-----------------------------------------------------------------------------
//- EXIT MSG PRIORITY
//-----------------------------------------------------------------------------
#define EXIT_MSG_PRIORITY HIGHEST_MSG_PRIORITY
//-----------------------------------------------------------------------------
//- MAX (i.e.HIGHEST) USER PRIORITY
//-----------------------------------------------------------------------------
#define MAX_USER_PRIORITY (HIGHEST_MSG_PRIORITY - 20)
//-----------------------------------------------------------------------------
//- DEFAULT MSG PRIORITY
//-----------------------------------------------------------------------------
#define DEFAULT_MSG_PRIORITY LOWEST_MSG_PRIORITY
// ============================================================================

// ============================================================================
// enum: MessageType
// ============================================================================
typedef enum
{
  //--------------------
  TASK_INIT,
  TASK_TIMEOUT,
  TASK_PERIODIC,
  TASK_EXIT,
  //--------------------
  FIRST_USER_MSG
} MessageType;

// ============================================================================
//  class: Message
// ============================================================================
class YAT_DECL Message : private yat::SharedObject
{
#if defined(_USE_MSG_CACHE_)
  //- define what a message cache is
  typedef CachedAllocator<Message, Mutex> Cache;
  //- "global" message cache
  static Cache m_cache;
#endif

public:

#if defined (YAT_DEBUG)
  typedef unsigned long MessageID;
#endif

#if defined(_USE_MSG_CACHE_)
  //- overloads the new operator (makes class "cachable")
  void * operator new (size_t);

  //- overloads the delete operator (makes class "cachable")
  void operator delete (void *);

  //-TODO: impl this...
  static void pre_alloc (size_t _nobjs) throw (Exception) {};

  //-TODO: impl this...
  static void release_pre_alloc () {};
#endif

  //---------------------------------------------
  // Message::factory
  //---------------------------------------------
  static Message * allocate (size_t msg_type, 
                             size_t msg_priority = DEFAULT_MSG_PRIORITY,
                             bool waitable = false)
    throw (Exception);

  //---------------------------------------------
  // Message::ctor
  //---------------------------------------------
  explicit Message ();

  //---------------------------------------------
  // Message::ctor
  //---------------------------------------------
  explicit Message (size_t msg_type, 
                    size_t msg_priority = DEFAULT_MSG_PRIORITY,
                    bool waitable = false);

  //---------------------------------------------
  // Message::dtor
  //---------------------------------------------
  virtual ~Message ();

  //---------------------------------------------
  // Message::to_string
  //---------------------------------------------
  virtual const char * to_string () const;

  //---------------------------------------------
  // Message::is_task_ctrl_message
  //---------------------------------------------
  bool is_task_ctrl_message ();

  //---------------------------------------------
  // Message::duplicate
  //---------------------------------------------
  Message * duplicate ();

  //---------------------------------------------
  // Message::release
  //---------------------------------------------
  void release ();

  //---------------------------------------------
  // Message::type
  //---------------------------------------------
  size_t type () const;

  //---------------------------------------------
  // Message::type
  //---------------------------------------------
  void type (size_t t);

  //---------------------------------------------
  // Message::type
  //---------------------------------------------
  size_t priority () const;

  //---------------------------------------------
  // Message::type
  //---------------------------------------------
  void priority (size_t p);

  //---------------------------------------------
  // Message::user_data
  //---------------------------------------------
  void * user_data () const;

  //---------------------------------------------
  // Message::user_data
  //---------------------------------------------
  void user_data (void * ud);

  //---------------------------------------------
  // Message::size_in_bytes
  //---------------------------------------------
  size_t size_in_bytes () const;

  //---------------------------------------------
  // Message::user_data
  //---------------------------------------------
  void size_in_bytes (size_t s);
  
  //---------------------------------------------
  // Message::attach_data 
  //---------------------------------------------
  template <typename T> void attach_data (T * _data, bool _transfer_ownership = true)
    throw (Exception);

  //---------------------------------------------
  // Message::attach_data (makes a copy of _data)
  //---------------------------------------------
  template <typename T> void attach_data (const T & _data)
    throw (Exception);

  //---------------------------------------------
  // Message::get_data
  //---------------------------------------------
  template <typename T> T& get_data () const
    throw (Exception);

  //---------------------------------------------
  // Message::detach_data
  //---------------------------------------------
  template <typename T> void detach_data (T*& _data) const
    throw (Exception);

  //---------------------------------------------
  // Message::detach_data
  //---------------------------------------------
  template <typename T> T * detach_data () const;

  //---------------------------------------------
  // Message::check_attached_data_type
  //---------------------------------------------
  template <typename T> bool check_attached_data_type () const;

  //---------------------------------------------
  // Message::make_waitable
  //---------------------------------------------
  void make_waitable ()
    throw (Exception);

  //---------------------------------------------
  // Message::waitable
  //---------------------------------------------
  bool waitable () const;

  //---------------------------------------------
  // Message::wait_processed
  //---------------------------------------------
  //! Wait for the message to be processed by the associated consumer(s).
  //! Returns "false" in case the specified timeout expired before the 
  //! message was processed. Returns true otherwise. An exception is 
  //! thrown in case the message is not "waitable".
  bool wait_processed (unsigned long tmo_ms)
    throw (Exception);

  //---------------------------------------------
  // Message::processed
  //---------------------------------------------
  void processed ();

  //---------------------------------------------
  // Message::has_error
  //---------------------------------------------
  bool has_error () const;

  //---------------------------------------------
  // Message::set_error
  //---------------------------------------------
  void set_error (const Exception & e);

  //---------------------------------------------
  // Message::get_error
  //---------------------------------------------
  const Exception & get_error () const;

  //---------------------------------------------
  // Message::dump
  //---------------------------------------------
  virtual void dump () const;

  //---------------------------------------------
  // Message::id
  //---------------------------------------------
#if defined (YAT_DEBUG)
  MessageID id () const;
#endif

  //---------------------------------------------
  // Message::elapsed_msec
  //---------------------------------------------
#if defined (YAT_DEBUG)
  double elapsed_msec ();
#endif

  //---------------------------------------------
  // Message::restart_timer
  //---------------------------------------------
#if defined (YAT_DEBUG)
  void restart_timer ();
#endif

protected:
  //- msg processed
  bool processed_;

  //- the msg type
  size_t type_;

  //- the msg priority
  size_t priority_;

  //- the associated user data (same for all messages handled by a given task)
  void * user_data_;

  //- the associated msg data (specific to a given message)
  Container * msg_data_;

  //- true if an error occured during message handling
  bool has_error_;

  //- TANGO exception local storage
  Exception exception_;

  //- condition variable (for waitable msgs)
  Condition * cond_;

  //- size of message content in bytes
	size_t size_in_bytes_;

#if defined (YAT_DEBUG)
  //- msg id
  MessageID id_;
  //- timer: provides a way to evaluate <dt> between msg <post> and <processing>
  yat::Timer timer_;
  //- msg counter
  static MessageID msg_counter;
  //- ctor/dtor counters
  static size_t ctor_counter;
  static size_t dtor_counter;
#endif

  // = Disallow these operations.
  //--------------------------------------------
  Message & operator= (const Message &);
  Message (const Message &);
};

//---------------------------------------------
// Message::attach_data 
//---------------------------------------------
template <typename T> void Message::attach_data (T * _data, bool _transfer_ownership)
  throw (Exception)
{
  //- try to avoid GenericContainer<T> reallocation
  if (this->msg_data_)
  {
    //- is <msg_data_> content a <T>?
    if (! this->check_attached_data_type<T>())
    {
      delete this->msg_data_;
      this->msg_data_ = 0;
    }
  }
  //- no existing or deleted data
  if (! this->msg_data_)
  {
    //- (re)allocate the underlying generic container
    Container * md = new GenericContainer<T>(_data, _transfer_ownership);
    if (md == 0)
    {
      THROW_YAT_ERROR("OUT_OF_MEMORY",
                      "MessageData allocation failed",
                      "Message::attach_data");
    }
    //- point to <new> data
    this->msg_data_ = md;
  }
  //- simply change message GenericContainer<T> content
  else 
  {
    GenericContainer<T> * c = reinterpret_cast<GenericContainer<T>*>(this->msg_data_); 
    c->set_content(_data, _transfer_ownership);
  }
}

//---------------------------------------------
// Message::attach_data (makes a copy of _data)
//---------------------------------------------
template <typename T> void Message::attach_data (const T & _data)
  throw (Exception)
{
  //- try to avoid GenericContainer<T> reallocation
  if (this->msg_data_)
  {
    //- is <msg_data_> content a <T>?
    if (! this->check_attached_data_type<T>())
    {
      delete this->msg_data_;
      this->msg_data_ = 0;
    }
  }
  //- no existing or deleted data
  if (! this->msg_data_)
  {
    //- (re)allocate the underlying generic container
    Container * md = new GenericContainer<T>(_data);
    if (md == 0)
    {
      THROW_YAT_ERROR("OUT_OF_MEMORY",
                      "MessageData allocation failed",
                      "Message::attach_data");
    }
    //- point to <new> data
    this->msg_data_ = md;
  }
  //- simply change message GenericContainer<T> content
  else 
  {
    GenericContainer<T> * c = reinterpret_cast<GenericContainer<T>*>(this->msg_data_); 
    c->set_content(_data);
  }
}

//---------------------------------------------
// Message::get_data
//---------------------------------------------
template <typename T> T& Message::get_data () const
  throw (Exception)
{
  GenericContainer<T> * c = 0;
  try
  {
    c = dynamic_cast<GenericContainer<T>*>(this->msg_data_);
    if (c == 0)
    {
      THROW_YAT_ERROR("RUNTIME_ERROR",
                      "could not extract data from message [attached data type is not the requested type]",
                      "Message::get_data");
    }
  }
  catch(const std::bad_cast&)
  {
    THROW_YAT_ERROR("RUNTIME_ERROR",
                    "could not extract data from message [attached data type is not the requested type]",
                    "Message::get_data");
  }
  return c->get_content();
}

//---------------------------------------------
// Message::detach_data
//---------------------------------------------
template <typename T> void Message::detach_data (T*& _data) const
  throw (Exception)
{
  _data = yat::any_cast<T>(this->msg_data_, true);
  if (! _data)
  {
    THROW_YAT_ERROR("RUNTIME_ERROR",
                    "could not extract data from message [attached data type is not the requested type]",
                    "Message::detach_data");
  }
}

//---------------------------------------------
// Message::detach_data
//---------------------------------------------
template <typename T> T * Message::detach_data () const
{
  return yat::any_cast<T>(this->msg_data_, true);
}

//---------------------------------------------
// Message::check_attached_data_type
//---------------------------------------------
template <typename T> bool Message::check_attached_data_type () const
{
  return yat::any_cast<T>(this->msg_data_)
       ? true
       : false;
}

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/threading/Message.i>
#endif // YAT_INLINE_IMPL

#endif // _MESSAGE_H_
