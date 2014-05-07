/************************************************************************************/
//模块名称：数据同步服务端 日志类
//功能说明：1)日志记录
//			2)日志文件分文件
//作者：徐永丰 tianya0513@hotmail.com
//日期：2008-9-1
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/
#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#ifndef _WIN32 
	#include <pthread.h>
#endif
#include <list>
#include <string>
#include <stdlib.h>

#ifndef _MAX_PATH
	#define _MAX_PATH	255
#endif

//错误日志文件名
#ifndef ERROR_LOG_FILE_NAME
	#define ERROR_LOG_FILE_NAME	"ERROR_DEFAULT-"
#endif
//普通日志文件名
#ifndef NORMAL_LOG_FILE_NAME
	#define NORMAL_LOG_FILE_NAME "NORMAIL_DEFAULT-"
#endif

//日志默认长度 
#ifndef	MAX_LOG_SIZE
	#define MAX_LOG_SIZE	8192
#endif

//默认的日志路径
#ifndef DEFAULT_LOG_PATH
	#define DEFAULT_LOG_PATH	"/var/log/"EXENAME
#endif

//日志信息列表
#ifndef MsgList
	typedef std::list<std::string> MsgList;
#endif

//获取可执行文件路径名
std::string GetModuleFileName();
//获取可执行文件路径
std::string GetExePath();

//template <class T> 
//std::string ConvertToString(T value);



class CLog
{
public:
	CLog();
	~CLog();
public:
	//线程结束标识
	bool m_bEndThread;

public:

	//启动
	bool Init();
	//退出
	void UnInit();

	//错误LOG
	bool LogError(char* chMsg);
	//正常LOG
	bool LogDefault(char* chMsg);
	//处理一次数据
	void LogOnce();
	//设置日志文件名称头
	void SetLogFile(const char* szError,const char* szNormal);

private:
	//初始化错误日志文件
	void InitError();
	//初始化普通日志文件
	void InitNormal();
	//写数据
	bool Log(FILE* fp,const char* chMsg);
	//切换文件
	bool SwitchFile(bool bError = true);
	//清理日志
	void DelLog();
private:
	//日志文件-ERROR
	FILE* m_fpError;       ///< file handle of log file
	//日志文件-普通
	FILE* m_fpNormal;
	//输出日志
	char m_chMsg[1024];
	//记录时间
	time_t m_time; 
	//时间结构
	struct tm *m_tNow;
	//错误日志文件名
	char m_chErrorName[_MAX_PATH];
	//普通日志文件名
	char m_chNormalName[_MAX_PATH];
	//临时使用文件名
	char m_chTempName[_MAX_PATH];

	//普通日志数据
	MsgList	m_listMsg_Normal;
	//错误日志数据
	MsgList m_listMsg_Error;
	//互斥
	pthread_mutex_t m_listMsg_mutex;

	//线程ID
	pthread_t m_nThreadID;

	//错误日志的文件名
	char m_chError[_MAX_PATH];
	//正常日志的文件名
	char m_chNormal[_MAX_PATH];

	//日志文件路径
	char m_chPath[_MAX_PATH];

	//日志文件切换标识
	bool m_bSwitchFile;
};


//日志处理
extern CLog	g_logMonitor;

//多参输出错误日志
void LogErrorPara(const char* chMsg,...);
//多参输出默认日志
void LogDefaultPara(const char* chMsg,...);
//错误日志
void LogError(char* chMsg);
//默认日志
void LogDefault(char* chMsg);



#endif //__LOG_H__
