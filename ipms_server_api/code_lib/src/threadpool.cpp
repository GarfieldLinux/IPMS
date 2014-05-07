#include "threadpool.h"



//全局变量
CThreadPool g_threadPool;
//构造函数
CThreadPool::CThreadPool()
{
	//线程数
	m_nThreadNumber = 0;
	//线程句柄
	m_nThreadID = 0;
	return;
}

//析构函数
CThreadPool::~CThreadPool()
{
	//结束线程池
	EndPool();
	return;
}

//启动
bool CThreadPool::BeginPool(int nThreadNumber,THREAD_FUNC func,void* args/* = NULL */)
{
	//有效线程数
	if(nThreadNumber <= 0) return false;
	if(m_nThreadID)
		delete[] m_nThreadID;
	m_nThreadID = 0;
	//保存线程数
	m_nThreadNumber = nThreadNumber;
	m_nThreadID = new pthread_t[m_nThreadNumber];
	if(m_nThreadID == 0) return false;
	//启动线程
	for(int i = 0; i < m_nThreadNumber; i ++)
	{
		//判断启动是否成功
		if(!BeginThread(i,func,args)) return false;
	}
	//创建成功
	return true;
}
//停止
void CThreadPool::EndPool()
{
	if(m_nThreadID == 0) return;
	//停止线程
	for(int i = 0; i < m_nThreadNumber; i ++)
		EndThread(i);
	if(m_nThreadID)
		delete[] m_nThreadID;
	m_nThreadID = 0;
	return;
}

//开启线程
bool CThreadPool::BeginThread(const int nIndex,THREAD_FUNC func,void* args)
{
	//创建线程
    if(pthread_create(&m_nThreadID[nIndex],NULL, func, args) != 0)
	{
		return false;
	}
	return true;
}


//中止线程
void CThreadPool::EndThread(const int nIndex)
{
	//等待线程结束
	usleep(10000);
	//等待线程结束
	if(m_nThreadID[nIndex] != 0)
		pthread_join(m_nThreadID[nIndex],NULL);
	m_nThreadID[nIndex] = 0;
	return;
}
