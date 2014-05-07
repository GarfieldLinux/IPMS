/************************************************************************************/
//模块名称：线程池管理
//功能说明：1)启动
//			2)停止
//说明：启动线程池
//作者：徐永丰 tianya0513@hotmail.com
//日期：2009-4-2
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#ifndef _WIN32 
	#include <pthread.h>
	#include <unistd.h>
#endif

#ifndef THREAD_FUNC
typedef void*(*THREAD_FUNC)(void*);
#endif

class CThreadPool
{
//公有函数
public:
	//构造函数
	CThreadPool();
	//析构函数
	virtual~CThreadPool();
public:
	//启动线程池
	//参数:nThreadNumber	线程数
	//	   func				线程函数
	//	   args				线程参数
	bool BeginPool(int nThreadNumber,THREAD_FUNC func,void* args = NULL);
	//停止线程池
	void EndPool();

private:
	//开启线程
	bool BeginThread(const int nIndex,THREAD_FUNC func,void* args);
	//结束线程
	void EndThread(const int nIndex);
private:
	//线程数
	int m_nThreadNumber;
	//线程句柄指针
	pthread_t *m_nThreadID;
};

//全局变量
extern CThreadPool g_threadPool;


#endif //__THREADPOOL_H__
