#include <pthread.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "epoll.h"


//���캯��
CEpollBase::CEpollBase()
{
	//�߳̽�����ʶ
	m_bEndThread = false;
	//�¼������߳�ID
	m_nThreadID = 0;
	//���SOCKET
	m_nSockListen = 0;
	//EPOLL FD
	m_nSizeFd = 0;
	//��ʼ������
	pthread_mutex_init(&m_fdEpoll_mutex, NULL);
	return;
}
//��������
CEpollBase::~CEpollBase()
{
	UnInit();
	//���ٻ���
	pthread_mutex_destroy(&m_fdEpoll_mutex);
	return;
}

//��ʼ������
bool CEpollBase::Init(const unsigned short nPort)
{
	m_bEndThread = false;
	//��������˿�
	if(!BeginServer(nPort)) return false;
	//���������߳�
	if(!BeginDealThread()) return false;
	return true;
}

//�ͷ�
void CEpollBase::UnInit()
{
	//�߳���ֹ
	m_bEndThread = true;
	//�ر�SERVER�˿�
	EndServer();
	//���������߳�
	EndDealThread();
	return;
}

//ɾ��һ������FD
bool CEpollBase::DelFd(const int nFd)
{
	//����
	pthread_mutex_lock(&m_fdEpoll_mutex);
	m_evEpoll.data.fd = nFd;
	if(epoll_ctl(m_nSizeFd, EPOLL_CTL_DEL, nFd,&m_evEpoll) < 0)
	{
		//����
		pthread_mutex_unlock(&m_fdEpoll_mutex);
		return false;
	}
	m_nSizeCurFd --;
	//����
	pthread_mutex_unlock(&m_fdEpoll_mutex);
	return true;
}

//epoll�����߳�
void* ThreadWait(void *arg)
{
	CEpollBase* pEpoll = (CEpollBase*)arg;
	if(pEpoll == 0) return arg;
	//ѭ����������
	while(!pEpoll->m_bEndThread)
	{
		//ѭ������һ���¼�
		pEpoll->WaitOnce();
		//ÿ�봦��1000���¼�
		usleep(1000);
	}
	return arg;
}

//���������߳�
bool CEpollBase::BeginDealThread()
{
	//����epoll�ƹ������߳�
    if(pthread_create(&m_nThreadID, NULL, ThreadWait, this) != 0)	return false;
	return true;
}
//���������߳�
void CEpollBase::EndDealThread()
{
	if(m_nThreadID != 0)
		pthread_join(m_nThreadID,NULL);
	m_nThreadID = 0;
	return;
}

//��������˿�
bool CEpollBase::BeginServer(const unsigned short nPort)
{
	//��ʼ��socket
	struct	rlimit rt;
	rt.rlim_max = rt.rlim_cur = MAX_EPOLL_SIZE;
	if(setrlimit(RLIMIT_NOFILE,&rt) == -1)	return false;
	//����socket
	if((m_nSockListen = socket(PF_INET,SOCK_STREAM,0)) == -1)	return false;
	if(SetNonblocking(m_nSockListen) == -1) return false;
	//��Ϊreuse
	int sFlag = 1;
	setsockopt(m_nSockListen,SOL_SOCKET,SO_REUSEADDR,(const char*)&sFlag,sizeof(sFlag));
	//�����������
	struct	sockaddr_in addr_my;
	bzero(&addr_my,sizeof(addr_my));
	addr_my.sin_family = PF_INET;
	addr_my.sin_port = htons(nPort);
    addr_my.sin_addr.s_addr = INADDR_ANY;
	//�󶨶˿�
	if(bind(m_nSockListen,(struct sockaddr*)&addr_my,sizeof(struct sockaddr)) == -1)	return false;
	//�����˿�
	if(listen(m_nSockListen,32) == -1) return false;
	//��ʼ��epoll
	m_nSizeFd = epoll_create(MAX_EPOLL_SIZE);
	bzero(&m_evEpoll,sizeof(m_evEpoll));
	m_evEpoll.events = EPOLLIN;
	m_evEpoll.data.fd = m_nSockListen;
	if(epoll_ctl(m_nSizeFd,EPOLL_CTL_ADD,m_nSockListen,&m_evEpoll) < 0)	return false;
	m_nSizeCurFd = 1;
	return true;
}
//�رշ���˿�
void CEpollBase::EndServer()
{
	//�رռ�ض˿�
	if(m_nSockListen)
	{
		shutdown(m_nSockListen,2);
		close(m_nSockListen);		//�ر�����
	}
	m_nSockListen = 0;
	//�ر�epoll���
	if(m_nSizeFd)
	{
		shutdown(m_nSizeFd,2);
		close(m_nSizeFd);
	}
	m_nSizeFd = 0;
	return;
}

//����һ���¼� 
void CEpollBase::WaitOnce()
{
	//����һ���¼� 
	struct	epoll_event m_events[MAX_EPOLL_SIZE];
	int nFd = epoll_wait(m_nSizeFd,m_events,m_nSizeCurFd,1);
	if(nFd == -1)	return;
	for(int i = 0;i < nFd; i++)
	{
		//�ж��¼�����
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
			
			//2010-11-30:�����շ�����
			//int sFlag = 1024*80;
			//setsockopt(nFd,SOL_SOCKET,SO_RCVBUF,(const char*)&sFlag,sizeof(int));
			//setsockopt(nFd,SOL_SOCKET,SO_SNDBUF,(const char*)&sFlag,sizeof(int));

			//���յ�����
			if(!OnAccept(nFd)) 
			{
				shutdown(nFd,2);
				close(nFd);
				continue;
			}
			//����
			pthread_mutex_lock(&m_fdEpoll_mutex);
			//��ӵ�EPOLL�б�
			m_evEpoll.events = EPOLLIN|EPOLLET;		//��Ե����
			m_evEpoll.data.fd = nFd;
			if(epoll_ctl(m_nSizeFd,EPOLL_CTL_ADD,nFd,&m_evEpoll) < 0)
			{
				shutdown(nFd,2);
				close(nFd);
				//����
				pthread_mutex_unlock(&m_fdEpoll_mutex);
				continue;
			}
			m_nSizeCurFd ++;
			//����
			pthread_mutex_unlock(&m_fdEpoll_mutex);
		}
		else
		{
			//����Ϣ�ɶ�
			if(m_events[i].events & EPOLLIN)
				OnRecv(m_events[i].data.fd);
		}
	}
	return;
}

//���÷�����ģʽ
bool CEpollBase::SetNonblocking(int nFd)
{
	//���÷�����ģʽ
	if(fcntl(nFd, F_SETFL, fcntl(nFd, F_GETFD, 0)|O_NONBLOCK) == -1) 
		return false;
	return true;
}
