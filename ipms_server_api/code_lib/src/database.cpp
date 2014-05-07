#include "common.h"
#include "database.h"


//���캯��
CDatabase::CDatabase()
{
	//����
	m_connMysql = NULL;
	return;
}

//��������
CDatabase::~CDatabase()
{
	//�Ͽ�����
	if(m_connMysql)
		Close();
	return;
}

//��ʼ��
bool CDatabase::Open(const char* szHost,const char* szName,const char* szUser,const char* szPass)
{
	//��ʼ��
	m_connMysql = mysql_init(NULL);
	if(m_connMysql == NULL)
	{	
		//��ʼ������ʧ��
		return false;
	}
	//����
	if (!mysql_real_connect(m_connMysql, szHost, szUser, szPass, szName, 3306, NULL,CLIENT_MULTI_STATEMENTS)) 
	{ 
		return false;
	}
	//�����ַ���
	mysql_query(m_connMysql,"SET NAMES utf8");
	return true;
}
//�ر�����
void CDatabase::Close()
{
	//�ر�db����
	if(m_connMysql != 0)
		mysql_close(m_connMysql); 
	m_connMysql = 0;
	return;
}
//ִ��SQL
bool CDatabase::ExeSQL(const char* chSQL)
{
	if(!m_connMysql)
	{
		return false;
	}
	int nTimes = 0;
	while((nTimes ++) < 3)
	{
		//ִ��sql
		if(mysql_query(m_connMysql, chSQL) == 0) return true;
		//��Ϣһ��
		sleep(1);
	}	
	return false;
}

