#include "common.h"
#include "mailmanager.h"
#include "mail.h"



//多参输出错误日志
void LogErrorAndMailTo(const ERROR_LEVEL level,const char* chMsg,...)
{
	do
	{
		char chTemp[DEFAULT_BUFFER_SIZE] = {'\0'};
		va_list varg;
		va_start(varg,chMsg);
        vsprintf(chTemp,chMsg,varg);
        va_end(varg);
		//保存到日志
		LogError(chTemp);
		//保存到邮件发送列表
		g_mailManager.SendMail(g_conf_t.strMailTitle,DEFAULT_MAIL_DEAL,chTemp,level);
	}
	while (0);
}


//全局变量
CMailManager g_mailManager;
//构造函数
CMailManager::CMailManager()
{
	m_nThreadID = 0;
	//
	m_bEndThread = false;
	//MAIL列表
	m_strMailTo = "garfieldlinux@gmail.com";
	//初始化互斥
	pthread_mutex_init(&m_listBody_mutex, NULL);
	return;
}
//析构函数
CMailManager::~CMailManager()
{
	//释放互斥
	pthread_mutex_destroy(&m_listBody_mutex);
	if(m_nThreadID == 0) return;
	UnInit();
	return;
}


#ifndef QUERY_DELAY
	#define QUERY_DELAY		(5 * 60)
#endif

//日志处理线程
void* ThreadMail(void *arg)
{
	CMailManager* pManager = (CMailManager*)arg;
	if(pManager == 0) return arg;
	//循环处理数据
	while(!pManager->m_bEndThread)
	{ 
		//发送一次邮件（5分钟处理一次）
		static time_t timep = time(0);
		if( (time(0) - timep) > QUERY_DELAY)
		{
			timep = time(0);
			pManager->SendMail();
	 	}
		sleep(1);
	}
	return arg;
}

//初始化
bool CMailManager::Init()
{	
	m_bEndThread = false;
	//启动UDP消息接收线程
    if(pthread_create(&m_nThreadID, NULL, ThreadMail, this) != 0)
	{
		LogError("启动邮件发送线程失败,监控中心启动失败!");
		return false;
	} 
//#ifdef _DEBUG
//	LogDefault("启动邮件发送线程成功,开始处理需要发送的邮件!");
//#endif
	return true;
}

//释放
void CMailManager::UnInit()
{
	//线程结束标识
	m_bEndThread = true;
	//等待0.1毫秒
	usleep(100);
	//停止UDP消息接收线程
	if(m_nThreadID != 0)
		pthread_join(m_nThreadID,NULL);
	m_nThreadID = 0;
//#ifdef _DEBUG
//	LogDefault("停止邮件发送线程成功!");
//#endif
	return;
}

//发送一次邮件
void CMailManager::SendMail()
{
	if(m_listBody.size() == 0) return;
	//发送一次邮件
#ifdef _DEBUG
	LogDefault("开始发送一次邮件......");
#endif
	//汇总邮件数据
	std::string strBody = MakeMailBody();
	//printf("XXXX %s XXXX\n",strBody.c_str());
	mailex_t mail;
	mail.mail_svrip = "imap.gmail.com";
	mail.mail_svrport = 993;
	mail.user = "garfieldlinux@gmail.com"; mail.passwd="comefromconfigfile";
	mail.mail_from="garfieldlinux@gmail.com";

	char seps[]   = " ,\t\n\r";
	char szMailTo[DEFAULT_BUFFER_SIZE] = {'\0'};
	if(m_strMailTo.size() >= DEFAULT_BUFFER_SIZE)
		LogError("发送的邮件列表超长...........................................");
	sprintf(szMailTo,"%s",m_strMailTo.c_str());
	char *token = NULL;
	token = strtok( szMailTo, seps );
	while( token != NULL )
	{
		mail.rcpt_to.push_back(token);
		token = strtok( NULL, seps );
	} 
	mail.subject = m_strHeader;
	mail.body = strBody;
	if(smtp_sendmail(&mail) < 0) 
		LogError("发送通知邮件失败!");
	return;
}

//报警级别
inline static const char* _ERROR_LEVEL(const ERROR_LEVEL level)
{
	switch(level)
	{
		case EL_1:
			return "一级报警.";
		case EL_2:
			return "二级报警.";
		case EL_3:
		case EL_4:
		default:
			return "普通报警.";
	}
}


//邮件内容
void CMailManager::SendMail(const std::string strHeader,const std::string strNote,const std::string strBody,const ERROR_LEVEL level)
{
	//加锁
	pthread_mutex_lock(&m_listBody_mutex);
	//保存邮件标题
	m_strHeader = strHeader;
	//保存邮件前部分信息
	m_strNote = strNote;
	//邮件内容
	std::string strMailBody = _ERROR_LEVEL(level) + strBody;
	//添加到列表
	m_listBody.insert(MailMap::value_type(level,strMailBody));
	//解锁
	pthread_mutex_unlock(&m_listBody_mutex);
	return;
}

//生成发送邮件内容
std::string CMailManager::MakeMailBody()
{
	struct sockaddr_in sock_in;
	std::string strMsg;

	strMsg = "<html><head><style>blockquote{border-width: 1px;border-color: Aqua;border-style: dashed;background: #FFEFDF;padding: 1px;}</style><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>";
	strMsg += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head><body><div style=\"WIDTH:1024px;font-size:12px;\">";
	strMsg += "<p align=center style=\"font-size:15px;\"><b>帝联分发系统报警邮件</b></p>";
	//邮件前部分描述
	strMsg += "<blockquote><font size=2 color=#9966FF><b>";
	strMsg += m_strNote;
	strMsg += "</b></font></blockquote>";

	//加锁
	pthread_mutex_lock(&m_listBody_mutex);
	//所有状态
	MailMap::iterator it = m_listBody.begin();
	while( it != m_listBody.end())
	{
		//邮件内容
		switch(it->first)
		{
			case EL_1:
				strMsg += "<blockquote><font size=2 color=#FF0000><b>";
				break;
			case EL_2:
				strMsg += "<blockquote><font size=2 color=#FF8F00><b>";
				break;
			case EL_3:
			case EL_4:
			default:
				strMsg += "<blockquote><font size=2 color=#9966FF><b>";
				break;
		}
		strMsg += it->second;
		strMsg += "</b></font></blockquote>";
		it ++;
	}
	m_listBody.clear();
	//解锁
	pthread_mutex_unlock(&m_listBody_mutex);
	strMsg += "</div></body></html>";
	return strMsg;
}
//设置发送的邮件地址
void CMailManager::SetMailTo(const std::string strMailTo)
{
	//发送的邮件地址列表
	m_strMailTo = strMailTo;
	return;
}
