/*
 * =====================================================================================
 *
 *       Filename:  database_so.h
 *
 *    Description:  so 导出头文件
 *
 *        Version:  1.0
 *        Created:  2011年06月20日 14时06分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  GarfieldLinux (zjf), garfieldlinux@gmail.com
 *        Company:  南通天联信息技术
 *
 * =====================================================================================
 */

#ifndef __dll_h__
#define __dll_h__

#ifdef __MY_DLL_LIB__
    #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
    #define DLL_EXPORT extern "C" __declspec(dllimport)
#endif
DLL_EXPORT int jmax(int x, int y);
#endif



