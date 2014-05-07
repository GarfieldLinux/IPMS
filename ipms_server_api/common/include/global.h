/************************************************************************************/
//ģ�����ƣ�����ͬ������� ȫ��ͷ�ļ� 
//����˵����1)ȫ��ͷ�ļ� 
//���ߣ������� tianya0513@hotmail.com
//���ڣ�2008-9-1
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#ifndef _WIN32
	#include <pthread.h>
	#include <sys/epoll.h>
	#include <sys/resource.h>
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <sys/wait.h>
	#include <sys/time.h>
	#include <pwd.h>
	#include <syslog.h>
	#include <netdb.h>
	#include <fstream>
	#include <iostream>
#endif
#include <errno.h>
#include <fcntl.h>
#include <map>
#include <list>
#include <string>
#include <set>
#include "xmlrpc.h"
#include "md5.h"

using namespace XmlRpc;


#ifdef _WIN32
	#define __int64_t __int64
	#define	__int32_t unsigned int
#endif

#include "constdef.h"		//��������
#include "structdef.h"		//�ṹ�嶨��

//string �б�
#ifndef StringList
	typedef std::list<std::string>	StringList;
#endif

//INT �б�
#ifndef IntList
	typedef std::list<int>	IntList;
#endif


//stringӳ��
#ifndef StringMap
	typedef std::map<std::string,std::string> StringMap;
#endif

#endif //__GLOBAL_H__
