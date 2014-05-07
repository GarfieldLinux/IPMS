#ifndef __CONF_H
#define	__CONF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//ת��ΪСд
char* tolowstr(char* str);
//���ݼ�ֵ��ȡVALUE
int get_value(char*line_buf,const char*logo,char**pval,bool raw = false);
//���ݼ�ֵ����ͬ����
#define GET_STRING(Logo,RetVal) {char*ptr;int r;r=get_value(line_buf,Logo,&ptr);if(r>0)RetVal = ptr;if(r!=0)continue;}
#define GET_INT(Logo,RetVal)   {char*ptr;int r;r=get_value(line_buf,Logo,&ptr);if(r>0)RetVal = atoi(ptr);if(r!=0)continue;}	                            
#define GET_STRING_LOW(Logo,RetVal) {char*ptr;int r;r=get_value(line_buf,Logo,&ptr);if(r>0){tolowstr(ptr);RetVal = ptr;}if(r!=0)continue;}
#define GET_FLOAT(Logo,RetVal)  {char*ptr;int r;r=get_value(line_buf,Logo,&ptr);if(r>0)RetVal= (float)atof(ptr);if(r!=0)continue;}
#define GET_RAW_STRING(Logo,RetVal) {int r; RetVal=0; r=get_value(line_buf,Logo,&RetVal,true);if(r<0)continue; }
#define GET_STRING_SET(Logo,RetVal) {char*ptr;int r;r=get_value(line_buf,Logo,&ptr);if(r>0){ RetVal.insert(ptr); }if(r!=0)continue;}


#endif //__CONF_H

