/*
 * =====================================================================================
 *
 *       Filename:  mysqlgenius.h
 *
 *    Description:  mysql 事物处理的头文件
 *
 *        Version:  1.0
 *        Created:  2011年07月13日 14时27分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  GarfieldLinux (zjf), garfieldlinux@gmail.com
 *        Company:  南通天联信息技术
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>


int dninsert(MYSQL *mysql,char *strsql)
{    
	int t; 
	MYSQL_RES *res;
	t=mysql_real_query(mysql,strsql,(unsigned int)strlen(strsql));

	if(t){
		printf( "Error id=%d  Error: %s\n",mysql_errno(mysql),mysql_error(mysql));
		return mysql_errno(mysql);
	}else{
		res=mysql_store_result(mysql);  
		printf("插入行数=%d\n",mysql_affected_rows(mysql));
		mysql_free_result(res);
	}

	return 0;

}

int dnupate(MYSQL *mysql,char *strsql)
{    
	int t; 
	MYSQL_RES *res;
	t=mysql_real_query(mysql,strsql,(unsigned int)strlen(strsql));
	if(t){
		printf( "Error id=%d  Error: %s\n",mysql_errno(mysql),mysql_error(mysql));
		return mysql_errno(mysql);
	}else{
		res=mysql_store_result(mysql);  
		printf("更新行数=%d\n",mysql_affected_rows(mysql));
		mysql_free_result(res);
	}
	return 0;
}

int dndelete(MYSQL *mysql,char *strsql)
{    
	int t; 
	MYSQL_RES *res;
	t=mysql_real_query(mysql,strsql,(unsigned int)strlen(strsql));
	if(t){
		printf( "Error id=%d  Error: %s\n",mysql_errno(mysql),mysql_error(mysql));
		return mysql_errno(mysql);
	}else{
		res=mysql_store_result(mysql);  
		printf("删行数=%d\n",mysql_affected_rows(mysql));
		mysql_free_result(res);
	}
	return 0;

}




