/*
 * =====================================================================================
 *
 *       Filename:  sotest.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年06月20日 09时17分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  GarfieldLinux (zjf), garfieldlinux@gmail.com
 *        Company:  南通天联信息技术
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <dlfcn.h>
#include "Config.h"

#define SOFILE "./config.so"
//int (*f)();
int main()
{
	int port ;
	printf("begin use so ! \n");
	 const char ConfigFile[]= "IPMS.conf" ;

	Config configSettings(ConfigFile);
	
	port = configSettings.Read("port",0);
	printf("port:%d \n",port);
	return 0;
}
