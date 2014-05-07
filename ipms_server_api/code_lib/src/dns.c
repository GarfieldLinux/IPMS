#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <dns.h>

#ifndef DNS_BUFFER_SIZE
#define DNS_BUFFER_SIZE		8192
#endif

#ifndef DNS_DOMAIN_SIZE
#define	DNS_DOMAIN_SIZE		256
#endif

#ifndef DEFAULT_BUFFER_READ
#define	DEFAULT_BUFFER_READ	81920
#endif

//域名转换
void ChangetoDnsNameFormat(unsigned char* dns,const char* domain)
{
	
	char host[DNS_DOMAIN_SIZE];
	int lock = 0 ,i;

	memset(host,0,sizeof(host));
	strcpy(host,domain);
	strcat((char*)host,".");
	for(i = 0 ;i < (int)strlen( (char*)host ) ; i++ )
	{
		if( host[i] == '.')
		{
			*dns++ = i - lock;
			for(;lock < i ; lock ++)
			{
				*dns++ = host[lock];
			}
			lock ++;
		}
	}
    *dns++ = '\0';
	return;
}

//描述:域名解析
//参数:	pszHost			DNS主机IP
//		pszDomainName	解析的域名
//		nTimeout		解析超时值
//返回值: 正确则返回解析到的A记录数,否则返回0
int dig(const char* pszHost,const char* pszDomainName,const int nTimeout)
{
	//数据缓存区
	unsigned char buff[DNS_BUFFER_SIZE];
	unsigned char*			qname = NULL;
	struct DNS_QUESTION*	qinfo = NULL;
	//目标主机sockaddr
	struct sockaddr_in addr_to;
	struct DNS_HEADER *dns = NULL;
	int addr_len = 0;
	//
	struct timeval tv_out;
    tv_out.tv_sec = nTimeout;
    tv_out.tv_usec = 0;
	//初始化SOCK FD
	int fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(fd < 0) return 0;
	//设置超时时间
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
	//设置缓冲区大小(监听设置无用)
	//addr_len = DEFAULT_BUFFER_READ;
	//setsockopt(fd,SOL_SOCKET,SO_RCVBUF,(const char*)&addr_len,sizeof(int));
	//setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(const char*)&addr_len,sizeof(int));

	//目标主机信息
	addr_to.sin_family = AF_INET;
	addr_to.sin_port=htons(53);
	addr_to.sin_addr.s_addr=inet_addr(pszHost);  //dns servers
	//DNS协议头
	dns = (struct DNS_HEADER*)&buff;
	srand(time(NULL));
	dns->id = (unsigned short) htons(rand()%65535);
	dns->qr = 0;dns->opcode = 0;dns->aa = 1;dns->tc = 0;dns->rd = 1;dns->ra = 0;dns->z = 0;dns->rcode = 0;
	//问题数
	dns->q_count = htons(1);
	//其他段均为0
	dns->ans_count = dns->auth_count = dns->add_count = 0;
	//域名
	qname =(unsigned char*)&buff[sizeof(struct DNS_HEADER)];
	ChangetoDnsNameFormat(qname,pszDomainName);
	//设置问题段其他参数
	qinfo =(struct DNS_QUESTION*)&buff[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)];
	qinfo->qtype = htons(1);qinfo->qclass = htons(1); 
	//发送查询数据包
	if(sendto(fd,(char*)buff,sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1) + sizeof(struct DNS_QUESTION),0,(struct sockaddr*)&addr_to,sizeof(addr_to)) == -1)
		goto error;
	//接收响应
	addr_len = sizeof(addr_to);
	if(recvfrom (fd,(char*)buff,sizeof(buff),0,(struct sockaddr*)&addr_to,(socklen_t*)&addr_len) == -1)
	{
		//超时
		if(errno == EAGAIN)
		{
#ifdef _DEBUG
			printf("Timeout,just think the package loss ......\n");
#endif
			//丢包,暂时认为正常
			shutdown(fd,2);
			close(fd);
			return 1;
		}
		goto error;
	}
	//解析返回
	dns = (struct DNS_HEADER*)buff;
#ifdef _DEBUG
    printf("The response contains : \n");
	printf("%d Questions.\n",ntohs(dns->q_count));
	printf("%d Answers.\n",ntohs(dns->ans_count));
	printf("%d Authoritative Servers.\n",ntohs(dns->auth_count));
	printf("%d Additional records.\n",ntohs(dns->add_count));
#endif
	shutdown(fd,2);
	close(fd);
	return ntohs(dns->ans_count);
error:
	shutdown(fd,2);
	close(fd);
	return 0;
}

