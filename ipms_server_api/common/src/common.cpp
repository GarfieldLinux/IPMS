#include "common.h"


//全局的系统参数
STRUCT_CONF_T	g_conf_t;

//获取文件大小
__int64_t _get_file_size(const std::string strFileName)
{
	struct stat64 st;
	memset(&st,0,sizeof(st));
	if(stat64(strFileName.c_str(),&st)  == 0) return st.st_size;
	return 0;
}



//获取文件MD5和HASH
std::string _get_file_md5sha1(const std::string strFileName,const int nKeySize)
{
	if(nKeySize == 40) return _get_file_sha1(strFileName);
	return _get_file_md5(strFileName);
}


#define MD5STR(str,md5)    \
	sprintf(str,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",md5[0],md5[1],md5[2],\
        md5[3],md5[4],md5[5],md5[6],md5[7],md5[8],md5[9],md5[10],md5[11],md5[12],md5[13],md5[14],md5[15]);

//获取文件MD5
std::string _get_file_md5(const std::string strFileName)
{
	std::string strMsg;
	//计算MD5
	MD5_CTX ctx;
	unsigned char szBuffer[10240], szDigest[16];
	int nLength = 0;
	std::ifstream ifs(strFileName.c_str(), std::ios::in|std::ios::binary);
	if(ifs.is_open())
	{
		while (!ifs.eof())
		{
			ifs.read((char*)szBuffer, sizeof(szBuffer));
			nLength = ifs.gcount();
			//可能读取硬盘数据失败，此时需要中止循环
			if(nLength == 0) break;
			ctx.MD5Update (szBuffer, nLength);
			usleep(1);
		}
		ctx.MD5Final (szDigest);
		ifs.close();
		//
		char szMd5[33] = {'\0'};
		MD5STR(szMd5,szDigest);
		//保持MD5值
		strMsg = szMd5;
	}
	return strMsg;
}

#define SHA1STR(str,sha1)    \
	sprintf(str,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",sha1[0],sha1[1],sha1[2],\
        sha1[3],sha1[4],sha1[5],sha1[6],sha1[7],sha1[8],sha1[9],sha1[10],sha1[11],sha1[12],sha1[13],sha1[14],sha1[15],sha1[16],sha1[17],sha1[18],sha1[19]);
//获取文件SHA1
std::string _get_file_sha1(const std::string strFileName)
{
	std::string strMsg;
	//计算SHA1
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	unsigned char szBuffer[10240], szDigest[20];//1024->10240
	int nLength = 0;
	std::ifstream ifs(strFileName.c_str(), std::ios::in|std::ios::binary);
	if(ifs.is_open())
	{
		while (!ifs.eof())
		{
			ifs.read((char*)szBuffer, sizeof(szBuffer));
			nLength = ifs.gcount();
			//可能读取硬盘数据失败，此时需要中止循环
			if(nLength == 0) break;
			SHA1_Update(&ctx,szBuffer, nLength);
			usleep(1);//把进程挂起百万分之一秒
		}
		SHA1_Final(szDigest,&ctx);
		ifs.close();
		//
		char szSha1[41] = {'\0'};
		SHA1STR(szSha1,szDigest);
		//保持MD5值
		strMsg = szSha1;
	}
	return strMsg;
}

//设置打开的最大文件句柄数
void _set_max_fileno()
{
	struct rlimit rt;
    /* 设置每个进程允许打开的最大文件数 */
    rt.rlim_max = rt.rlim_cur = MAX_FILENO_SIZE;
    setrlimit(RLIMIT_NOFILE, &rt);
	return;
}

//判断是否为目录
bool IsDir(const std::string strPathName)
{
	struct stat64 st;
	memset(&st,0,sizeof(st));
	if(stat64(strPathName.c_str(),&st)  < 0) return false;
	if(S_ISDIR(st.st_mode))	return true;
	return false;
}
