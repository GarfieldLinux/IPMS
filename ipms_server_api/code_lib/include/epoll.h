/************************************************************************************/
//ģ�����ƣ�EPOLL ����
//����˵��:	1)��������SOCKET
//			2)EPOLL����
//			3)�ṩ����¼�
//				OnAccept()
//				OnRecv()
//���ߣ������� tianya0513@hotmail.com
//���ڣ�2010-09-21
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/

#ifndef __EPOLL_H
#define __EPOLL_H

#include <sys/epoll.h>
#include <pthread.h>

//ĿǰĬ�ϴ���2000������
#ifndef MAX_EPOLL_SIZE
#define	MAX_EPOLL_SIZE 2000
#endif

class CEpollBase
{
//���к���
public:
	//���캯��
	CEpollBase();
	//��������
	virtual~CEpollBase();
public:
	//��ʼ��
	bool Init(const unsigned short nPort);
	//�ͷ�
	void UnInit();
	//ɾ��һ������FD
	bool DelFd(const int nFd);
	//����һ���¼�
	void WaitOnce();
//���ڼ̳�
protected:
	//���������¼�
	virtual bool OnAccept(const int nFd) = 0;
	//������Ϣ�¼�
	virtual	void OnRecv(const int nFd) = 0;
private:
	//���������߳�
	bool BeginDealThread();
	//���������߳�
	void EndDealThread();
	//��������˿�
	bool BeginServer(const unsigned short nPort);
	//�رշ���˿�
	void EndServer();
	//��Ϊ������
	bool SetNonblocking(int nFd);
public:
	//�߳�ֹͣ��ʶ
	bool m_bEndThread;
private:	
	//������SERVER�˿�
	int m_nSockListen;
	//��ǰFD��Ŀ
	int	m_nSizeCurFd;
	//EPOLL���
	int	m_nSizeFd;
	//EPOLL��д��
	pthread_mutex_t m_fdEpoll_mutex;

	//EPOLL EVENT
	struct	epoll_event m_evEpoll;
	//�¼������߳�ID
	pthread_t m_nThreadID;
};

#endif //__EPOLL_H
