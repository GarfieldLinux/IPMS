/************************************************************************************/
//模块名称：读写锁 、互斥锁
//功能说明: 1)构造函数加锁
//	    2) 析构函数解锁
// 作者：徐永丰 tianya0513@hotmail.com
//日期： 2010-09-21
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/

#ifndef __RWLOCK_H
#define __RWLOCK_H


#include <pthread.h>

//读写锁
//锁类型
enum RWLOCK_TYPE
{
	RWLOCK_READ = 0,
	RWLOCK_WRITE
};
class CRwLock
{
//公有函数
public:
        //构造函数
        CRwLock(pthread_rwlock_t* lock,RWLOCK_TYPE type = RWLOCK_WRITE);
	//析构函数
	virtual~CRwLock();
private:
	//读写锁
	pthread_rwlock_t* m_rwLock;
};


//互斥锁 
class CMutexLock
{
//公有函数
public:
	//构造函数
	CMutexLock(pthread_mutex_t* lock);
	//析构函数
	virtual~CMutexLock();
private:
	//互斥锁
	pthread_mutex_t* m_mutexLock;
};


#endif //__RWLOCK_H
