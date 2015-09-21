//----------------------------------------------------------------------------
// Copyright (c) 2004-2015 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
/*!
 * \file     
 * \brief    An example of yat::Task (and related classes) usage. .
 * \author   N. Leclercq, J. Malik - Synchrotron SOLEIL
 */

#ifndef _MY_TASK_H_
#define _MY_TASK_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <iostream>
#include <yat/threading/Task.h>
#include <yat/memory/DataBuffer.h>

// ============================================================================
// SOME USER DEFINED MESSAGES
// ============================================================================
#define kDUMMY_MSG 10000
#define kDATA_MSG  10001

// ============================================================================
// SOME USER DEFINED MESSAGE PRIORITIES
// ============================================================================
#define kDUMMY_MSG_PRIORITY adtb::MAX_USER_PRIORITY

// ============================================================================
//  SharedBuffer: a thread safe shared data buffer 
// ============================================================================
class SharedBuffer : public yat::Buffer<double>, private yat::SharedObject
{
public:
  //- explicit public ctor
  explicit SharedBuffer ()
   : yat::Buffer<double>(), yat::SharedObject()
    {
      //- noop ctor
    }
   
  //- private dtor
  virtual ~SharedBuffer ()
    { 
      static size_t cnt = 0;
      std::cout << "SharedBuffer dtor called " << ++cnt << " times" << std::endl; 
    }
    
  //- returns a "shallow" copy of this shared object (avoids deep copy).
  //- increments the shared reference count by 1 (thread safe).
  SharedBuffer* duplicate ()
    { return reinterpret_cast<SharedBuffer*>(yat::SharedObject::duplicate()); }
  
  //- decrements the shared reference count by 1.
  //- in case it drops to 0 then self delete the instance (thread safe).
  void release ()
    { yat::SharedObject::release(); } 
    
  //- returns the current value of object's reference counter 
  int reference_count () const
    { return yat::SharedObject::reference_count(); } 
  
  //- lock the underlying buffer 
  void lock ()
    { yat::SharedObject::lock(); } 
    
  //- unlock the underlying buffer 
  void unlock ()
    { yat::SharedObject::unlock(); } 
};

// ============================================================================
//  AutoSharedBuffer: auto lock/unlock a SharedBuffer
// ============================================================================
class AutoSharedBuffer
{
public:
  //- ctor
  AutoSharedBuffer (SharedBuffer& sdb) 
   : m_sdb(sdb)
    { m_sdb.lock(); }
   
  //- dtor
  virtual ~AutoSharedBuffer ()
    { m_sdb.unlock(); }
  
private:
   SharedBuffer& m_sdb;
};

// ============================================================================
// class: Consumer
// ============================================================================
class Consumer: public yat::Task
{
public:

	//- ctor ---------------------------------
	Consumer (size_t lo_wm, size_t hi_wm);

	//- dtor ---------------------------------
	virtual ~Consumer (void);

protected:
	//- handle_message -----------------------
	virtual void handle_message (yat::Message& msg);

private:
  //- num of ctrl msg received
  unsigned long ctrl_msg_counter;
  //- num of user msg received
  unsigned long user_msg_counter;

#if defined (YAT_DEBUG)
  //- id of the last received msg
  yat::Message::MessageID last_msg_id;
  //- num of lost msg 
  unsigned long lost_msg_counter;
  //- num msg received in wrong order
  unsigned long wrong_order_msg_counter;
#endif

};

#endif // _MY_TASK_H_
