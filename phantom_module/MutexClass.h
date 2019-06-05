// MutexClass.h: interface for the CMutexClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTEXCLASS_H__69C13777_FF9E_4972_BFCA_475700687AD1__INCLUDED_)
#define AFX_MUTEXCLASS_H__69C13777_FF9E_4972_BFCA_475700687AD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef MUTEX_CLASS
#define MUTEX_CLASS

#ifndef WINDOWS
#if defined(WIN32) || defined(WIN64)
#define WINDOWS
#endif
#endif

#ifndef WINDOWS
#include <pthread.h>
#endif
#include "Thread.h"

class CMutexClass
{
private:
#ifdef WINDOWS
	HANDLE m_mutex;
#else
	pthread_mutex_t m_mutex;
#endif
	ThreadId_t m_owner;
public:
	BOOL m_bCreated;
	void Lock();
	void Unlock();
	CMutexClass(void);
	~CMutexClass(void);
};
#endif
#endif // !defined(AFX_MUTEXCLASS_H__69C13777_FF9E_4972_BFCA_475700687AD1__INCLUDED_)
