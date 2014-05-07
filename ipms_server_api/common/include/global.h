/************************************************************************************/
//模块名称：数据同步服务端 全局头文件 
//功能说明：1)全局头文件 
//作者：徐永丰 tianya0513@hotmail.com
//日期：2008-9-1
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

#include "constdef.h"		//常量定义
#include "structdef.h"		//结构体定义

//string 列表
#ifndef StringList
	typedef std::list<std::string>	StringList;
#endif

//INT 列表
#ifndef IntList
	typedef std::list<int>	IntList;
#endif


//string映射
#ifndef StringMap
	typedef std::map<std::string,std::string> StringMap;
#endif

#endif //__GLOBAL_H__
