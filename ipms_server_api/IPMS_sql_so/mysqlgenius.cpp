/*
 * =====================================================================================
 *
 *       Filename:  mysqlgenius.cpp
 *
 *    Description:  mysql 事物测试例子 
 *
 *        Version:  1.0
 *        Created:  2011年07月13日 14时30分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  GarfieldLinux (zjf), garfieldlinux@gmail.com
 *        Company:  南通天联信息技术
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
#include "mysqlgenius.h"

int main()
{
	MYSQL mysql;     //mysql连接 
	MYSQL_RES *res; //这个结构代表返回行的一个查询结果集 
	MYSQL_ROW row; //一个行数据的类型安全(type-safe)的表示
	char *query;  //查询语句 
	int t,r;
	mysql_init(&mysql);


	if (!mysql_real_connect(&mysql,"localhost", "root", "garfield", "test01",3306,NULL,0))
	{
		printf( "Error connecting to database: %s\n",mysql_error(&mysql));
		return 0;
	}  else 
		printf("Connected\n");


	query="SET CHARACTER SET UTF8"; //设置编码 
	t=mysql_real_query(&mysql,query,(unsigned int)strlen(query));
	if(t)
	{
		printf("编码设置失败\n");
	}


//////////////////查询
	query=" select * from dn ";
	t=mysql_real_query(&mysql,query,(unsigned int)strlen(query));
	if(t)
	{
		printf("执行查询时出现异常: %s",mysql_error(&mysql));

	}else{
		printf("[%s] 构建成功 \n",query);
	}
	res=mysql_store_result(&mysql);
	while(row=mysql_fetch_row(res))
	{                              
		for(t=0;t<mysql_num_fields(res);t++)
		{
			printf("%s\t",row[t]);
		}
		printf("\n");
	}
	mysql_free_result(res);

//事务处理开始
	t=mysql_real_query(&mysql,"SET AUTOCOMMIT =0",(unsigned int)strlen("SET AUTOCOMMIT =0"));
	if(t){
		printf("启用手工事务失败\n");
	}else{
		printf("启用手工事务成功\n");
	}
	t=mysql_real_query(&mysql,"Begin ;",(unsigned int)strlen("Begin ;"));
	
	query="insert into dn(name,age)values('数据','89')";
	int inset_result=dninsert(&mysql,query);

	query="delete  from dn  where name='数据'";
	int delete_result=dndelete(&mysql,query);

	query="update dn set name='测试数据' where name='ccd' and inc_id=13";
	int update_result=dnupate(&mysql,query); 

	query="insert into dn(inc_idi,name,age)values(16,'数据','89')";//执行会出现异常的语句 
	int fail_result=dninsert(&mysql,query);

	if(inset_result==0 && delete_result==0 && update_result==0 && fail_result==0){
		printf("事务提交\n");
		t=mysql_real_query(&mysql,"COMMIT;",(unsigned int)strlen("COMMIT;"));
	}else{
		printf("事务回滚\n");
		t=mysql_real_query(&mysql,"ROLLBACK;",(unsigned int)strlen("ROLLBACK;"));
	}

printf("断开mysql连接…… \n");


	return 0;  
}
