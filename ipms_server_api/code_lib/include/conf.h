#ifndef __CONF_H
#define	__CONF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//转换为小写
char* tolowstr(char* str);
//根据键值获取VALUE
int get_value(char*line_buf,const char*logo,char**pval,bool raw = false);
//根据键值做不同处理
#define GET_STRING(Logo,RetVal) {char*ptr;int r;r=get_value(line_buf,Logo,&ptr);if(r>0)RetVal = ptr;if(r!=0)continue;}
#define GET_INT(Logo,RetVal)   {char*ptr;int r;r=get_value(line_buf,Logo,&ptr);if(r>0)RetVal = atoi(ptr);if(r!=0)continue;}	                            
#define GET_STRING_LOW(Logo,RetVal) {char*ptr;int r;r=get_value(line_buf,Logo,&ptr);if(r>0){tolowstr(ptr);RetVal = ptr;}if(r!=0)continue;}
#define GET_FLOAT(Logo,RetVal)  {char*ptr;int r;r=get_value(line_buf,Logo,&ptr);if(r>0)RetVal= (float)atof(ptr);if(r!=0)continue;}
#define GET_RAW_STRING(Logo,RetVal) {int r; RetVal=0; r=get_value(line_buf,Logo,&RetVal,true);if(r<0)continue; }
#define GET_STRING_SET(Logo,RetVal) {char*ptr;int r;r=get_value(line_buf,Logo,&ptr);if(r>0){ RetVal.insert(ptr); }if(r!=0)continue;}


#endif //__CONF_H

