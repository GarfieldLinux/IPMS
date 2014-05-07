#ifndef __DNS_H
#define	__DNS_H



//DNS header structure
struct DNS_HEADER
{
	unsigned	short id;		    // identification number
	
	unsigned	char rd     :1;		// recursion desired
	unsigned	char tc     :1;		// truncated message
	unsigned	char aa     :1;		// authoritive answer
	unsigned	char opcode :4;	    // purpose of message
	unsigned	char qr     :1;		// query/response flag
	
	unsigned	char rcode  :4;	    // response code
	unsigned	char z      :3;		// its z! reserved
	unsigned	char ra     :1;		// recursion available
	
	unsigned    short q_count;	    // number of question entries
	unsigned	short ans_count;	// number of answer entries
	unsigned	short auth_count;	// number of authority entries
	unsigned	short add_count;	// number of resource entries
};


//Constant sized fields of query structure
struct DNS_QUESTION
{
	unsigned short qtype;
	unsigned short qclass;
};


//Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct  DNS_RDATA
{
	unsigned short type;
	unsigned short _class;
	unsigned int   ttl;
	unsigned short data_len;
};
#pragma pack(pop)


//Pointers to resource record contents
struct DNS_RECORD
{
	unsigned char		*name;
	struct DNS_RDATA	*resource;
	unsigned char		*rdata;
};


//描述:域名解析
//参数:	pszHost			DNS主机IP
//		pszDomainName	解析的域名
//		nTimeout		解析超时值
//返回值: 正确则返回解析到的A记录数,否则返回0
int dig(const char* pszHost,const char* pszDomainName,const int nTimeout);

//域名转换
void ChangetoDnsNameFormat(unsigned char* dns,const char* domain);

#endif //__DNS_H
