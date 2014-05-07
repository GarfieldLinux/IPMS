/************************************************************************************/
//模块名称：监控中心邮件发送管理器
//功能说明：1)启动线程
//			2)停止线程
//			3)邮件数据
//			4)发送邮件
//作者：徐永丰 tianya0513@hotmail.com
//日期：2009-11-26
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/
#ifndef __MAILMANAGER_H__
#define __MAILMANAGER_H__

#include "global.h"

enum ERROR_LEVEL
{
	EL_1 = 0,		//一级故障
	EL_2,			//二级故障
	EL_3,			//三级故障
	EL_4			//四级故障
};

#ifndef MailMap
	typedef std::multimap<ERROR_LEVEL,std::string> MailMap;
#endif


//发送邮件
void LogErrorAndMailTo(const ERROR_LEVEL level,const char* pszMsg,...);

//深度分析处理管理中心
class CMailManager
{
public:
	CMailManager();
	virtual ~CMailManager();
public:
	//初始化
	bool Init();
	//释放
	void UnInit();
	//邮件内容
	void SendMail(const std::string strHeader,const std::string strNote,const std::string strBody,const ERROR_LEVEL level);
	//发送一次邮件
	void SendMail();

	//设置发送的邮件地址
	void SetMailTo(const std::string strMailTo);

	//判断是否有邮件未发送
	bool IsMailListEmpty() { return m_listBody.size() == 0;};

private:
	//生成发送邮件内容
	std::string MakeMailBody();
public:
	//线程结束标识
	bool m_bEndThread;
private:
	//线程ID
	pthread_t m_nThreadID;
	//状态列表
	MailMap m_listBody;
	//互斥
	pthread_mutex_t m_listBody_mutex;
	//邮件标题
	std::string m_strHeader;
	//邮件前部分内容
	std::string m_strNote;
	//发送的邮件地址
	std::string m_strMailTo;
};
//全局变量
extern CMailManager g_mailManager;


#endif //__FLUXMANAGER_H__
