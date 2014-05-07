/*
 * =====================================================================================
 *
 *       Filename:  a.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年06月20日 11时20分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  GarfieldLinux (zjf), garfieldlinux@gmail.com
 *        Company:  南通天联信息技术
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <mysql/mysql.h>
  

void show()
{
	printf("begin \n");
}

 void database_select()
{

	printf("conn  to databases ; \n");

}


/*

 void database_select_so()
{
	char *user="root" ,*pwd="garfield" ,*dbname="garfield" ;
	MYSQL mysql ;
	MYSQL_RES *mysql_ret ;
	MYSQL_ROW  mysql_row ;
	unsigned long num_rows ;
	int ret ;
	mysql_init(&mysql);
	if(mysql_real_connect(&mysql,"192.168.1.111",user,pwd,dbname,0,NULL,0))
	{
	    printf("连接成功! \t");
	    ret  =  mysql_query(&mysql,"select  *  from  IPMS");
	    string sql = "insert into IPMS values(1,'201.134.56.12',123456,'201.134.56.156',2342522,'zhejiang','    zhejiangdianxin')" ;
	    if(!ret)
	    {
	       printf("查询成功!\t");
	       mysql_ret  =  mysql_store_result(&mysql);
	       if(mysql_ret  !=  NULL)
	       {
	           printf("存储结果成功!\t");
	           num_rows  =  mysql_num_rows(mysql_ret);
	           if(num_rows  !=  0)
	           {
	               printf("一共查询出 %d 条记录 \n",num_rows);
	               while(mysql_row  =  mysql_fetch_row(mysql_ret))
	               {
	                printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n",mysql_row[0],mysql_row[1],mysql_row[2],mysql_row[3],mysql_row[4],mysql_row[5],mysql_row[6]);
	               }
	           }
			   else{    }
				  mysql_free_result(mysql_ret);
				 }
				else
				{   }
		}
				else{   }
	}
					  else {      printf("连接失败\n");  }
}

*/


