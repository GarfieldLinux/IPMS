/************************************************************************************/
//模块名称：EPOLL 基类
//功能说明:	1)创建监听SOCKET
//			2)EPOLL管理
//			3)提供相关事件
//				OnAccept()
//				OnRecv()
//作者：徐永丰 tianya0513@hotmail.com
//日期：2010-09-21
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/

#ifndef __EPOLL_H
#define __EPOLL_H

#include <sys/epoll.h>
#include <pthread.h>

//目前默认处理2000个连接
#ifndef MAX_EPOLL_SIZE
#define	MAX_EPOLL_SIZE 2000
#endif

class CEpollBase
{
//公有函数
public:
	//构造函数
	CEpollBase();
	//析构函数
	virtual~CEpollBase();
public:
	//初始化
	bool Init(const unsigned short nPort);
	//释放
	void UnInit();
	//删除一个监听FD
	bool DelFd(const int nFd);
	//处理一次事件
	void WaitOnce();
//用于继承
protected:
	//接收连接事件
	virtual bool OnAccept(const int nFd) = 0;
	//接收消息事件
	virtual	void OnRecv(const int nFd) = 0;
private:
	//开启处理线程
	bool BeginDealThread();
	//结束处理线程
	void EndDealThread();
	//开启服务端口
	bool BeginServer(const unsigned short nPort);
	//关闭服务端口
	void EndServer();
	//设为非阻塞
	bool SetNonblocking(int nFd);
public:
	//线程停止标识
	bool m_bEndThread;
private:	
	//监听的SERVER端口
	int m_nSockListen;
	//当前FD数目
	int	m_nSizeCurFd;
	//EPOLL句柄
	int	m_nSizeFd;
	//EPOLL读写锁
	pthread_mutex_t m_fdEpoll_mutex;

	//EPOLL EVENT
	struct	epoll_event m_evEpoll;
	//事件处理线程ID
	pthread_t m_nThreadID;
};

#endif //__EPOLL_H
