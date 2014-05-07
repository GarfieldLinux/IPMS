/************************************************************************************/
//ģ�����ƣ����ݿ�ģ��
//����˵����1)���ݿ�����
//			2)���ݲ�ѯ
//			3)����INSERT
//���ߣ������� tianya0513@hotmail.com
//���ڣ�2008-9-1
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/
#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "global.h"
#include <mysql/mysql.h>


/***********************************************************************************/
//���ݿ����
/***********************************************************************************/
class CDatabase  
{
//���к���
public:
	//���캯��
	CDatabase();
	//��������
	virtual~CDatabase();
public:
	//��ʼ�����Ӳ���
	bool Open(const char* szHost,const char* szName,const char* szUser,const char* szPass);
	//�ر�����
	void Close();
//��������
protected:
	//ִ��SQL
	bool ExeSQL(const char* chSQL);
//������������������
protected:
	//MYSQL ����
	MYSQL *m_connMysql;
	//SQL��仺����
	char szSQL[DEFAULT_BUFFER_SIZE];
	//��¼����
	MYSQL_ROW row;
};
#endif //__DATABASE_H__
