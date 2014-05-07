/************************************************************************************/
//模块名称：数据同步服务端 公共类
//功能说明：1)全局变量  
//			2)全局函数
//作者：徐永丰 tianya0513@hotmail.com
//日期：2008-9-1
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__

#include "global.h"
#include "customdef.h"
#include "conf.h"
#include "log.h"
#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include "md5.h"
#include <openssl/sha.h>
#include "rwlock.h"
#ifdef	_TRANSFER_CLIENT
	#include "mailmanager.h"
	#include "taskcenter.h"
	#include "taskmanager.h"
#endif
//全局的系统参数
extern STRUCT_CONF_T	g_conf_t;

//获取文件大小
__int64_t _get_file_size(const std::string strFileName);

//获取文件MD5和HASH
std::string _get_file_md5sha1(const std::string strFileName,const int nKeySize);

//获取文件MD5
std::string _get_file_md5(const std::string strFileName);
//获取文件SHA1
std::string _get_file_sha1(const std::string strFileName);
//设置最大打开的文件句柄数
void _set_max_fileno();

//判断是否为目录
bool IsDir(const std::string strPathName);


#endif //__COMMON_H__
