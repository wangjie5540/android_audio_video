/******************************************************************************
* Copyright (c) 2012, Letion
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of California, Berkeley nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
///============================================================================
/// \file    : mutex.h
/// \brief   : ÁÙ½çËø
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-16
///============================================================================
#ifndef	__MATRIX_MUTEX_H__
#define __MATRIX_MUTEX_H__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif	//_XNIX

//=============================================================================
// class mutex
class mutex
{
public:
	mutex()
	{
#ifdef _WIN32
		InitializeCriticalSection(&m_oSection);
#else
		pthread_mutexattr_t attr;   
		pthread_mutexattr_init(&attr);   
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&m_hMutex,&attr);
		pthread_mutexattr_destroy(&attr);
#endif
	};

	~mutex()
	{
#ifdef WIN32
		DeleteCriticalSection(&m_oSection);
#else
		pthread_mutex_destroy(&m_hMutex);
#endif
	}

	void Lock()
	{
#ifdef WIN32
		EnterCriticalSection(&m_oSection);
#else
		pthread_mutex_lock(&m_hMutex);
#endif
	}

	void UnLock()
	{
#ifdef WIN32
		LeaveCriticalSection(&m_oSection);
#else
		pthread_mutex_unlock(&m_hMutex);
#endif
	};

private:
#ifdef _WIN32
	CRITICAL_SECTION m_oSection;
#else
	pthread_mutex_t m_hMutex;
#endif
};

//=============================================================================
// class auto_lock
class auto_lock
{
public:
	auto_lock(mutex& lock)
		:m_lock(lock)
	{
		m_lock.Lock();
	}

	~auto_lock()
	{
		m_lock.UnLock();
	}

private:
	mutex& m_lock;
};

#endif // __MATRIX_MUTEX_H__
