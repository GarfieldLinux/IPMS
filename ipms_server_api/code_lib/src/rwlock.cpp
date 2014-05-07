#include <stdio.h>
#include <assert.h>
#include "rwlock.h"


//构造函数
CRwLock::CRwLock(pthread_rwlock_t* lock,RWLOCK_TYPE type/* = RWLOCK_WRITE */)
{
	assert(lock);
	m_rwLock = lock;
	switch(type)
	{
		case RWLOCK_READ:
			pthread_rwlock_rdlock(m_rwLock);
			break;
		case RWLOCK_WRITE:
			pthread_rwlock_wrlock(m_rwLock);
			break;
	}
	return;
}

//析构函数
CRwLock::~CRwLock()
{
	pthread_rwlock_unlock(m_rwLock);
	return;
}


//构造函数
CMutexLock::CMutexLock(pthread_mutex_t* lock)
{
	assert(lock);
	m_mutexLock = lock;
	pthread_mutex_lock(m_mutexLock);
	return;
}
//析构函数
CMutexLock::~CMutexLock()
{
	pthread_mutex_unlock(m_mutexLock);
	return;
}
