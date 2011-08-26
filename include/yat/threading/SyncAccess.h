//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2011  The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// Ramon Sune (ALBA) for the yat::Signal class 
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
 * \author S.Poirier - Synchrotron SOLEIL
 */

#ifndef __SYNCACCESS_H__
#define __SYNCACCESS_H__

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/CommonHeader.h>
#include <yat/threading/Thread.h>
#include <yat/threading/Mutex.h>
#include <yat/threading/Utilities.h>
#include <yat/threading/Task.h>
#include <map>

#define LOCK(ptr) yat::SyncAccess _lock(ptr);

namespace yat
{
//=============================================================================
/// Synchronize access to a object
//=============================================================================
class YAT_DECL SyncAccess
{
private:
  typedef void* obj_ptr;
  
	/// Synchronized object
	class SyncObject
	{
	public:
		void*          m_pObj;            // Pointer to the synchronized object
		uint32         m_uiThreadCount;	  // Thread access count
		uint32         m_uiLockCount;     // Current lock count
		yat::Mutex     m_Mutex;           // Locked by using thread
		yat::ThreadUID m_CurrentThreadId; // Thread  currently lock access
		
		SyncObject(void *ptr) : m_pObj(ptr), m_uiThreadCount(0), m_uiLockCount(0), 
		  m_CurrentThreadId(YAT_INVALID_THREAD_UID) { }
	};

	SyncObject *m_pSyncObj;
	
	static std::map<void *, SyncObject *> m_sSynObjMap;
	static yat::Mutex m_sLock; // To lock access on the above map

	void lock_obj(void *pObj);
	void unlock_obj(void *pObj);
	
public:
	/// c-tor
	/// @param pObj pointer on object or ressource
	SyncAccess(void *pObj);

	/// d-tor
	~SyncAccess();
};

} // namespace gdshare

#endif // __THREADUTIL_H__
