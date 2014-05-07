/*
 * =====================================================================================
 *
 *       Filename:  ConfigTest.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年06月18日 23时11分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  GarfieldLinux (zjf), garfieldlinux@gmail.com
 *        Company:  南通天联信息技术
 *
 * =====================================================================================
 */

#include "Config.h"  
int main()  
{  
	    int port;  
	    std::string ipAddress;  
	    std::string username;  
	    std::string password;  
		std::string dbname;
	   	//const char ConfigFile[]= "ipms.conf"; 
	    const char ConfigFile[]= "IPMS.conf" ;	
	    Config configSettings(ConfigFile);  
				      
	    port = configSettings.Read("port", 0);  
	    ipAddress = configSettings.Read("ipAddress", ipAddress);  
	    username = configSettings.Read("username", username);  
	    password = configSettings.Read("password", password); 
	  	dbname = configSettings.Read("dbname",dbname);	
	    std::cout<<"port:"<<port<<std::endl;  
	    std::cout<<"ipAddress:"<<ipAddress<<std::endl;  
	    std::cout<<"username:"<<username<<std::endl;  
	    std::cout<<"password:"<<password<<std::endl;  
		std::cout<<"dbname:"<<dbname<<std::endl;											      
	    return 0;  
}  
