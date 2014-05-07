/*
 * =====================================================================================
 *
 *       Filename:  CCode.c
 *
 *    Description:  i
 *
 *        Version:  1.0
 *        Created:  2011年06月20日 14时22分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  GarfieldLinux (zjf), garfieldlinux@gmail.com
 *        Company:  南通天联信息技术
 *
 * =====================================================================================
 */

#include <stdio.h>  
int main()  
{  
        char* val=(char*)malloc(50*sizeof(char));  
        strcpy(val,"HI,this is Thunderclock"); //must be less than 50 chars  
        val=getNewString(val);  
        printf("New String is %s \n",val); 

		database_select();

        return 0;  
 }
