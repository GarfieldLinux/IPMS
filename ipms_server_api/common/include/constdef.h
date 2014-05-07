/************************************************************************************/
//模块名称：GSLB二期 常量定义头文件 
//功能说明：1)常量定义
//作者：徐永丰 tianya0513@hotmail.com
//日期：2008-9-1
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/
#ifndef __CONSTDEF_H__
#define __CONSTDEF_H__

#ifndef _MAX_PATH
	#define	_MAX_PATH 255
#endif

//默认缓冲长度,用于读取参数
#define DEFAULT_BUFFER_SIZE	8192
//配置文件名
#define DEFAULT_CONFIG_NAME	EXENAME".ini"
//IP地址使用的长度
#define DEFAULT_HOST_SIZE	32

//最大文件描述符
#define	MAX_FILENO_SIZE	10000

//传输命令以及状态码定义
/*******************************************************************************************/
#define DT_SUB		0x00000000			//二级查询
#define	DT_SUB_REP	0x80000000			//二级查询响应
#define	DT_ADD		0x00000001			//发送文件
#define DT_DEL		0x00000002			//删除文件
#define DT_ADEL_REP	0x80000001			//删除文件响应 与 发送文件响应相同


//前10个预留
#define DT_FILE		0x0000000A			//传输文件-----------未使用
#define DT_FILE_REP	0x8000000A			//传输文件响应


/*******************************************************************************************/
#define E_SUCCESS			0x00000000			//成功
#define E_BUSY_NOW			0x00000001			//节点忙
#define	E_FAILE				0x00000002			//删除失败
#define E_FILE_NOTEXIST		0x00000003			//删除时文件不存在
#define E_FILE_EXIST_DEL	0x00000004			//传输时发现已经存在的文件,删除失败
#define E_FILE_EXIST		0x00000005			//传输时发现文件已经存在,并且MD5相同
#define E_MD5_CHECK_FAIL	0x00000006			//传输完成后验证MD5失败
#define E_RENAME_FAIL		0x00000007			//传输完成后重命名失败
#define E_CONNECT_FAIL		0x00000008			//UDT连接错误
#define E_SUB_SYS			0x00000009			//二级分发机,需再次查询二级分发机状态
#define E_SEND_ERROR		0x0000000A			//发送数据失败
#define E_RECV_ERROR		0x0000000B			//接收数据失败
#define E_SUB_NOEXIST		0x0000000C			//二级分发机,返回任务不存在,重新来过
#define	E_FEINT_STATE		0x0000000D			//二级分发机返回假象完成状态

#define E_MD5_FAILED	0x00000004			//MD5验证失败
#define	E_WRITE_ERROR	0x00000005			//写文件失败





/*******************************************************************************************/




#endif //__CONSTDEF_H__
