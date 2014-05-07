/************************************************************************************/
//模块名称：自定义常量以及结构体
//功能说明：1)常量
//			2)结构体
//作者：徐永丰 tianya0513@hotmail.com
//日期：2008-9-1
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/

#ifndef __CUSTOMDEF_H
#define	__CUSTOMDEF_H

#include "global.h"


#ifndef SyncList	
	typedef std::set<std::string> SyncList;
#endif

#ifndef RhsvrList
	typedef SyncList RhsvrList;
#endif

//允许访问的主机列表
#ifndef AllowHost	
	typedef SyncList AllowHost;
#endif

//程序参数结构体
typedef struct _STRUCT_CONF_T
{
#ifndef	SUB_SYS
	std::string strDbSvr;				//数据库服务器IP
	std::string strDbUsr;				//数据库用户名
	std::string strDbPas;				//数据库密码
	std::string strSvrHost;				//服务IP,查询任务条件
	AllowHost	allowHost;				//允许连接的主机IP集
#else
//二级分发机使用
	__int32_t nThansfer_thread_size;			//分发接收线程
	__int32_t nTransfer_recv_timeout;			//接收超时时间
	__int32_t nTransfer_send_timeout;			//发送超时时间
#endif

	std::string strStorage_path;		//分发存储路径
	SyncList	listSync;				//分发的同步列表
	RhsvrList	listRhsvr;				//分发的rhsvr列表
	__int32_t	nTaskUpdateDealy;		//任务读取/更新间隔,单位为秒
	__int32_t	nTaskThreadSize;		//任务的线程数
	__int32_t	nTaskErrorTreadSize;	//任务的错误列表处理线程数
	__int32_t	nTaskErrorDealDelay;	//任务的错误列表中任务处理间隔时间
	__int32_t	nTaskDecompostSize;		//任务分解线程数
	__int32_t	nTaskMaxBandWidth;		//限制带宽数
	__int32_t	nTaskMaxRetryTime;		//任务失败重试次数
	__int32_t	nTaskMaxBacklog;		//任务最大积压报警数
#ifndef	SUB_SYS
	__int32_t	nTaskFeintDealDelay;	//假象完成任务的重试间隔,默认为1小时
#endif
	std::string strMailTo;				//发送邮件列表
	std::string strMailTitle;			//发送邮件标题
	_STRUCT_CONF_T()
	{
#ifndef	SUB_SYS
		strDbSvr = "127.0.0.1";
		strDbUsr = "";
		strDbPas = "";
		strSvrHost = "127.0.0.1";
#else
//二级分发机使用		
		nThansfer_thread_size = 21;
		nTransfer_recv_timeout = 10;
		nTransfer_send_timeout = 10;
#endif
		strStorage_path ="/cache/data";
		listSync;
		listRhsvr;
		nTaskUpdateDealy = 5 * 60;		//默认为5分钟
		nTaskThreadSize = 20;
		nTaskErrorTreadSize = 1;		//默认为1线程
		nTaskErrorDealDelay = 5 * 60;	//默认为5分钟
		nTaskDecompostSize = 10;
		nTaskMaxBandWidth = 0;
		nTaskMaxRetryTime = 3;
		nTaskMaxBacklog = 1000;
#ifndef	SUB_SYS
		nTaskFeintDealDelay = 1;	//假象完成任务的重试间隔
#endif
		strMailTo = "";
		strMailTitle = "Transfer System";
	};
}STRUCT_CONF_T;




//任务结构
typedef struct _STRUCT_TASK_ITEM_T
{
	std::string strPubUrl;			//发布的URL
	__int64_t	nFileLength;		//文件长度
	std::string	strFileMd5;			//文件MD5
	__int32_t	nType;				//操作类型 添加 删除 重命名
	__int32_t	nPriority;			//优先级,默认为0
	__int32_t	nFeint;				//慢恢复状态任务
	std::string	strNewName;			//重命名使用	
	_STRUCT_TASK_ITEM_T()
	{
		strPubUrl = "";
		nFileLength = 0;
		strFileMd5 = "";
		nType = 1;					//DT_ADD DT_DEL DT_RENAME
		nPriority = 0;				//级别越高,入队列越早
		nFeint = 0;
		strNewName = "";
	}
}STRUCT_TASK_ITEM_T;

//任务状态
typedef struct _STRUCT_TASK_STATE_T
{
	__int32_t	nDealing;			//正在处理状态,用于二次读取任务时保持状态
	__int32_t	nCompleteNumber;	//完成数,用于更新状态
	__int32_t	nFeint;				//假象
	__int64_t	nDealTime;			//进队时间,完成后变迁为完成时间
	__int32_t	nUseTime;			//使用时间
	_STRUCT_TASK_STATE_T()
	{
		nDealing = 0;
		nCompleteNumber = 0;
		nFeint = 0;						
		nDealTime = time(NULL);

		nUseTime = 0;
	}
}STRUCT_TASK_STATE_T;


//任务
typedef struct _STRUCT_TASK_T
{
	__int64_t	nTaskID;			//任务ID,使用64位存储
	STRUCT_TASK_STATE_T sTaskState;	//任务状态
	STRUCT_TASK_ITEM_T	sTaskItem;	//任务内容
	_STRUCT_TASK_T()
	{
		nTaskID = 0;
	}
}STRUCT_TASK_T;


//发送实体的任务结构
typedef struct _STRUCT_ENTITY_TASK_T
{
	__int64_t	nTaskID;			//任务ID,用于更新状态
	std::string strEntityHost;		//实体 HOST
	std::string	strHost;			//任务的接收IP
	__int32_t	nTryTimes;			//重试的次数
	__int32_t	nType;				//任务类型
	__int32_t	nTypeBefore;		//状态变迁为二级状态前的任务类型
	__int32_t	nPriority;			//优先级
	__int32_t	nFeint;				//假象完成的故障任务
	__int64_t	nStartTime;			//传输开始时间
	__int64_t	nEndTime;			//传输结束时间
	__int32_t	nSendSpeed;			//发送速度 Mb/sec
	std::string strPubUri;			//去掉存储的文件路径名,用于到节点组合真实路径名
	std::string strTrueName;		//真实的文件路径名,用于传输数据
	std::string strFileMd5;			//文件的MD5
	__int64_t	nFileLength;		//文件长度
	__int64_t	nErrorTime;			//处理错误时间
	_STRUCT_ENTITY_TASK_T()
	{
		nTaskID = 0;
		strEntityHost = "";
		strHost = "";
		nTryTimes = 0;
		nType = DT_ADD;
		nTypeBefore = DT_ADD;			
		nPriority = 0;
		nFeint = 0;
		nStartTime = 0;
		nEndTime = 0;
		nSendSpeed = 0;				//发送速度
		strPubUri = "";
		strTrueName = "";
		strFileMd5 = "";
		nFileLength = 0;
		nErrorTime = time(NULL);
	}
}STRUCT_ENTITY_TASK_T;




//邮件处理描述
#define DEFAULT_MAIL_DEAL	"此邮件为帝联分发系统报警邮件<br>"\
							"邮件说明:<br>"\
							"<font color=#9999CC>1.本邮件由帝联分发系统自动发出,请及时处理并回复;</font><br>"\
							"级别说明:<br>"\
							"<font color=#FF0000>1.一级报警,必须通知负责人处理.包括[1.任务文件不存在;2.任务格式不正确;3.其他一些不应出现的错误.]</font><br>"\
							"<font color=#FF8F00>2.二级报警,保持关注,频繁出现时通知负责人处理.包括[1.任务状态变迁.]</font><br>"




#endif //__CUSTOMDEF_H

