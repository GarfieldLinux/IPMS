#include <pthread.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "epoll.h"


//构造函数
CEpollBase::CEpollBase()
{
	//线程结束标识
	m_bEndThread = false;
	//事件处理线程ID
	m_nThreadID = 0;
	//监控SOCKET
	m_nSockListen = 0;
	//EPOLL FD
	m_nSizeFd = 0;
	//初始化互斥
	pthread_mutex_init(&m_fdEpoll_mutex, NULL);
	return;
}
//析构函数
CEpollBase::~CEpollBase()
{
	UnInit();
	//销毁互斥
	pthread_mutex_destroy(&m_fdEpoll_mutex);
	return;
}

//初始化函数
bool CEpollBase::Init(const unsigned short nPort)
{
	m_bEndThread = false;
	//开启服务端口
	if(!BeginServer(nPort)) return false;
	//启动处理线程
	if(!BeginDealThread()) return false;
	return true;
}

//释放
void CEpollBase::UnInit()
{
	//线程终止
	m_bEndThread = true;
	//关闭SERVER端口
	EndServer();
	//结束处理线程
	EndDealThread();
	return;
}

//删除一个监听FD
bool CEpollBase::DelFd(const int nFd)
{
	//加锁
	pthread_mutex_lock(&m_fdEpoll_mutex);
	m_evEpoll.data.fd = nFd;
	if(epoll_ctl(m_nSizeFd, EPOLL_CTL_DEL, nFd,&m_evEpoll) < 0)
	{
		//解锁
		pthread_mutex_unlock(&m_fdEpoll_mutex);
		return false;
	}
	m_nSizeCurFd --;
	//解锁
	pthread_mutex_unlock(&m_fdEpoll_mutex);
	return true;
}

//epoll处理线程
void* ThreadWait(void *arg)
{
	CEpollBase* pEpoll = (CEpollBase*)arg;
	if(pEpoll == 0) return arg;
	//循环处理数据
	while(!pEpoll->m_bEndThread)
	{
		//循环处理一次事件
		pEpoll->WaitOnce();
		//每秒处理1000条事件
		usleep(1000);
	}
	return arg;
}

//开启处理线程
bool CEpollBase::BeginDealThread()
{
	//创建epoll逼供处理线程
    if(pthread_create(&m_nThreadID, NULL, ThreadWait, this) != 0)	return false;
	return true;
}
//结束处理线程
void CEpollBase::EndDealThread()
{
	if(m_nThreadID != 0)
		pthread_join(m_nThreadID,NULL);
	m_nThreadID = 0;
	return;
}

//开启服务端口
bool CEpollBase::BeginServer(const unsigned short nPort)
{
	//初始化socket
	struct	rlimit rt;
	rt.rlim_max = rt.rlim_cur = MAX_EPOLL_SIZE;
	if(setrlimit(RLIMIT_NOFILE,&rt) == -1)	return false;
	//创建socket
	if((m_nSockListen = socket(PF_INET,SOCK_STREAM,0)) == -1)	return false;
	if(SetNonblocking(m_nSockListen) == -1) return false;
	//设为reuse
	int sFlag = 1;
	setsockopt(m_nSockListen,SOL_SOCKET,SO_REUSEADDR,(const char*)&sFlag,sizeof(sFlag));
	//本机服务参数
	struct	sockaddr_in addr_my;
	bzero(&addr_my,sizeof(addr_my));
	addr_my.sin_family = PF_INET;
	addr_my.sin_port = htons(nPort);
    addr_my.sin_addr.s_addr = INADDR_ANY;
	//绑定端口
	if(bind(m_nSockListen,(struct sockaddr*)&addr_my,sizeof(struct sockaddr)) == -1)	return false;
	//监听端口
	if(listen(m_nSockListen,32) == -1) return false;
	//初始化epoll
	m_nSizeFd = epoll_create(MAX_EPOLL_SIZE);
	bzero(&m_evEpoll,sizeof(m_evEpoll));
	m_evEpoll.events = EPOLLIN;
	m_evEpoll.data.fd = m_nSockListen;
	if(epoll_ctl(m_nSizeFd,EPOLL_CTL_ADD,m_nSockListen,&m_evEpoll) < 0)	return false;
	m_nSizeCurFd = 1;
	return true;
}
//关闭服务端口
void CEpollBase::EndServer()
{
	//关闭监控端口
	if(m_nSockListen)
	{
		shutdown(m_nSockListen,2);
		close(m_nSockListen);		//关闭连接
	}
	m_nSockListen = 0;
	//关闭epoll句柄
	if(m_nSizeFd)
	{
		shutdown(m_nSizeFd,2);
		close(m_nSizeFd);
	}
	m_nSizeFd = 0;
	return;
}

//处理一次事件 
void CEpollBase::WaitOnce()
{
	//处理一次事件 
	struct	epoll_event m_events[MAX_EPOLL_SIZE];
	int nFd = epoll_wait(m_nSizeFd,m_events,m_nSizeCurFd,1);
	if(nFd == -1)	return;
	for(int i = 0;i < nFd; i++)
	{
		//判断事件类型
		if(m_events[i].data.fd == m_nSockListen)
		{
			struct	sockaddr_in addr_new;
			bzero(&addr_new,sizeof(addr_new));
			socklen_t sockLen = sizeof(socklen_t);
			int nFd = accept(m_nSockListen,(struct	sockaddr*)&addr_new,&sockLen);
			if(nFd < 0)	continue;
			if(SetNonblocking(nFd) == -1)
			{
				shutdown(nFd,2);
				close(nFd);
				continue;
			}
			
			//2010-11-30:设置收发缓存
			//int sFlag = 1024*80;
			//setsockopt(nFd,SOL_SOCKET,SO_RCVBUF,(const char*)&sFlag,sizeof(int));
			//setsockopt(nFd,SOL_SOCKET,SO_SNDBUF,(const char*)&sFlag,sizeof(int));

			//接收到连接
			if(!OnAccept(nFd)) 
			{
				shutdown(nFd,2);
				close(nFd);
				continue;
			}
			//加锁
			pthread_mutex_lock(&m_fdEpoll_mutex);
			//添加到EPOLL列表
			m_evEpoll.events = EPOLLIN|EPOLLET;		//边缘触发
			m_evEpoll.data.fd = nFd;
			if(epoll_ctl(m_nSizeFd,EPOLL_CTL_ADD,nFd,&m_evEpoll) < 0)
			{
				shutdown(nFd,2);
				close(nFd);
				//解锁
				pthread_mutex_unlock(&m_fdEpoll_mutex);
				continue;
			}
			m_nSizeCurFd ++;
			//解锁
			pthread_mutex_unlock(&m_fdEpoll_mutex);
		}
		else
		{
			//有消息可读
			if(m_events[i].events & EPOLLIN)
				OnRecv(m_events[i].data.fd);
		}
	}
	return;
}

//设置非阻塞模式
bool CEpollBase::SetNonblocking(int nFd)
{
	//设置非阻塞模式
	if(fcntl(nFd, F_SETFL, fcntl(nFd, F_GETFD, 0)|O_NONBLOCK) == -1) 
		return false;
	return true;
}
