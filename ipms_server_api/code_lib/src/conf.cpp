#include "conf.h"

//大写转小写
char* tolowstr(char* str)
{
	char* p=str;
	while(*p){
		if(*p >= 'A' && *p <= 'Z')*p = *p + ('a'-'A' );
		++p;
	}
	return str;
}

//读一行配置文件,根据指定的log键值,读取VALUE
int get_value(char*line_buf,const char*logo,char**pval,bool raw /* = false */)
{
	char*ptr = line_buf;
	while(*ptr && isspace(*ptr))++ptr;
	if(!*ptr || *ptr =='#' || *ptr == '/')
		return -1;
	int logo_len = strlen(logo);
	if(memcmp(ptr,logo,logo_len ) == 0 && isspace(*(ptr+logo_len )) ){

		char*p = ptr + logo_len;
		while(*p && ( isspace(*p) || *p =='=' ) )++p;
		char *q=p;
		if(raw){
			while(*q && *q != '\n' )++q;
			*q = 0;
			*pval = p;
			return 1;
		}
		///
		if(*q == '\"'){
			char*t = strchr(q+1,'\"');if(!t)return -1;
			*t = '\0';
			*pval = q+1;
			return 1;
		}
		while(*q && !isspace(*q) /*&& *q != '#'*/ && *q != '\n')++q; *q = 0;
		*pval = p;
		return 1;
	}
	//
	return 0;
}

