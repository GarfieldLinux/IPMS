#include "log.h"

//日志
CLog	g_logMonitor;


std::string GetModuleFileName()
{
#ifndef WIN32
	char szName[PATH_MAX+NAME_MAX] = {'\0'};
	char szExe[NAME_MAX] = {'\0'};
	sprintf(szExe,"/proc/%d/exe",getpid());
	if (readlink( szExe, szName, sizeof(szName) ) == -1)
		return std::string("");
	else
		return std::string(szName);
#else 
	char szName[MAX_PATH*2];
	if ( !::GetModuleFileName(GetModuleHandle(NULL),szName,sizeof(szName)) )
		return std::string("");
	else
		return std::string(szName);
#endif
}

//获取可执文件文件路径
std::string GetExePath()
{
	std::string sExe = GetModuleFileName();
#ifdef WIN32
	std::string::size_type nLastBs = sExe.find_last_of("\\");
#else
	std::string::size_type nLastBs = sExe.find_last_of("/");
#endif
	if ( nLastBs != std::string::npos )
		return sExe.substr(0, nLastBs + 1);
	else
		return std::string("");
}

CLog::CLog()
{
	m_nThreadID = 0;
	//错误日志的文件名
	memset(m_chError,0,sizeof(m_chError));
	sprintf(m_chError,"%s",ERROR_LOG_FILE_NAME);
	//正常日志的文件名
	memset(m_chNormal,0,sizeof(m_chNormal));
	sprintf(m_chNormal,"%s",NORMAL_LOG_FILE_NAME);
	//
	memset(m_chPath,0,sizeof(m_chPath));
	sprintf(m_chPath,"%slog",GetExePath().c_str());
	//切换日志名称标志
	m_bSwitchFile = false;
	return;
}

//设置日志文件名前缀
void CLog::SetLogFile(const char* szError,const char* szNormal)
{
	//错误日志的文件名
	memset(m_chError,0,sizeof(m_chError));
	sprintf(m_chError,"%s",szError);
	//正常日志的文件名
	memset(m_chNormal,0,sizeof(m_chNormal));
	sprintf(m_chNormal,"%s",szNormal);
	return;
}

//初始化错误日志文件
void CLog::InitError()
{
	//取系统时间
	time( &m_time );
	//转成本地时间
	m_tNow = localtime(&m_time);
	//取错误文件名
	memset(m_chErrorName,0,sizeof(m_chErrorName));
	sprintf(m_chErrorName,"%s/%s%04d-%02d-%02d.log",m_chPath,m_chError,m_tNow->tm_year+1900,m_tNow->tm_mon + 1,m_tNow->tm_mday);

#ifdef _DEBUG
	printf("错误日志文件名: %s \n",m_chErrorName);
#endif
	//打开日志文件
	m_fpError = fopen(m_chErrorName,"a+b");
	//创建失败
	if(m_fpError == 0)
	{
		//错误日志失败
		return;
	}
	//不使用缓冲区
	setbuf(m_fpError, 0 );
	return;
}


//初始化普通日志文件
void CLog::InitNormal()
{
	//取系统时间
	time( &m_time );
	//转成本地时间
	m_tNow = localtime(&m_time);
	//取普通文件名
	memset(m_chNormalName,0,sizeof(m_chNormalName));
	sprintf(m_chNormalName,"%s/%s%04d-%02d-%02d.log",m_chPath,m_chNormal,m_tNow->tm_year+1900,m_tNow->tm_mon + 1,m_tNow->tm_mday);
#ifdef _DEBUG
	printf("普通日志文件名: %s \n",m_chNormalName);
#endif
	//打开日志文件
	m_fpNormal = fopen(m_chNormalName,"a+b");
	//创建失败
	if(m_fpNormal == 0)
	{
		//正常日志失败
		return;
	}
	//不使用缓冲区
	//setbuf(m_fpNormal, 0 );
	
	return;
}

//记日志
bool CLog::Log(FILE* fp,const char* chMsg)
{
//	memset(m_chMsg,0,sizeof(m_chMsg));
//	//时间[年-月-日 时:分:秒][文件名][行][内容]
//	sprintf(m_chMsg,"[%04d-%02d-%02d %02d:%02d:%02d][%s]\r\n",m_tNow->tm_year+1900,m_tNow->tm_mon + 1,m_tNow->tm_mday,\
//		m_tNow->tm_hour,m_tNow->tm_min,m_tNow->tm_sec,chMsg);

 	if(fwrite(chMsg, sizeof(char), strlen(chMsg), fp) != strlen(chMsg))
		return false;

    return true;
}

//错误LOG
bool CLog::LogError(char* chMsg)
{
	//记录到列表
	//锁
	pthread_mutex_lock(&m_listMsg_mutex);
	//保存数据
	m_listMsg_Error.push_back(chMsg);
	//解锁
	pthread_mutex_unlock(&m_listMsg_mutex);


	//切换文件
	//if(!SwitchFile() || m_fpError == 0 ) return false;
	//记日志	
	//Log(m_fpError,chMsg,chFileName,nLine);
	return true;
}
//正常LOG
bool CLog::LogDefault(char* chMsg)
{

	//记录到列表
	//锁
	pthread_mutex_lock(&m_listMsg_mutex);
	//保存数据
	m_listMsg_Normal.push_back(chMsg);
	//解锁
	pthread_mutex_unlock(&m_listMsg_mutex);

	return true;

	//切换文件
	//if(!SwitchFile(false) || m_fpNormal == 0) return false;
	//记日志
	//return Log(m_fpNormal,chMsg,chFileName,nLine);
}

//析构
CLog::~CLog()
{
	if(m_nThreadID != 0)
	{
		//如果缓存中存在日志数据,
		while(m_listMsg_Normal.size() != 0 || m_listMsg_Error.size() != 0)
			sleep(1);
		UnInit();
	}
	return;
}


//日志处理线程
void* ThreadDealLog(void *arg)
{
	CLog* pLog = (CLog*)arg;
	if(pLog == 0) return arg;
	//循环处理数据
	while(!pLog->m_bEndThread)
	{
		//循环处理一次消息
		pLog->LogOnce();
		//每秒处理1000条数据
		usleep(1000);
	}
	return arg;
}

//处理一次LOG
void CLog::LogOnce()
{
	//判断是否有数据
	//printf("normal log size = %d error log size = %d \r\n",m_listMsg_Normal.size(),m_listMsg_Error.size());
	if(m_listMsg_Normal.size() == 0 && m_listMsg_Error.size() == 0)
	{
		//此处可能会使得退出的LOG不全
		usleep(10);
		return;
	}
	//锁
	pthread_mutex_lock(&m_listMsg_mutex);
	//保存数据
	MsgList::iterator it = m_listMsg_Normal.begin();
	if( it != m_listMsg_Normal.end())
	{
		//取NORMAL日志
		if(SwitchFile(false) && m_fpNormal != 0 )
			Log(m_fpNormal,it->c_str());	
		m_listMsg_Normal.pop_front();		//弹出消息
	
	}
	//错误日志
	it = m_listMsg_Error.begin();
	if( it != m_listMsg_Error.end())
	{
		//取错误日志
		if(SwitchFile() && m_fpError != 0 )
			Log(m_fpError,it->c_str());
		m_listMsg_Error.pop_front();		//弹出消息
	}
	//解锁
	pthread_mutex_unlock(&m_listMsg_mutex);
	//清理一次历史日志
	if(m_bSwitchFile)	DelLog();
	return;
}

	//启动
bool CLog::Init()
{
	std::string strExe = "mkdir " + std::string(m_chPath);
	//创建日志目录
	if(access(m_chPath,0) == -1)
		system(strExe.c_str());
	//线程结束标识
	m_bEndThread = false;
	//线程ID初始化
	m_nThreadID = 0;
	//初始化互斥
	pthread_mutex_init(&m_listMsg_mutex, NULL);

	//分离线程
	pthread_attr_t tattr; 
    pthread_attr_init(&tattr); 
    pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_JOINABLE); 
	//创建线程
    if(pthread_create(&m_nThreadID, &tattr, ThreadDealLog, this) != 0)
	{
		//创建cient实体发送线程失败
		printf("创建日志线程失败!!!\n");
		return false;
	}
	//输出线程ID
	//printf("日志线程 = %d !",m_nThreadID);

	InitError();
	InitNormal();


	return true;
}
//退出
void CLog::UnInit()
{
	//线程结束标识
	m_bEndThread = true;
	//等待0.1毫秒
	usleep(100);
	//等待线程结束
	if(m_nThreadID != 0)
		pthread_join(m_nThreadID,NULL);
	m_nThreadID = 0;
	//销毁互斥
	pthread_mutex_destroy(&m_listMsg_mutex);

	//关闭错误日志文件
	if(m_fpError)
		fclose(m_fpError);
	m_fpError = 0;
	//关闭普通日志文件
	if(m_fpNormal)
		fclose(m_fpNormal);
	m_fpNormal = 0;



	return;
}


//切换文件
bool CLog::SwitchFile(bool bError)
{
	//未切换日志文件
	m_bSwitchFile = false;
	//处理错误日志
	if(bError)
	{
		//判断当前的文件名是否可用
		//取系统时间
		time( &m_time );
		//转成本地时间
		m_tNow = localtime(&m_time);
		//取错误文件名
		memset(m_chTempName,0,sizeof(m_chTempName));
		sprintf(m_chTempName,"%s/%s%04d-%02d-%02d.log",m_chPath,m_chError,m_tNow->tm_year+1900,m_tNow->tm_mon + 1,m_tNow->tm_mday);
		//文件名相同
		if(strcmp(m_chTempName,m_chErrorName) == 0) return true;
		//文件名不相同,切换到当前文件
		//关闭错误日志文件
		if(m_fpError)
			fclose(m_fpError);
		m_fpError = 0;

		//切换到当前日期文件
		InitError();
		//切换文件
		m_bSwitchFile = true;
		return true;
	}
	//判断当前的文件名是否可用
	//取系统时间
	time( &m_time );
	//转成本地时间
	m_tNow = localtime(&m_time);
	//取错误文件名
	memset(m_chTempName,0,sizeof(m_chTempName));
	sprintf(m_chTempName,"%s/%s%04d-%02d-%02d.log",m_chPath,m_chNormal,m_tNow->tm_year+1900,m_tNow->tm_mon + 1,m_tNow->tm_mday);
	
	//文件名相同
	if(strcmp(m_chTempName,m_chNormalName) == 0) return true;

	//文件名不相同,切换到当前文件
	//关闭普通日志文件
	if(m_fpNormal)
		fclose(m_fpNormal);
	m_fpNormal = 0;
	
	//切换到当前日期文件
	InitNormal();
	//切换文件
	m_bSwitchFile = true;
	return true;
}


//一周前时间
#define WEEK_AGO	7 * 24 * 60 * 60
//删除一周前日志
void CLog::DelLog()
{
	//读取目录文件列表
	//打开文件夹
	DIR *dir = opendir((const char*)m_chPath);
	if(dir == NULL) 
	{
		LogErrorPara("打开日志文件夹[%s]失败,错误码[%d],无法删除历史日志!!!\n",m_chPath,errno);
		return;
	}
	//取当前日期
	//取系统时间
	time( &m_time );
	//一周前数据
	m_time -= WEEK_AGO;
	//转成本地时间(一周前的日期)
	m_tNow = localtime(&m_time);
	char szDate[11];
	memset(szDate,0,sizeof(szDate));
	sprintf(szDate,"%04d-%02d-%02d",m_tNow->tm_year+1900,m_tNow->tm_mon + 1,m_tNow->tm_mday);
#ifdef _DEBUG
	LogDefaultPara("清理日期:%s",szDate);
#endif
	//取目录下文件信息
	struct dirent *dirp = NULL;
	//读文件
	while(( dirp = readdir(dir) ) != NULL )
	{
		//不显示隐藏文件
		if(dirp->d_name == NULL || dirp->d_name[0] == '.' || strcmp(dirp->d_name,"..") == 0 ) continue;
		//判断文件类型
		if(strncmp(dirp->d_name,m_chNormal,strlen(m_chNormal)) == 0)
		{
			//判断是否小于一周前日期 
			if(strncmp(dirp->d_name + strlen(m_chNormal),szDate,strlen(szDate)) < 0)
			{
				//文件路径名
				memset(m_chTempName,0,sizeof(m_chTempName));
				sprintf(m_chTempName,"%s/%s",m_chPath,dirp->d_name);
				if(unlink(m_chTempName) < 0)
					LogErrorPara("删除普通历史日志[%s]失败!!!",m_chTempName);
				else
					LogDefaultPara("删除普通历史日志[%s]成功!!!",m_chTempName);
				continue;
			}
			continue;
		}
		//错误日志
		if(strncmp(dirp->d_name,m_chError,strlen(m_chError)) == 0)
		{
			//判断是否小于一周前的日期
			if(strncmp(dirp->d_name + strlen(m_chError),szDate,strlen(szDate)) < 0)
			{
				//文件路径名
				memset(m_chTempName,0,sizeof(m_chTempName));
				sprintf(m_chTempName,"%s/%s",m_chPath,dirp->d_name);
				if(unlink(m_chTempName) < 0)
					LogErrorPara("删除错误历史日志[%s]失败!!!",m_chTempName);
				else
					LogDefaultPara("删除错误历史日志[%s]成功!!!",m_chTempName);
				continue;
			}
			continue;
		}
		continue;
	}
	//关闭目录
	closedir(dir);
	return;
}


//错误日志
void LogError(char* chMsg)
{

	time_t timep;
	struct tm *m_tnow = 0;

	char m_chMsg[MAX_LOG_SIZE];
	memset(m_chMsg,0,sizeof(m_chMsg));

	time (&timep);
	m_tnow = localtime(&timep);
	sprintf(m_chMsg,"[%04d-%02d-%02d %02d:%02d:%02d][%s]\r\n",m_tnow->tm_year+1900,m_tnow->tm_mon + 1,m_tnow->tm_mday,\
	m_tnow->tm_hour,m_tnow->tm_min,m_tnow->tm_sec,chMsg);

	//记录错误日志
	g_logMonitor.LogError(m_chMsg);
//#ifdef _DEBUG
	printf("%s",m_chMsg);
//#endif

	return;
}
//默认日志
void LogDefault(char* chMsg)
{

	

	time_t timep;
	struct tm *m_tnow = 0;

	char m_chMsg[MAX_LOG_SIZE];
	memset(m_chMsg,0,sizeof(m_chMsg));

	time (&timep);
	m_tnow = localtime(&timep);
	sprintf(m_chMsg,"[%04d-%02d-%02d %02d:%02d:%02d][%s]\r\n",m_tnow->tm_year+1900,m_tnow->tm_mon + 1,m_tnow->tm_mday,\
	m_tnow->tm_hour,m_tnow->tm_min,m_tnow->tm_sec,chMsg);


	//记录正常日志
	g_logMonitor.LogDefault(m_chMsg);

//#ifdef _DEBUG
	printf("%s",m_chMsg);
//#endif

	return;
}


//多参输出错误日志
void LogErrorPara(const char* chMsg,...)
{
	do
	{
		char chTemp[MAX_LOG_SIZE];
		memset(chTemp,0,sizeof(chTemp));
		va_list varg;
		va_start(varg,chMsg);
        vsprintf(chTemp,chMsg,varg);
        va_end(varg);
		LogError(chTemp);
	}
	while (0);
}

//多参输出默认日志
void LogDefaultPara(const char* chMsg,...)
{
	do
	{
		char chTemp[MAX_LOG_SIZE];
		memset(chTemp,0,sizeof(chTemp));
		va_list varg;
		va_start(varg,chMsg);
        vsprintf(chTemp,chMsg,varg);
        va_end(varg);
		LogDefault(chTemp);
	}
	while (0);
}

