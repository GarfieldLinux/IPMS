/************************************************************************************/
//模块名称：数据库模块
//功能说明：1)数据库连接
//			2)数据查询
//			3)数据INSERT
//作者：徐永丰 tianya0513@hotmail.com
//日期：2008-9-1
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/
#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "global.h"
#include <mysql/mysql.h>


/***********************************************************************************/
//数据库基类
/***********************************************************************************/
class CDatabase  
{
//公有函数
public:
	//构造函数
	CDatabase();
	//析构函数
	virtual~CDatabase();
public:
	//初始化连接参数
	bool Open(const char* szHost,const char* szName,const char* szUser,const char* szPass);
	//关闭连接
	void Close();
//保护函数
protected:
	//执行SQL
	bool ExeSQL(const char* chSQL);
//保护变量，用于重载
protected:
	//MYSQL 连接
	MYSQL *m_connMysql;
	//SQL语句缓冲区
	char szSQL[DEFAULT_BUFFER_SIZE];
	//记录数据
	MYSQL_ROW row;
};
#endif //__DATABASE_H__
