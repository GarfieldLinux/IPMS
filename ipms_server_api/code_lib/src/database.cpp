#include "common.h"
#include "database.h"


//构造函数
CDatabase::CDatabase()
{
	//连接
	m_connMysql = NULL;
	return;
}

//析构函数
CDatabase::~CDatabase()
{
	//断开连接
	if(m_connMysql)
		Close();
	return;
}

//初始化
bool CDatabase::Open(const char* szHost,const char* szName,const char* szUser,const char* szPass)
{
	//初始化
	m_connMysql = mysql_init(NULL);
	if(m_connMysql == NULL)
	{	
		//初始化连接失败
		return false;
	}
	//连接
	if (!mysql_real_connect(m_connMysql, szHost, szUser, szPass, szName, 3306, NULL,CLIENT_MULTI_STATEMENTS)) 
	{ 
		return false;
	}
	//设置字符集
	mysql_query(m_connMysql,"SET NAMES utf8");
	return true;
}
//关闭连接
void CDatabase::Close()
{
	//关闭db连接
	if(m_connMysql != 0)
		mysql_close(m_connMysql); 
	m_connMysql = 0;
	return;
}
//执行SQL
bool CDatabase::ExeSQL(const char* chSQL)
{
	if(!m_connMysql)
	{
		return false;
	}
	int nTimes = 0;
	while((nTimes ++) < 3)
	{
		//执行sql
		if(mysql_query(m_connMysql, chSQL) == 0) return true;
		//休息一秒
		sleep(1);
	}	
	return false;
}

