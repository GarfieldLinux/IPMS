#include "netmask.h"
#include "ipmstor.h"
#include "errors.h"
#include <mysql/mysql.h>
#include <sys/socket.h>
#include<sys/types.h>
#include<dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include "Config.h"
#include <math.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include "IPMS_sql_so/mysqlgenius.h"
//#include <boost/bind.hpp> 
#include <sys/stat.h> 
#include <pthread.h>
#include <map>

using namespace std;
//using namespace boost; 

#define N 8
//定义一次性写入数据库的buffer大小 
#define WRITE_SQL_SIZE  (100 * 1024)
#define DEFAULT_LINE_SIZE  256
#define MAX_SQL_SIZE    (1 * 1024 * 1024)
#define DUPLICATE_ENTRY 1062

int port; 
string ipaddress;  
string username;  
string password;  
string dbname;
string server_adapter;
string adapter_address;
const char ConfigFile[]= "server.conf" ;	


MYSQL  mysql;   
MYSQL_RES  *mysql_ret;   
MYSQL_ROW  mysql_row;   


int initsql(const string&  mes)
{
   mysql_init(&mysql);   
   if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
   {   
       printf("连接成功! \t");
       mysql_query(&mysql,"SET NAMES utf8");  
   }else{   
	 printf("连接失败\n"); 
         return -1;	
   }    
    return  0 ; 
}

int ExeSQL(const char* chSQL)
{
	mysql_query(&mysql, chSQL) ;
	printf("执行SQL 返回值%d  \n", mysql_errno(&mysql));
	//return mysql_errno(&mysql)  ;
}



string cstr_to_string(char *p_str)
{
	//该函数的功能 将char* 类型的转换为string 类的值
	return p_str;
}

unsigned long iptolong(string ip)
{
//该函数的功能是将IP地址转换为无符号的长整数
	return  htonl(inet_addr(ip.c_str()));
}

string longtostring(unsigned long lon)
{
//将无符号的长整型转换为string类型
	stringstream s;
	string lts="";
	s<<lon;
	s>>lts;
return lts;
}

string longtoip(long iplong)
{
//将IP地址的长整型转换为string类型的表示
   string ipstring="";
   for(int i=0;i<3;i++)
   {
      stringstream s1;
      string ss;
      s1<<((iplong>>(24-i*8))&0x000000ff);
      s1>>ss;
      ss=ss+".";
      ipstring=ipstring+ss;
   }
   stringstream s4;
   s4<<(iplong&0x000000ff);
   string sf;
   s4>>sf;
   ipstring=ipstring+sf;
return ipstring ;
}


IPinfoList read_ipdatautf8();
bool xadd(const IPinfo& ipinfo);
bool update_ipdatautf8();

void LoadConfig(int iSize);//加载程序配置文件


string findbeforeqitayouxu(const char* ips,string prov,string city)
{
       unsigned  long  num_rows;   
	int  ret; 
	string result="";	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
	//printf("连接成功! \t"); 
	string mys="";
	 string sstart=longtostring(iptolong(ips)-1);
	mys="select isp_id from Odie_OK  where end = '"+sstart+"' and isp_id!='qita' and prov_id='"+prov+"'  and city_id='"+city+"' ;";
	//cout<<mys<<endl;
	mysql_query(&mysql,"set names utf8 ;");	
	ret  =  mysql_query(&mysql,mys.c_str());  
	if(!ret)   
	{   
		mysql_ret  =  mysql_store_result(&mysql);   
		if(mysql_ret  !=  NULL)   
		{   
			num_rows  =  mysql_num_rows(mysql_ret);   
			if(num_rows  !=  0)   
			{   
				while(mysql_row  =  mysql_fetch_row(mysql_ret))   
				{  
					result=mysql_row[0] ;
				}   
			}else{
				mysql_free_result(mysql_ret);
			     }	
		}   
		else   
		{
				mysql_free_result(mysql_ret);
		}   
	}   
        }else{   
	   printf("连接失败\n"); 
	   mysql_close(&mysql);	
        }    
	//cout << num_rows << endl;
	mysql_close(&mysql);
	if(num_rows==0)
	{
	   result="qita";
	}
return result ; 
}


string findnextqitayouxu(const char* ipe,string prov,string city)
{
	unsigned  long  num_rows;   
	int  ret; 
	string result="";	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
	   string mys="";
	   string sstart=longtostring(iptolong(ipe)+1);
	   mys="select isp_id from Odie_OK  where start = '"+sstart+"'   and isp_id !='qita'   and prov_id='"+prov+"'  and city_id='"+city+"'   order by start  ;";
	   //cout<<mys<<endl;
	   mysql_query(&mysql,"set names utf8 ;");	
	   ret  =  mysql_query(&mysql,mys.c_str());  
	   if(!ret)   
	   {   
		mysql_ret  =  mysql_store_result(&mysql);   
		if(mysql_ret  !=  NULL)   
		{   
			num_rows  =  mysql_num_rows(mysql_ret);   
			if(num_rows  !=  0)   
			{   
				while(mysql_row  =  mysql_fetch_row(mysql_ret))   
				{  
					result=mysql_row[0] ;
					mysql_free_result(mysql_ret);
				}   
			}else{
				mysql_free_result(mysql_ret);
		             }	
		}   
		else{
		       mysql_free_result(mysql_ret);
		    }   
	}   
        }else{   
	   printf("连接失败\n"); 
	   mysql_close(&mysql);	
        }    
//	cout << num_rows << endl;
        if(num_rows==0)
        {
           result="qita";
        }
	mysql_close(&mysql);
return result ; 
}


string findbefore(const char* ips,string prov,string city)
{
        unsigned  long  num_rows;   
	int  ret; 
	string result="";	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
	string mys="";
	string sstart=longtostring(iptolong(ips)-1);
	mys="select isp_id from Odie_OK  where end = '"+sstart+"' and isp_id!='qita' and prov_id='"+prov+"' ;";
	//cout<<mys<<endl;
	mysql_query(&mysql,"set names utf8 ;");	
	ret  =  mysql_query(&mysql,mys.c_str());  
	if(!ret)   
	{   
		mysql_ret  =  mysql_store_result(&mysql);   
		if(mysql_ret  !=  NULL)   
		{   
			num_rows  =  mysql_num_rows(mysql_ret);   
			if(num_rows  !=  0)   
			{   
				while(mysql_row  =  mysql_fetch_row(mysql_ret))   
				{  
					result=mysql_row[0] ;
				}   
			}else{
				mysql_free_result(mysql_ret);
		             }	
		}else   
		{
                   mysql_free_result(mysql_ret);
		}   
	}   
	}else   
	{   
	   printf("连接失败\n"); 
	   mysql_close(&mysql);	
	}    
	//cout << num_rows << endl;
	mysql_close(&mysql);
	if(num_rows==0)
	{
	   result="qita";
	}
return result ; 
}


string findnext(const char* ipe,string prov,string city)
{
	unsigned  long  num_rows;   
	int  ret; 
	string result="";	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
	string mys="";
	 string sstart=longtostring(iptolong(ipe)+1);
	mys="select isp_id from Odie_OK  where start = '"+sstart+"'   and isp_id !='qita'   and prov_id='"+prov+"'  order by start  ;";
	//cout<<mys<<endl;
	mysql_query(&mysql,"set names utf8 ;");	
	ret  =  mysql_query(&mysql,mys.c_str());  
	if(!ret)   
	{   
	mysql_ret  =  mysql_store_result(&mysql);   
	if(mysql_ret  !=  NULL)   
	{   
		num_rows  =  mysql_num_rows(mysql_ret);   
		if(num_rows  !=  0)   
		{   
			while(mysql_row  =  mysql_fetch_row(mysql_ret))   
			{  
				result=mysql_row[0] ;
				mysql_free_result(mysql_ret);
			}   
		}else{
			mysql_free_result(mysql_ret);
		}	
	}else{
	   mysql_free_result(mysql_ret);
	}   
        }   
        }else{   
	   printf("连接失败\n"); 
	   mysql_close(&mysql);	
        }    
//	cout << num_rows << endl;
        if(num_rows==0)
        {
           result="qita";
        }
	mysql_close(&mysql);
	return result ; 
}



bool upispidqitalianxu(string start,string end,string ispid)
{
	string isppy= ispid;
	int  ret;  
	int num_rows;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
	        string mys="update Odie_OK  set isp_id='"+ispid+"'  where end <= "+end+" and  start >="+start+" and  isp_id='qita'  ;";
                cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		//mysql_query(&mysql,"");	
		ret  =  mysql_query(&mysql,mys.c_str());  
	}else{   
		printf("连接失败\n"); 
		mysql_close(&mysql);	
	}   
	mysql_close(&mysql);
return true ;
}


bool upisp_at_end(string keyword)
{
	int  ret;  
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
	        string mys="update Odie_OK  set isp_id='dianxin'  where location like '%"+keyword+"%' and isp_id = 'qita' and country_id='china' and prov_id!='hkmotw'  ;";
                cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		//mysql_query(&mysql,"");	
		ret  =  mysql_query(&mysql,mys.c_str());  
	}else{   
		printf("连接失败\n"); 
		mysql_close(&mysql);	
	}   
	mysql_close(&mysql);
return true ;
}



bool upispid(string ip,string ispid)
{
	string ipend=ip;
	string isppy= ispid;
	int  ret;  
	int num_rows;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
	        string mys="update Odie_OK  set isp_id='"+ispid+"'  where endip= '"+ip+"' and isp_id='qita'  ;";
                cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		//mysql_query(&mysql,"");	
		ret  =  mysql_query(&mysql,mys.c_str());  
	}   
	else   
	{   
		printf("连接失败\n"); 
		mysql_close(&mysql);	
	}   
	mysql_close(&mysql);
return true ;
}

SSinfoList IpmstorI::feedbackss()
{
//cout<<"查找省市 "<<endl;
	SSinfoList ssil ;
	MYSQL  mysql;   
	MYSQL_RES  *mysql_ret;   
	MYSQL_ROW  mysql_row;   
	int  ret;  
        int num_rows;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
	       //string mys="select dn_prov.prov_name,dn_prov.prov_py,dn_city.dn_city_name,dn_city.dn_city_py from dn_prov INNER JOIN dn_city ON dn_prov.sort = dn_city.parentid;";
		string mys="select viewinfo_sheng.vi_des,viewinfo_sheng.vi_shengpy,viewinfo_shi.vi_shiname,viewinfo_shi.vi_shipy from viewinfo_sheng INNER JOIN viewinfo_shi ON  viewinfo_sheng.vi_areaid = viewinfo_shi.vi_areaid;";
	cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		//mysql_query(&mysql,"");	
		ret  =  mysql_query(&mysql,mys.c_str());  
		if(!ret)   
		{   
			mysql_ret  =  mysql_store_result(&mysql);   
			if(mysql_ret  !=  NULL)   
			{  
			    num_rows  =  mysql_num_rows(mysql_ret);
			   cout<<"num_rows"<<num_rows<<endl;	
				if(num_rows  !=  0)   
				{   
					while(mysql_row  =  mysql_fetch_row(mysql_ret))   
					{   
						SSinfo  ssi;
						ssi.provname=mysql_row[0];
						ssi.provpy=mysql_row[1];
						ssi.cityname=mysql_row[2];
						ssi.citypy=mysql_row[3];
						ssil.push_back(ssi);
					}   
				}   
				mysql_free_result(mysql_ret);   
			}   
		}   
	}   
	else   
	{   
		printf("连接失败\n"); 
		mysql_close(&mysql);	
	}    
//	cout << num_rows << endl;
	mysql_close(&mysql);
return ssil;
}


bool IpmstorI::updateOdie()
{
        string s="";
	string mysc ="";
	int opflag=0;
        mysc="select startip,endip,isp_id,prov_id,city_id  from  Odie_OK where isp_id='qita' and  country_id='china'  and prov_id!='qita'  order by start";
        OdieList odielist;	
	MYSQL  mysqlc;   
	MYSQL_RES  *mysql_retc;   
	MYSQL_ROW  mysql_rowc;   
	unsigned  long  num_rowsc;   
	int  retc; 
	string result="";	
	mysql_init(&mysqlc);
	if(mysql_real_connect(&mysqlc,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
		mysql_query(&mysqlc,"set names utf8 ;");	
		retc  =  mysql_query(&mysqlc,mysc.c_str());  
		if(!retc)   
		{   
			mysql_retc  =  mysql_store_result(&mysqlc);   
			if(mysql_retc  !=  NULL)   
			{   
				num_rowsc  =  mysql_num_rows(mysql_retc);   
				cout<<"num: "<<num_rowsc<<endl;
				if(num_rowsc  !=  0)   
				{
					while(mysql_rowc  =  mysql_fetch_row(mysql_retc))
					{
						Odie odie;
						odie.ispname=mysql_rowc[0];
						odie.location=mysql_rowc[1];
						odie.ispid=mysql_rowc[2];
						odie.provid=mysql_rowc[3];
						odie.cityid=mysql_rowc[4];
						odielist.push_back(odie);
					}}}
					cout<<"查出条数:"<<num_rowsc<<endl;
		}
	}
        mysql_close(&mysqlc);
	int j=0;
	for(int i=0;i<odielist.size();i++)
	{
		//cout<<"isp确认之前"<<odielist[i].ispid<<endl;
	   string   isppys=findbefore(odielist[i].ispname.c_str(),odielist[i].provid,odielist[i].cityid);
	   string   isppye=findnext(odielist[i].location.c_str(),odielist[i].provid,odielist[i].cityid);
	   if(isppye==isppys&&isppye!="qita"&&isppys!="qita"&&isppye!=""&&isppys!="")
	   {
	      cout<<"第："<<j<<"次确认==startip:"<<odielist[i].ispname<<" : "<<isppye<<"---"<<isppys<<endl;
	      j++;
	      upispid(odielist[i].location,isppye);
           }
         }
}


ISPinfoList IpmstorI::feedbackisp()
{
	//cout<<"查找isp "<<endl;
	ISPinfoList ispil ;
	MYSQL  mysql;   
	MYSQL_RES  *mysql_ret;   
	MYSQL_ROW  mysql_row;   
	int  ret;  
	int num_rows;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
		//printf("连接成功! \t");  
		string mys="select vi_ispname,vi_isppy  from viewinfo_isp  ;";
		cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		//mysql_query(&mysql,"");	
		ret  =  mysql_query(&mysql,mys.c_str());  
		if(!ret)   
		{   
			mysql_ret  =  mysql_store_result(&mysql);   
			if(mysql_ret  !=  NULL)   
			{  
				num_rows  =  mysql_num_rows(mysql_ret);
				//cout<<"num_rows"<<num_rows<<endl;	
				if(num_rows  !=  0)   
				{   
					while(mysql_row  =  mysql_fetch_row(mysql_ret))   
					{   
		                                //printf("%s\t%s\t%s\t%s\t\n",mysql_row[0],mysql_row[1],mysql_row[2],mysql_row[3]);  
						ISPinfo  isp;
						isp.ispname=mysql_row[0];
						isp.isppy=mysql_row[1];
						ispil.push_back(isp);

					}   
				}   
			mysql_free_result(mysql_ret);   
			}   
		}   
	}   
	else   
	{   
		printf("连接失败\n"); 
		mysql_close(&mysql);	
	}    
	//	cout << num_rows << endl;
	mysql_close(&mysql);
	return ispil;
}

void map_insert(map < string, int > *mapFile, string index, int stat) 
{ 
    mapFile->insert(map < string, int >::value_type(index, stat)); 
} 


int API_StringToTime(const string &strDateStr,time_t &timeData)
{
    char *pBeginPos = (char*) strDateStr.c_str();
    char *pPos = strstr(pBeginPos,"-");
    if(pPos == NULL)
    {
        return -1;
    }
    int iYear = atoi(pBeginPos);
    int iMonth = atoi(pPos + 1);
 
    pPos = strstr(pPos + 1,"-");
    if(pPos == NULL)
    {
        return -1;
    }
 
    int iDay = atoi(pPos + 1);
 
    struct tm sourcedate;
    bzero((void*)&sourcedate,sizeof(sourcedate));
    sourcedate.tm_mday = iDay;
    sourcedate.tm_mon = iMonth - 1; 
    sourcedate.tm_year = iYear - 1900;
 
    timeData = mktime(&sourcedate);  
 
    return 0;
}


int check_ip (const char *ip_str) 
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ip_str, &(sa.sin_addr));
	if (result == 0) 
	{ 
           return result; 
	} 
	
   return 1;
} 


vector <IPinfo> IpmstorI::init_ip_data(vector <IPinfo> ipmscanshuone)
{
   SSinfoList ssil;
   SSinfoList shengil ;
   SSinfoList haiwai;
   ISPinfoList ispil ;

   ssil = feedbackss();//用它来确定省以及直辖市的py编码 以及国家编码 
   shengil = feedbacks(); //用它来初始化所属为省的下面行政区划为市的py编码
   haiwai = feedbackhaiwai();//用它初始化海外国家和城市的代码
   ispil = feedbackisp();//初始化运营商的PY编码

   for(int i=0 ; i<ipmscanshuone.size();i++)
   {
    //开始确定省市的编码
    for(int j=0; j<shengil.size();j++)
    { 
        string::size_type position;
        //printf("%s-%s-%s-%s \n",ssil[i].provname.c_str(),ssil[i].provpy.c_str(),ssil[i].cityname.c_str(),ssil[i].citypy.c_str());
        position = ipmscanshuone[i].location.find(shengil[j].provname);  
        if(position != ipmscanshuone[i].location.npos)  //如果没找到，返回一个特别的标志c++中用npos表示 
        {  
           //cout << "国家代码： china "  <<  "省拼音编码: " << shengil[j].provpy << endl;
           ipmscanshuone[i].countryid="china";
           ipmscanshuone[i].provid=shengil[j].provpy;
           for(int k=0; k<ssil.size();k++)
           { 
              string::size_type position;
              position = ipmscanshuone[i].location.find(ssil[k].cityname);
              if(position != ipmscanshuone[i].location.npos) 
              {
                  //cout << "市拼音编码: " << ssil[k].citypy << endl;
                  ipmscanshuone[i].cityid = ssil[k].citypy ; 
                  break;
              }
           }
           break; 
        }
    }

//开始确定运营商py代码  
    for(int n=0; n<ispil.size();n++)
    {
      string::size_type position;
      position = ipmscanshuone[i].location.find(ispil[n].ispname);
      if(position != ipmscanshuone[i].location.npos){
        //cout <<"ISP编码: "<<ispil[n].isppy <<endl;
        ipmscanshuone[i].ispid=ispil[n].isppy ;
      }
    }

//初始化只含有"中国" 两个字的还没有初始化国家代码的区域 编码一定UTF8  
        if(ipmscanshuone[i].countryid==""){
           string::size_type position;
           position = ipmscanshuone[i].location.find("中国");  
           if(position != ipmscanshuone[i].location.npos)  //如果没找到，返回一个特别的标志c++中用npos表示 
           {
            ipmscanshuone[i].countryid="china";
           }
		  //初始化国外区域的代码段
            for(int m=0; m<haiwai.size();m++)
            {
                string::size_type position;
                position = ipmscanshuone[i].location.find(haiwai[m].cityname);
                if(position != ipmscanshuone[i].location.npos){
                  ipmscanshuone[i].countryid=haiwai[m].provpy ;
                  ipmscanshuone[i].provid=haiwai[m].citypy;
                }
            }
        }
if (ipmscanshuone[i].countryid =="")
     ipmscanshuone[i].countryid="qita";
if (ipmscanshuone[i].provid =="")
     ipmscanshuone[i].provid="qita";
if (ipmscanshuone[i].cityid =="")
     ipmscanshuone[i].cityid="qita";
if (ipmscanshuone[i].ispid =="")
     ipmscanshuone[i].ispid="qita";
}
return ipmscanshuone ; 
}




//IP 库更新操作
int IpmstorI::filecompare()
{
//开始获取data文件夹下的文件 分为2种情况  只有1个文件和有2个文件 的情况  
//1---》  初始化IP数据库的节奏
//2---》  更新IP数据库的节奏
initsql("init");
        DIR *dp;
        struct dirent *dirp;
        char dirname[]="./data";
        
        //store file names
        vector<string> file_names;

        if((dp=opendir(dirname))==NULL){
                perror("opendir error");
                exit(1);
        }

        while((dirp=readdir(dp))!=NULL){
                if((strcmp(dirp->d_name,".")==0)||(strcmp(dirp->d_name,"..")==0))
                        continue;
                 file_names.push_back(dirp->d_name);
                //printf("%6d:%-19s %5s\n",dirp->d_ino,dirp->d_name,(dirp->d_type==DT_DIR)?("(DIR)"):(""));
        }

        
        for(vector<string>::iterator it = file_names.begin(); it != file_names.end(); it++)
        {
           cout<<*it<<endl;
           //printf("time:%d\n",strtotime(*it));
           time_t timeData = time(0);
           API_StringToTime(*it,timeData);
           printf("shijian:%d \n",timeData);
          
        }
printf("文件名容器长度%d \n",file_names.size());
if(file_names.size()>2)
   return -1 ;
//这里是含有2个文件的处理过程 一个文件的处理过程和2个文件比较之后的处理过程比较像 所以可以合并处理
if(file_names.size()==2)
{
   printf("更新模式 访问数据: %s \n" , file_names[0].c_str());
   printf("更新模式 访问数据: %s \n" , file_names[1].c_str());
    
   string data_old_file = "data/"+file_names[0];
   string data_new_file = "data/"+file_names[1];
  
   time_t timeData0 = time(0);
   time_t timeData1 = time(0);
   if(API_StringToTime(file_names[0],timeData0)>API_StringToTime(file_names[1],timeData1)){
      data_new_file = "data/"+file_names[0];
      data_old_file = "data/"+file_names[1];
      }

   map<string,int> old_file; 
   map<string,int> new_file;
   
   ifstream fold(data_old_file.c_str());  
   string s; 
   
   while( getline(fold,s) )
   { 
       string::size_type position=s.find("IP数据库共有数据");
       if (position != s.npos) 
          continue;

       if(s!=""||position == s.npos)
          map_insert(&old_file, s, 0);   
   } 

   ifstream fnew(data_new_file.c_str());  
   string snew;  
   while( getline(fnew,snew) )
   {    
       string::size_type position=snew.find("IP数据库共有数据");
       if (position != snew.npos) 
          continue;
      
       if(snew!=""||position == snew.npos)
          map_insert(&new_file, snew, 0);   
   }
//开始旧的数据在新的里面查找  若找到旧的文本数据标记为1  新的文本数据标记为1 未找到的旧的文本数据标记为-1(要删除的)  剩下的新的文本的数据仍然为0(要添加的)
  for (map<string,int>::iterator iter = old_file.begin(); iter != old_file.end(); iter++)
  {
      string str = iter->first;
      //cout << "键：" << str.c_str() << "      值：" << iter->second << endl;
      map<string,int>::iterator iter_find = new_file.find(iter->first);
 if (iter_find!= new_file.end()){
    //找到了
        iter->second = 1 ;
        iter_find->second = 1 ;
       //cout << iter_find->first.c_str() << endl;
    }else{
        //没有找到
       //cout <<"没有找到:   "<< iter->first.c_str() << endl;
       iter->second = -1 ;
       
    } 
  }

for (map<string,int>::iterator iter = old_file.begin(); iter != old_file.end(); iter++){
   if(iter->second == -1){
      string str = iter->first;
      //cout << "要删除的 键：" << str.c_str() << "      值：" << iter->second << endl;
  

  const char *d =" ";
  char *p;
char *cstr = new char[str.length() + 1];
strcpy(cstr, str.c_str());
// do stuff
  p = strtok(cstr,d);

  vector <IPinfo> ipmscanshu ;
  int i=0 ;
  IPinfo ii ; 
   while(p)
   {
      if(check_ip(p)==1)
      {
          if(i==0)
             ii.startip=p;
          if(i==1)
             ii.endip=p;
          //printf("%s  是合法的IP地址 \n",p);
          i++ ;
      }else{
          //printf("%s 不是一个IP地址 \n",p);
          ii.location=ii.location+p;
      }
      string str = p  ;
            p=strtok(NULL,d);
   }

ipmscanshu.push_back(ii);

delete [] cstr;
for(int i=0 ; i<ipmscanshu.size();i++)
{
   printf("需要从数据库中删除的处理后的有序的数据串   %s %s %s \n",ipmscanshu[i].startip.c_str(),ipmscanshu[i].endip.c_str(),ipmscanshu[i].location.c_str());
}
  }
}

printf("\n");
for (map<string,int>::iterator iter = new_file.begin(); iter != new_file.end(); iter++){
   if(iter->second == 0){
      string str = iter->first;
      //cout << "要新增的 键：" << str.c_str() << "      值：" << iter->second << endl;
 const char *d =" ";
  char *p;
char *cstr = new char[str.length() + 1];
strcpy(cstr, str.c_str());
// do stuff
  p = strtok(cstr,d);

  vector <IPinfo> ipmscanshu ;
  int i=0 ;
  IPinfo ii ; 
   while(p)
   {
      if(check_ip(p)==1)
      {
          if(i==0)
             ii.startip=p;
          if(i==1)
             ii.endip=p;
          printf("%s  是合法的IP地址 \n",p);
          i++ ;
      }else{
          printf("%s 不是一个IP地址 \n",p);
          ii.location=ii.location+p;
      }
      string str = p  ;
            p=strtok(NULL,d);
   }

ipmscanshu.push_back(ii);

delete [] cstr;

vector <IPinfo> ipmsdata;
//初步组装完成后 开始根据字典表设置 解析出国家 省 市 运营商的 PY 代码 
  ipmsdata=  init_ip_data(ipmscanshu);
//所有拼音编码确认完毕
for(int i=0 ; i<ipmsdata.size();i++)
{
   updatednipdata(ipmsdata[i]);
}

}
 }

}else{//前面已经判断过>= 2 的情况  这里是处理只有一个文件的 初始化整个情况
   printf("只有一个数据文件的情况 初始化整个IP数据库 开始初始化数据\n");
   SSinfoList shengil ;
   shengil = feedbacks(); //用它来初始化所属为省的下面行政区划为市的py编码
   SSinfoList ssil;
   ssil = feedbackss();//用它来确定省以及直辖市的py编码 以及国家编码 


//开始确定国别信息  国别信息确认规则 首先匹配location 字段是否含有中国省的字段 如果含有这些数据的country_id=china  and prov_id=匹配到的省  
vector <IPinfo> ipmscanshuone ; //存储处理结果的结构
vector <IPinfo> ipmsdata;

  string data_first_file = "data/"+file_names[0];  
   map<string,int> first_file;
   
   ifstream fold(data_first_file.c_str());  
   string s; 
   //读取文件 把所有数据存入内存中 
   while( getline(fold,s) )
   {    
       //cout << "Read from file: " << s << endl;
       string::size_type position=s.find("IP数据库共有数据");
       if (position != s.npos) 
          continue;

       if(s!="")
          map_insert(&first_file, s, 0);   
   }

//遍历该结构 拆分信息 此次只处理3段 切割出起始IP 终止IP 以及该IP的地理位置以及运营商信息
for (map<string,int>::iterator iter = first_file.begin(); iter != first_file.end(); iter++)
{
      string str = iter->first;
      //cout << "键：" << str.c_str() << "      值：" << iter->second << endl;
      const char *d =" ";
      char *p;
      char *cstr = new char[str.length() + 1];
      strcpy(cstr, str.c_str());
      p = strtok(cstr,d);
      int i=0 ;
      IPinfo ii ; 
      while(p)
      {
         if(check_ip(p)==1)
         {
             if(i==0)
                ii.startip=p;
             if(i==1)
                ii.endip=p;
             //printf("%s  是合法的IP地址 \n",p);
             i++ ;
         }else{
             //printf("%s 不是一个IP地址 \n",p);
             ii.location=ii.location+p;
         } 
      string str = p  ;
      p=strtok(NULL,d);
     }
     ipmscanshuone.push_back(ii);
delete [] cstr;
}

//初步组装完成后 开始根据字典表设置 解析出国家 省 市 运营商的 PY 代码 
  ipmsdata=  init_ip_data(ipmscanshuone);
//所有拼音编码确认完毕

//开始组装SQL插入语句 采用效率比较高的一次插入多条记录的方式 操作mysql 数据库
char _column[10240]="insert into Odie_OK(startip,start,endip,end,location,country_id,prov_id,city_id,isp_id,opflag)values " ; 
std::string strSQL="";
strSQL.reserve(MAX_SQL_SIZE);
string scol=cstr_to_string(_column);
strSQL += scol;
char szTemp[DEFAULT_LINE_SIZE];
int countall = ipmsdata.size()-1 ; 
for(int d=0 ; d<ipmsdata.size();d++)
{
   if(ipmsdata[d].startip !="" && ipmsdata[d].endip !="")
   {
   memset(szTemp,0,sizeof(szTemp));
   sprintf(szTemp,"('%s',INET_ATON(startip),'%s',INET_ATON(endip),'%s','%s','%s','%s','%s',0)",ipmsdata[d].startip.c_str(),ipmsdata[d].endip.c_str(),ipmsdata[d].location.c_str(),ipmsdata[d].countryid.c_str(),ipmsdata[d].provid.c_str(),ipmsdata[d].cityid.c_str(),ipmsdata[d].ispid.c_str());
   strSQL += szTemp;
   if(strSQL.size() > WRITE_SQL_SIZE || d == countall)
   {
      printf("d=%d total=%d  SQl: %s \n",d,ipmsdata.size(),strSQL.c_str());
      int ret = ExeSQL(strSQL.c_str());
      strSQL = scol;
      continue; 
   }
   strSQL += ",";
   }
}
printf("SQl 组装结束 \n");
mysql_close(&mysql);
//开始初始化 那些IP 有连续 前后ISP相同的 IP 段的ISP 确认
for(int m=0; m<shengil.size();m++)
{ 
   for(int n=0; n<ssil.size();n++)
   { 
      feedbackqitalianxu(shengil[m].provpy,ssil[n].citypy);
   }
}
//开始初始化剩下的 规则：ISP 其他分类中 网吧  小学 中学 学校 学院 大学 研究(所 院) 公司 高中 ===> 电信
 upisp_at_end("网吧");
 upisp_at_end("小学");
 upisp_at_end("中学");
 upisp_at_end("学校");
 upisp_at_end("学院");
 upisp_at_end("大学");
 upisp_at_end("研究");
 upisp_at_end("公司");
 upisp_at_end("高中");
}
//以上是完全更新IP数据库的处理分支
}





SSinfoList IpmstorI::feedbackpushengpy()
{
//cout<<"查找省市 "<<endl;
	SSinfoList ssil ;
	MYSQL  mysql;   
	MYSQL_RES  *mysql_ret;   
	MYSQL_ROW  mysql_row;   
	int  ret;  
	int num_rows;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
        string mys="select distinct(vi_shengpy)  from viewinfo_sheng where vi_areaid>1 and vi_areaid<=50";
//获取全国32个省市的省的py 编码
	cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		//mysql_query(&mysql,"");	
		ret  =  mysql_query(&mysql,mys.c_str());  
		if(!ret)   
		{   
			mysql_ret  =  mysql_store_result(&mysql);   
			if(mysql_ret  !=  NULL)   
			{  
			    num_rows  =  mysql_num_rows(mysql_ret);
			   cout<<"num_rows"<<num_rows<<endl;	
				if(num_rows  !=  0)   
				{   
					while(mysql_row  =  mysql_fetch_row(mysql_ret))   
					{   
						SSinfo  ssi;
						ssi.provpy=mysql_row[0];
						ssil.push_back(ssi);
					}   
				}   
				mysql_free_result(mysql_ret);   
			}   
		}   
	}else{   
	   printf("连接失败\n"); 
	   mysql_close(&mysql);	
	}    
//	cout << num_rows << endl;
	mysql_close(&mysql);
return ssil;
}


SSinfoList IpmstorI::feedbacks()
{
	SSinfoList ssil ;
	MYSQL  mysql;   
	MYSQL_RES  *mysql_ret;   
	MYSQL_ROW  mysql_row;   
	int  ret;  
	int num_rows;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
		//printf("连接成功! \t");  
	//	string mys="select dn_prov.prov_name,dn_prov.prov_py,dn_city.dn_city_name,dn_city.dn_city_py from dn_prov INNER JOIN dn_city ON dn_prov.sort = dn_city.parentid;";
	string mys="select vi_des,vi_shengpy  from viewinfo_sheng ;";
	cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		//mysql_query(&mysql,"");	
		ret  =  mysql_query(&mysql,mys.c_str());  
		if(!ret)   
		{   
			mysql_ret  =  mysql_store_result(&mysql);   
			if(mysql_ret  !=  NULL)   
			{  
			    num_rows  =  mysql_num_rows(mysql_ret);
			   cout<<"num_rows"<<num_rows<<endl;	
				if(num_rows  !=  0)   
				{   
					while(mysql_row  =  mysql_fetch_row(mysql_ret))   
					{   
		//printf("%s\t%s\t%s\t%s\t\n",mysql_row[0],mysql_row[1],mysql_row[2],mysql_row[3]);  
						//在这里组装我们的IPinfoList
						SSinfo  ssi;
						ssi.provname=mysql_row[0];
						ssi.provpy=mysql_row[1];
						ssil.push_back(ssi);
					}   
				}   
				mysql_free_result(mysql_ret);   
			}   
		}   
	}   
	else   
	{   
		printf("连接失败\n"); 
		mysql_close(&mysql);	
	}    
//	cout << num_rows << endl;
	mysql_close(&mysql);
return ssil;
}


SSinfoList IpmstorI::feedbackhaiwai()
{
//cout<<"查找省市 "<<endl;
	SSinfoList ssil ;
	MYSQL  mysql;   
	MYSQL_RES  *mysql_ret;   
	MYSQL_ROW  mysql_row;   
	int  ret;  
        int num_rows;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
		//printf("连接成功! \t");  
	//	string mys="select dn_prov.prov_name,dn_prov.prov_py,dn_city.dn_city_name,dn_city.dn_city_py from dn_prov INNER JOIN dn_city ON dn_prov.sort = dn_city.parentid;";
		string mys="select vi_shiname,vi_shipy from viewinfo_shi  where vi_areaid=45 ";
	cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		//mysql_query(&mysql,"");	
		ret  =  mysql_query(&mysql,mys.c_str());  
		if(!ret)   
		{   
			mysql_ret  =  mysql_store_result(&mysql);   
			if(mysql_ret  !=  NULL)   
			{  
			    num_rows  =  mysql_num_rows(mysql_ret);
			   cout<<"num_rows"<<num_rows<<endl;	
				if(num_rows  !=  0)   
				{   
					while(mysql_row  =  mysql_fetch_row(mysql_ret))   
					{   
						SSinfo  ssi;
						ssi.provname="海外"+cstr_to_string(mysql_row[0]);
						ssi.provpy="haiwai";
						ssi.cityname=mysql_row[0];
						ssi.citypy=mysql_row[1];
						ssil.push_back(ssi);
					}   
				}   
				mysql_free_result(mysql_ret);   
			}   
		}   
	}   
	else   
	{   
		printf("连接失败\n"); 
		mysql_close(&mysql);	
	}    
//	cout << num_rows << endl;
	mysql_close(&mysql);
return ssil;
}


SSIspList IpmstorI::feedbackoldss()
{
//cout<<"查找省市 "<<endl;
	SSIspList ssisplist ;
	MYSQL  mysql;   
	MYSQL_RES  *mysql_ret;   
	MYSQL_ROW  mysql_row;   
	int  ret;  
   int num_rows;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
//string mys="select dn_prov.prov_name,dn_prov.prov_py,dn_city.dn_city_name,dn_city.dn_city_py from dn_prov INNER JOIN dn_city ON dn_prov.sort = dn_city.parentid;";
		string mys="select vi_shengpy,vi_shipy,vi_isppy from  viewinfo_view";
	cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		//mysql_query(&mysql,"");	
		ret  =  mysql_query(&mysql,mys.c_str());  
		if(!ret)   
		{   
			mysql_ret  =  mysql_store_result(&mysql);   
			if(mysql_ret  !=  NULL)   
			{  
			    num_rows  =  mysql_num_rows(mysql_ret);
			   cout<<"num_rows"<<num_rows<<endl;	
				if(num_rows  !=  0)   
				{   
					while(mysql_row  =  mysql_fetch_row(mysql_ret))   
					{   
						SSIsp  ssisp;
						ssisp.shengpy=mysql_row[0];
						ssisp.shipy=mysql_row[1];
						ssisp.isppy=mysql_row[2];
						ssisplist.push_back(ssisp);

					}   
				}   
				mysql_free_result(mysql_ret);   
			}   
		}   
	}   
	else   
	{   
		printf("连接失败\n"); 
		mysql_close(&mysql);	
	}    
//	cout << num_rows << endl;
	mysql_close(&mysql);
return ssisplist;
}





string IpmstorI::feedbackqitalianxu(const string& prov,const string& city )
{
	string s="";
	cout<<"开始处理isp_name 为其它的数据"<<endl;
	string mysc="select start,end  from Odie_OK where country_id='china' and prov_id='"+prov+"'  and  city_id='"+city+"'   and  isp_id='qita'  order by start; ";//自定义SQL 语句执行
	IPhebingList iphebinglist;
	MYSQL  mysqlc;   
	MYSQL_RES  *mysql_retc;   
	MYSQL_ROW  mysql_rowc;   
	unsigned  long  num_rowsc;   
	int  retc; 
	string result="";	
	mysql_init(&mysqlc);
	cout<<"开始连接数据库"<<endl;
	if(mysql_real_connect(&mysqlc,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
		mysql_query(&mysqlc,"set names utf8 ;");	
		retc  =  mysql_query(&mysqlc,mysc.c_str());  
		if(!retc)   
		{   
			mysql_retc  =  mysql_store_result(&mysqlc);   
			if(mysql_retc  !=  NULL)   
			{   
				num_rowsc  =  mysql_num_rows(mysql_retc);   
				cout<<"num: "<<num_rowsc<<endl;
				if(num_rowsc  !=  0)   
				{
					while(mysql_rowc  =  mysql_fetch_row(mysql_retc))
					{
						IPhebing iphebing;
						iphebing.start=mysql_rowc[0];
						iphebing.end=mysql_rowc[1];
						iphebinglist.push_back(iphebing);
					}}}
					if(num_rowsc==0){
						string rs="";
						mysql_close(&mysqlc);
						return rs;
					}else{
						cout<<s;
					}
		}
	}
IPhebingList ipduanin;
IPhebing ipdd;
ipdd=iphebinglist[0];
for(int i=0;i<iphebinglist.size();i++)
{
	//cout<<"start:"<<iphebinglist[i].start<<"----"<<"end:"<<iphebinglist[i].end<<endl;
	if(i==iphebinglist.size()-1)
	{
		cout<<"可以合并:"<<ipdd.start<<"---"<<ipdd.end<<endl;
		ipduanin.push_back(ipdd);
		break;
	}
//以下两个置换的操作是为了去除潜在的段包含重复的数据项
	if(strtoul(iphebinglist[i+1].start.c_str(),NULL,0) <= strtoul(iphebinglist[i].end.c_str(),NULL,0)&&strtoul(iphebinglist[i+1].end.c_str(),NULL,0) >= strtoul(iphebinglist[i].end.c_str(),NULL,0))
	{
		ipdd.end=iphebinglist[i+1].end;
		cout<<"置换1"<<endl;
	}

	if(strtoul(iphebinglist[i+1].start.c_str(),NULL,0) <= strtoul(iphebinglist[i].end.c_str(),NULL,0)&&strtoul(iphebinglist[i+1].end.c_str(),NULL,0) < strtoul(iphebinglist[i].end.c_str(),NULL,0))
	{
		ipdd.end=iphebinglist[i].end;
		cout<<"置换2: "<<iphebinglist[i].start<<"----"<<iphebinglist[i].end<<"---"<<iphebinglist[i+1].end<<endl;
	}
  
//if((strtoul(iphebinglist[i+1].start.c_str(),NULL,0)-strtoul(iphebinglist[i].end.c_str(),NULL,0)==1)&&(iphebinglist[i+1].provid==iphebinglist[i].provid)&&(iphebinglist[i+1].cityid==iphebinglist[i].cityid))
if(strtoul(iphebinglist[i+1].start.c_str(),NULL,0)-strtoul(iphebinglist[i].end.c_str(),NULL,0)==1)
{
		cout<<"可以合并:"<<ipdd.start<<"---"<<ipdd.end<<endl;
		ipdd.end=iphebinglist[i+1].end;
	}else{
		cout<<"可以合并:"<<ipdd.start<<"---"<<ipdd.end<<endl;
		cout<<"找到完整段"<<endl;
		ipduanin.push_back(ipdd);
		if(i+1<iphebinglist.size()){
			ipdd=iphebinglist[i+1];
		}
	}
}
cout<<"合并前共计"<<iphebinglist.size()<<endl;
cout<<"合并后共计"<<ipduanin.size()<<endl;
int j=0;
for(int i=0;i<ipduanin.size();i++)
{
   cout<<"合并后"<<ipduanin[i].start<<"---"<<ipduanin[i].end<<endl;
   const char * s1=ipduanin[i].start.c_str();
   const char * s2=ipduanin[i].end.c_str();
   string ip1 = longtoip(strtoul(s1,NULL,0));
   string ip2=longtoip(strtoul(s2,NULL,0));
   string  isppys=findbeforeqitayouxu(s1,prov,city);
   string  isppye=findnextqitayouxu(s2,prov,city);
   cout<<" 前一个isp:"<<isppys<<"后一个isp:"<<isppye<<endl;
   if(isppye==isppys&&isppye!="qita"&&isppys!="qita"&&isppye!=""&&isppys!="")
   {
      cout<<"第："<<j<<"次确认==startip:"<<ip1<<"---"<<"endip"<<ip2<<": isp_id :"<<isppys<<endl;
      j++;
      upispidqitalianxu(ipduanin[i].start,ipduanin[i].end,isppye);
   }
}
   mysql_close(&mysqlc);
return s+"};};";
}



string IpmstorI::feedbackviewsheng(const string& sheng,const string& isppy  )
{
   string s="";
   string mysc ="";
   string opflag="viewinfo";
   if(opflag=="viewinfo")
   {
      mysc="select start,end  from Odie_OK where prov_id='"+sheng+"' and isp_id='"+isppy+"'   order by start; ";//自定义SQL 语句执行
      s="view "+sheng+"-"+isppy+"    { match-clients {";
   }	
	IPhebingList iphebinglist;
	MYSQL  mysqlc;   
	MYSQL_RES  *mysql_retc;   
	MYSQL_ROW  mysql_rowc;   
	unsigned  long  num_rowsc;   
	int  retc; 
	string result="";	
	mysql_init(&mysqlc);
	if(mysql_real_connect(&mysqlc,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
		mysql_query(&mysqlc,"set names utf8 ;");	
		retc  =  mysql_query(&mysqlc,mysc.c_str());  
		if(!retc)   
		{   
			mysql_retc  =  mysql_store_result(&mysqlc);   
			if(mysql_retc  !=  NULL)   
			{   
				num_rowsc  =  mysql_num_rows(mysql_retc);   
				if(num_rowsc  !=  0)   
				{
					while(mysql_rowc  =  mysql_fetch_row(mysql_retc))
					{
						IPhebing iphebing;
						iphebing.start=mysql_rowc[0];
						iphebing.end=mysql_rowc[1];
						iphebinglist.push_back(iphebing);
					}}}
					if(num_rowsc==0){
						string rs="";
						mysql_close(&mysqlc);
						return rs;
					}else{
						cout<<s;
					}
		}
	}

IPhebingList ipduanin;
IPhebing ipdd;
ipdd.start=iphebinglist[0].start;
ipdd.end=iphebinglist[0].end;
for(int i=0;i<iphebinglist.size();i++)
{
	//cout<<"start:"<<iphebinglist[i].start<<"----"<<"end:"<<iphebinglist[i].end<<endl;
	if(i==iphebinglist.size()-1)
	{
               ipduanin.push_back(ipdd); 
		break;
	}

	if(strtoul(iphebinglist[i+1].start.c_str(),NULL,0) <= strtoul(iphebinglist[i].end.c_str(),NULL,0)&&strtoul(iphebinglist[i+1].end.c_str(),NULL,0) >= strtoul(iphebinglist[i].end.c_str(),NULL,0))
	{
		ipdd.end=iphebinglist[i+1].end;
		cout<<"置换1"<<endl;
	}

	if(strtoul(iphebinglist[i+1].start.c_str(),NULL,0) <= strtoul(iphebinglist[i].end.c_str(),NULL,0)&&strtoul(iphebinglist[i+1].end.c_str(),NULL,0) < strtoul(iphebinglist[i].end.c_str(),NULL,0))
	{
		ipdd.end=iphebinglist[i].end;
		cout<<"置换2: "<<iphebinglist[i].start<<"----"<<iphebinglist[i].end<<"---"<<iphebinglist[i+1].end<<endl;
	}

if((strtoul(iphebinglist[i+1].start.c_str(),NULL,0)-strtoul(iphebinglist[i].end.c_str(),NULL,0)==1))
	{
	//	cout<<"可以合并"<<endl;
		ipdd.end=iphebinglist[i+1].end;
	}else{
	//	cout<<"找到完整段"<<endl;
		ipduanin.push_back(ipdd);
		
		if(i+1<iphebinglist.size()){
			ipdd.start=iphebinglist[i+1].start;
			ipdd.end=iphebinglist[i+1].end;
		}
	}
}
for(int i=0;i<ipduanin.size();i++)
{
	const char * s1=ipduanin[i].start.c_str();
	const char * s2=ipduanin[i].end.c_str();
	string ip1 = longtoip(strtoul(s1,NULL,0));
	string ip2=longtoip(strtoul(s2,NULL,0));
    string ss=ip1+":"+ip2 ;
    string netmaskipduan="";
    output_t output = OUT_CIDR;
    char sw[50];
    strcpy(sw,ss.c_str());
    spectoaml(sw,1);
    netmaskipduan=displayduan(output);
    //在这里计算IP 段的值
    s=s+netmaskipduan;
}
mysql_close(&mysqlc);
return s+"};};";

}


string IpmstorI::feedbackviewarea(const string& area  )
{
    string s="";
    string mysc ="";
    string opflag="viewinfo";
    if(opflag=="viewinfo")
    {
       mysc="select start,end  from Odie_OK where area='"+area+"'      order by start; ";
       //mysc="select  start,end from  Odie_OK  where isp_name like '%歌华宽带%'  order by start; ";//自定义SQL 语句执行
       cout<<"sql:"<<mysc<<endl;
       s="view "+area+" { match-clients {";
    }	
	
IPhebingList iphebinglist;
MYSQL  mysqlc;   
MYSQL_RES  *mysql_retc;   
MYSQL_ROW  mysql_rowc;   
unsigned  long  num_rowsc;   
int  retc; 
string result="";	
mysql_init(&mysqlc);
if(mysql_real_connect(&mysqlc,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
{   
	mysql_query(&mysqlc,"set names utf8 ;");	
	retc  =  mysql_query(&mysqlc,mysc.c_str());  
	if(!retc)   
	{   
		mysql_retc  =  mysql_store_result(&mysqlc);   
		if(mysql_retc  !=  NULL)   
		{   
			num_rowsc  =  mysql_num_rows(mysql_retc);   
			cout<<"num: "<<num_rowsc<<endl;
			if(num_rowsc  !=  0)   
			{
				while(mysql_rowc  =  mysql_fetch_row(mysql_retc))
				{
					IPhebing iphebing;
					iphebing.start=mysql_rowc[0];
					iphebing.end=mysql_rowc[1];
					iphebinglist.push_back(iphebing);
				}}}
				if(num_rowsc==0){
					string rs="";
					mysql_close(&mysqlc);
					return rs;
				}else{
					cout<<s;
				}
	}
}

IPhebingList ipduanin;
IPhebing ipdd;
ipdd.start=iphebinglist[0].start;
ipdd.end=iphebinglist[0].end;
for(int i=0;i<iphebinglist.size();i++)
{
	//cout<<"start:"<<iphebinglist[i].start<<"----"<<"end:"<<iphebinglist[i].end<<endl;
	if(i==iphebinglist.size()-1)
	{
               ipduanin.push_back(ipdd); 
		break;
	}

	if(strtoul(iphebinglist[i+1].start.c_str(),NULL,0) <= strtoul(iphebinglist[i].end.c_str(),NULL,0)&&strtoul(iphebinglist[i+1].end.c_str(),NULL,0) >= strtoul(iphebinglist[i].end.c_str(),NULL,0))
	{
		ipdd.end=iphebinglist[i+1].end;
		cout<<"置换1"<<endl;
	}

	if(strtoul(iphebinglist[i+1].start.c_str(),NULL,0) <= strtoul(iphebinglist[i].end.c_str(),NULL,0)&&strtoul(iphebinglist[i+1].end.c_str(),NULL,0) < strtoul(iphebinglist[i].end.c_str(),NULL,0))
	{
		ipdd.end=iphebinglist[i].end;
cout<<"置换2: "<<iphebinglist[i].start<<"----"<<iphebinglist[i].end<<"---"<<iphebinglist[i+1].end<<endl;
	}

if((strtoul(iphebinglist[i+1].start.c_str(),NULL,0)-strtoul(iphebinglist[i].end.c_str(),NULL,0)==1))
	{
	//	cout<<"可以合并"<<endl;
		ipdd.end=iphebinglist[i+1].end;
	}else{
	//	cout<<"找到完整段"<<endl;
		ipduanin.push_back(ipdd);
		if(i+1<iphebinglist.size()){
			ipdd.start=iphebinglist[i+1].start;
			ipdd.end=iphebinglist[i+1].end;
		}
	}
}

for(int i=0;i<ipduanin.size();i++)
{
	const char * s1=ipduanin[i].start.c_str();
	const char * s2=ipduanin[i].end.c_str();
	string ip1 = longtoip(strtoul(s1,NULL,0));
	string ip2=longtoip(strtoul(s2,NULL,0));
 	string ss=ip1+":"+ip2 ;
        output_t output = OUT_CIDR;
        char sw[50];
        strcpy(sw,ss.c_str());
        spectoaml(sw,1);
        string netmaskipduan=displayduan(output);
        //在这里计算IP 段的值
        s=s+netmaskipduan;
}
 return s+"};};";
}


string IpmstorI::feedbackview(const string& country ,const string& prov,const string& city, const string& isp )
{
string s="";
string mysc ="";
string country_id="";
string prov_id="";
string city_id="";
string isp_id="";
if(country != "no"){
   if(prov == "no" && city == "no" && isp == "no"){
      country_id="country_id='"+country+"' ";
   }else{
      country_id="country_id='"+country+"' and ";
   }
}

if(prov != "no"){
   if(city == "no" && isp == "no"){
      prov_id="prov_id='"+prov+"' " ;
   }else{
       prov_id="prov_id='"+prov+"' and " ;
   }
}

if(city != "no"){
   if(city != "no" && isp == "no"){
      city_id="city_id='"+city+"' " ;  
   }else{
       city_id="city_id='"+city+"' and " ;
   }
}

if(isp != "no")
   isp_id="isp_id='"+isp+"' " ; 
 
string search_sql="select start,end from Odie_OK  where " +country_id+prov_id+city_id+isp_id+"order by start ;"; 
cout<<search_sql<<endl;
	if(prov=="no"&&city=="no"&&isp=="no")
	{//省市ISP信息都没有确认的
		s="view "+prov+" { match-clients {";
	}
	if(prov!="no"&&city=="no"&&isp=="no")
	{
		s="view "+prov+" { match-clients {";
	}
	if(prov!="no"&&city=="no"&&isp!="no")
	{
//下面为全省全部提取 
	//mysc="select start,end from Odie_OK  where  prov_id='"+prov+"'  and isp_id= '"+isp+"' order  by start ;";
//下面为5min 专用
	//mysc="select start,end from Odie_OK  where  prov_id='"+prov+"' and city_id='qita' and isp_id= '"+isp+"'    order  by start ;";
//cout<<mysc<<endl;
		s="view "+prov+"-"+isp+" { match-clients {";
	}
	if(prov!="no"&&city!="no"&&isp!="no")
	{
		s="view "+prov+"-"+city+"-"+isp+" { match-clients {";
	}
	
	IPhebingList iphebinglist;
	MYSQL  mysqlc;   
	MYSQL_RES  *mysql_retc;   
	MYSQL_ROW  mysql_rowc;   
	unsigned  long  num_rowsc;   
	int  retc; 
	string result="";	
	mysql_init(&mysqlc);
	//cout<<"开始连接数据库"<<endl;
	if(mysql_real_connect(&mysqlc,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
		mysql_query(&mysqlc,"set names utf8 ;");	
		retc  =  mysql_query(&mysqlc,search_sql.c_str());  
		if(!retc)   
		{   
			mysql_retc  =  mysql_store_result(&mysqlc);   
			if(mysql_retc  !=  NULL)   
			{   
				num_rowsc  =  mysql_num_rows(mysql_retc);   
				//cout<<"num: "<<num_rowsc<<endl;
				if(num_rowsc  !=  0)   
				{
					while(mysql_rowc  =  mysql_fetch_row(mysql_retc))
					{
						//cout<<mysql_rowc[0]<<"------"<<mysql_rowc[1]<<endl;
						IPhebing iphebing;
						iphebing.start=mysql_rowc[0];
						iphebing.end=mysql_rowc[1];
						iphebinglist.push_back(iphebing);
					}}}
					//cout<<"cunchu"<<endl; 
					//cout<<"查出条数:"<<num_rowsc<<endl;
					if(num_rowsc==0){
						string rs="";
						mysql_close(&mysqlc);
						return rs;
					}else{
						//cout<<s<<endl;//干嘛要输出阿 
					}
		}
	}

IPhebingList ipduanin;
IPhebing ipdd;
ipdd.start=iphebinglist[0].start;
ipdd.end=iphebinglist[0].end;
for(int i=0;i<iphebinglist.size();i++)
{
	//cout<<"start:"<<iphebinglist[i].start<<"----"<<"end:"<<iphebinglist[i].end<<endl;
	if(i==iphebinglist.size()-1)
	{
               ipduanin.push_back(ipdd); 
	       break;
	}
	if(strtoul(iphebinglist[i+1].start.c_str(),NULL,0) <= strtoul(iphebinglist[i].end.c_str(),NULL,0)&&strtoul(iphebinglist[i+1].end.c_str(),NULL,0) >= strtoul(iphebinglist[i].end.c_str(),NULL,0))
	{
		ipdd.end=iphebinglist[i+1].end;
		cout<<"置换1"<<endl;
	}
	if(strtoul(iphebinglist[i+1].start.c_str(),NULL,0) <= strtoul(iphebinglist[i].end.c_str(),NULL,0)&&strtoul(iphebinglist[i+1].end.c_str(),NULL,0) < strtoul(iphebinglist[i].end.c_str(),NULL,0))
	{
		ipdd.end=iphebinglist[i].end;
		cout<<"置换2: "<<iphebinglist[i].start<<"----"<<iphebinglist[i].end<<"---"<<iphebinglist[i+1].end<<endl;
	}
if((strtoul(iphebinglist[i+1].start.c_str(),NULL,0)-strtoul(iphebinglist[i].end.c_str(),NULL,0)==1))
	{
	//	cout<<"可以合并"<<endl;
		ipdd.end=iphebinglist[i+1].end;
	}else{
	//	cout<<"找到完整段"<<endl;
		ipduanin.push_back(ipdd);
		if(i+1<iphebinglist.size()){
			ipdd.start=iphebinglist[i+1].start;
			ipdd.end=iphebinglist[i+1].end;
		}
	}
}
//cout<<"合并后共计"<<ipduanin.size()<<endl;

for(int i=0;i<ipduanin.size();i++)
{
    const char * s1=ipduanin[i].start.c_str();
    const char * s2=ipduanin[i].end.c_str();
    string ip1 = longtoip(strtoul(s1,NULL,0));
    string ip2=longtoip(strtoul(s2,NULL,0));
    string ss=ip1+":"+ip2 ;
    string netmaskipduan="";
    output_t output = OUT_CIDR;
    char sw[50];
    strcpy(sw,ss.c_str());
    spectoaml(sw,1);
    netmaskipduan=displayduan(output);
    //在这里计算IP 段的值
    s=s+netmaskipduan;   
}
mysql_close(&mysqlc);
return s+"};};";
}




bool searchdnipdatach(IPinfo ii,IPinfo it,IPinfo userii)
{
	IPinfo ipl;
	cout<<"sreach  操作范围"<<endl;
	IPinfo  ipi=ii;
	cout<<ii.startip<<endl;
	cout<<ii.endip<<endl;
	bool bret = true ;
	//cout<<"返回："<<getstart(atol(ii.startip))<<endl;
	MYSQL  mysql;   
	MYSQL_RES  *mysql_ret;   
	MYSQL_ROW  mysql_row;   
	int  ret;  
	int num_rows=0;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
	    string mys="select startip,start,endip,end,location,country_id,prov_id,city_id,isp_id,opflag,isp_name  from Odie_OK  where start <= "+ii.start+" and end >= "+it.end;
		cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		ret  =  mysql_query(&mysql,mys.c_str());  
		if(!ret)   
		{   
			mysql_ret  =  mysql_store_result(&mysql);   
			if(mysql_ret  !=  NULL)   
			{  
			    num_rows  =  mysql_num_rows(mysql_ret);
			   cout<<"num_rows"<<num_rows<<endl;	
				if(num_rows  !=  0)   
				{   
					while(mysql_row  =  mysql_fetch_row(mysql_ret))   
					{   
						printf("startip: %s\tstart:%s\t endip:%s\t end:%s\t \n",mysql_row[0],mysql_row[1],mysql_row[2],mysql_row[3]); 
						ipl.startip=mysql_row[0];
						ipl.start=mysql_row[1];
                                                ipl.endip=mysql_row[2];
                                                ipl.end=mysql_row[3];
                                                ipl.location=mysql_row[4] ;
                                                ipl.countryid=mysql_row[5];		 
                                                ipl.provid=mysql_row[6];		 
                                                ipl.cityid=mysql_row[7];		 
                                                ipl.ispid=mysql_row[8];		 
                                                ipl.opflag=mysql_row[9];
						ipl.ipmask=mysql_row[10];
					}   
				}   
				else   
				{
 string mys="select startip,start,endip,end,location,country_id,prov_id,city_id,isp_id,opflag,isp_name  from Odie_OK  where start >= "+ii.start+" and end <= "+it.end;
		cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		//mysql_query(&mysql,"");	
		ret  =  mysql_query(&mysql,mys.c_str());  
		if(!ret)   
		{   
			mysql_ret  =  mysql_store_result(&mysql);   
			if(mysql_ret  !=  NULL)   
			{  
			    num_rows  =  mysql_num_rows(mysql_ret);
			   cout<<"num_rows"<<num_rows<<endl;	
				if(num_rows  !=  0)   
				{   
					while(mysql_row  =  mysql_fetch_row(mysql_ret))   
					{   
						printf("startip: %s\tstart:%s\t endip:%s\t end:%s\t \n",mysql_row[0],mysql_row[1],mysql_row[2],mysql_row[3]); 
						ipl.startip=mysql_row[0];
						ipl.start=mysql_row[1];
                                                ipl.endip=mysql_row[2];
                                                ipl.end=mysql_row[3];
                                                ipl.location=mysql_row[4] ;
                                                ipl.countryid=mysql_row[5];		 
                                                ipl.provid=mysql_row[6];		 
                                                ipl.cityid=mysql_row[7];		 
                                                ipl.ispid=mysql_row[8];		 
                                                ipl.opflag=mysql_row[9];	
						ipl.ipmask=mysql_row[10];
					}   
				}   
			}
			mysql_free_result(mysql_ret);  
	                mysql_close(&mysql);//断开mysql的连接
			}   
		}
            }   
	}   
	mysql_close(&mysql);//断开mysql的连接
}else{   
		printf("连接失败\n"); 
		mysql_close(&mysql);
		bret=false;
		return bret;	
	}    
//以下代码进行事务的操作
	int t=-1;
	const char* query="" ;
	if(!mysql_real_connect(&mysql, ipaddress.c_str(), username.c_str(), password.c_str(), dbname.c_str(),3306,NULL,0))
	{
		printf( "Error connecting to database: %s\n",mysql_error(&mysql));
	}  else{
		printf("Connected mysqlserver \n");
	query="set names utf8"; //设置编码  
	t=mysql_real_query(&mysql,query,(unsigned int)strlen(query));
	if(t)
	{
		printf("编码设置失败\n");
	}
	t=mysql_real_query(&mysql,"SET AUTOCOMMIT =0",(unsigned int)strlen("SET AUTOCOMMIT =0"));
	if(t!=0){
		printf("启用自定义事务失败\n");
		bret=false;
		return bret;
	}else{
		printf("启用自定义事务成功\n");
	}
	t=mysql_real_query(&mysql,"Begin ;",(unsigned int)strlen("Begin ;"));
int deleteno=0;
int insert1no=0;
int insert2no=0;
int insert3no=0;
int insert_result=0;
//////////////////////////////////////////////////////////
cout<<"查出记录："<<num_rows<<endl;
if(num_rows==0)
{
//如果执行到这一步是有问题的 不应该出现这种情况的
/*  
cout<<"直接插入新增的IP段数据,该数据来自客户端的的输入全新的数据"<<endl;
string queryin="insert into Odie_OK(startip,start,endip,end)values('"+ii.startip+"','"+longtostring(iptolong(ii.startip))+"','"+ii.endip+"','"+longtostring(iptolong(ii.endip))+"')";
insert_result=dninsert(&mysql,strdup(queryin.c_str()));
*/

}

///////////////////////////////拆分IP段过程//////////////////////////////////////

if(num_rows==1){
    cout<<"划分查出的一条IP段"<<endl;
    cout<<"该条数据的详细描述:"<<endl;
    cout<<"startip:"<<ipl.startip<<"endip:"<<ipl.endip<<"location:"<<ipl.location<<endl;
    string deletes1="delete  from Odie_OK where start='"+ipl.start+"' and end='"+ipl.end+"' ";
    cout<<"删除段:"<<deletes1<<endl;
    deleteno=dndelete(&mysql,(char*)deletes1.c_str());
    if(ipl.startip==userii.startip){}else{
        string inserts1="insert into Odie_OK(startip,start,endip,end,location,isp_name,country_id,prov_id,city_id,isp_id,opflag)values('"+ipl.startip+"','"+ipl.start+"','"+longtoip(iptolong(userii.startip)-1)+"','"+longtostring(iptolong(userii.startip)-1)+"','"+ipl.location+"','"+ipl.ipmask+"','"+ipl.countryid+"','"+ipl.provid+"','"+ipl.cityid+"','"+ipl.ispid+"','"+ipl.opflag+  "')";
        insert1no=dninsert(&mysql,(char*)inserts1.c_str());
        cout<<"第一段:"<<inserts1<<endl;
    }
    string inserts2="insert into Odie_OK(startip,start,endip,end,location,isp_name,country_id,prov_id,city_id,isp_id,opflag)values('"+userii.startip+"','"+longtostring(iptolong(userii.startip))+"','"+userii.endip+"','"+longtostring(iptolong(userii.endip))+"','"+userii.location+"','"+userii.ipmask+"','"+userii.countryid+"','"+userii.provid+"','"+userii.cityid+"','"+userii.ispid+"','"+userii.opflag+"')";
    insert2no=dninsert(&mysql,(char*)inserts2.c_str());
    cout<<"第二段:"<<inserts2<<endl;
    if(userii.endip==ipl.endip){}else{
        string inserts3="insert into Odie_OK(startip,start,endip,end,location,isp_name,country_id,prov_id,city_id,isp_id,opflag)values('"+longtoip(iptolong(userii.endip)+1)+"','"+longtostring(iptolong(userii.endip)+1)+"','"+ipl.endip+"','"+ipl.end+"','"+ipl.location+"','"+ipl.ipmask+"','"+ipl.countryid+"','"+ipl.provid+"','"+ipl.cityid+"','"+ipl.ispid+"','"+ipl.opflag+  "')";
        insert3no=dninsert(&mysql,(char*)inserts3.c_str());
        cout<<"第三段:"<<inserts3<<endl;
    }
}
////////////////////////////////////////////////////////////////////////////////
if(num_rows>1){
    string deletes1="delete  from Odie_OK where start>='"+ii.start+"' and end<='"+it.end+"'";
    deleteno=dndelete(&mysql,(char*)deletes1.c_str());
    cout<<"删除段:"<<deletes1<<endl;

    cout<<"合并查出的"<<num_rows<<"条Ip段的首尾,中间段若连续则合并，相关location prov_id city_id isp_id 也要相应的更改"<<endl;
    if(ii.startip==userii.startip){}else{
        string inserts1="insert into Odie_OK(startip,start,endip,end,location,isp_name,country_id,prov_id,city_id,isp_id,opflag)values('"+ii.startip+"','"+ii.start+"','"+longtoip(iptolong(userii.startip)-1)+"','"+longtostring(iptolong(userii.startip)-1)+"','"+ii.location+"','"+ii.ipmask+"','"+ii.countryid+"','"+ii.provid+"','"+ii.cityid+"','"+ii.ispid+"','"+ii.opflag+"')";
        insert1no=dninsert(&mysql,(char*)inserts1.c_str());
        cout<<"第si段:"<<inserts1<<endl;
    }

    string inserts2="insert into Odie_OK(startip,start,endip,end,location,isp_name,country_id,prov_id,city_id,isp_id,opflag)values('"+userii.startip+"','"+longtostring(iptolong(userii.startip))+"','"+userii.endip+"','"+longtostring(iptolong(userii.endip))+"','"+userii.location+"','"+userii.ipmask+"','"+userii.countryid+"','"+userii.provid+"','"+userii.cityid+"','"+userii.ispid+"','"+userii.opflag+"')";
    insert2no=dninsert(&mysql,(char*)inserts2.c_str());
    cout<<"第wu段:"<<inserts2<<endl;

    if(userii.endip==it.endip){}else{
        string inserts3="insert into Odie_OK(startip,start,endip,end,location,isp_name,country_id,prov_id,city_id,isp_id,opflag)values('"+longtoip(iptolong(userii.endip)+1)+"','"+longtostring(iptolong(userii.endip)+1)+"','"+it.endip+"','"+it.end+"','"+it.location+"','"+it.ipmask+"','"+it.countryid+"','"+it.provid+"','"+it.cityid+"','"+it.ispid+"','"+it.opflag+ "')";
        insert3no=dninsert(&mysql,(char*)inserts3.c_str());
        cout<<"第liu段:"<<inserts3<<endl;
    }
}
//sleep(100000000);
if(insert_result==0&&deleteno==0&&insert1no==0&&insert2no==0&&insert3no==0){
	printf("事务提交\n");
	t=mysql_real_query(&mysql,"COMMIT;",(unsigned int)strlen("COMMIT;"));
}else{
	printf("事务回滚\n");
	t=mysql_real_query(&mysql,"ROLLBACK;",(unsigned int)strlen("ROLLBACK;"));
	bret=false;
	return bret  ;
}
	mysql_close(&mysql);//断开mysql的连接
}
return bret;
}



IPinfo searchdnipdataend(IPinfo ii)
{
	IPinfo ipl;
	cout<<"sreach end ch_test—————－";
	IPinfo  ipi=ii;
	//cout<<"返回："<<getstart(atol(ii.startip))<<endl;
	MYSQL  mysql;   
	MYSQL_RES  *mysql_ret;   
	MYSQL_ROW  mysql_row;   
	int  ret;  
	int num_rows;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
		string mys="select startip,endip,end,location,isp_id,country_id,prov_id,city_id,opflag,isp_name from Odie_OK  where start<="+ii.end+" and end >= "+ii.end;
		cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		ret  =  mysql_query(&mysql,mys.c_str());  
		if(!ret)   
		{   
			mysql_ret  =  mysql_store_result(&mysql);   
			if(mysql_ret  !=  NULL)   
			{  
			    num_rows  =  mysql_num_rows(mysql_ret);
			   //cout<<"num_rows"<<num_rows<<endl;	
				if(num_rows  !=  0)   
				{   
					while(mysql_row  =  mysql_fetch_row(mysql_ret))   
					{   
			                   printf("%s\t end :%s\t%s\t%s\t%s\t\n",mysql_row[0],mysql_row[1],mysql_row[2],mysql_row[3],mysql_row[4]);  
					   ipl.endip=mysql_row[1];
					   ipl.end=mysql_row[2];
					   ipl.location=mysql_row[3];
					   ipl.countryid=mysql_row[5];
					   ipl.provid=mysql_row[6];
					   ipl.cityid=mysql_row[7];
					   ipl.opflag=mysql_row[8];
					   ipl.ipmask=mysql_row[9];
					   ipl.ispid=mysql_row[4];
					   cout<<"ipl.endip:"<<ipl.endip<<endl;
					}   
				}   
				mysql_free_result(mysql_ret);   
			}   
		}   
	}   
	else{   
	   printf("连接失败\n"); 
	   mysql_close(&mysql);	
	}    
//	cout << num_rows << endl;
	mysql_close(&mysql);
	return ipl;
}


IPinfo searchdnipdatastart(IPinfo ii)
{
	IPinfo ipl;
	cout<<"sreach start ——————－";
    IPinfo  ipi=ii;
	//cout<<"返回："<<getstart(atol(ii.startip))<<endl;
	MYSQL  mysql;   
	MYSQL_RES  *mysql_ret;   
	MYSQL_ROW  mysql_row;   
	int  ret;  
        int num_rows;	
	mysql_init(&mysql);   
	if(mysql_real_connect(&mysql,ipaddress.c_str(),username.c_str(),password.c_str(),dbname.c_str(),0,NULL,0))   
	{   
	string mys="select startip,endip,location,isp_id,start,country_id,prov_id,city_id,opflag,isp_name  from Odie_OK where start<="+ii.start+" and end >= "+ii.start;
		cout<<"sql: "<<mys<<endl;
		mysql_query(&mysql,"set names utf8 ;");
		ret  =  mysql_query(&mysql,mys.c_str());  
		if(!ret)   
		{   
			mysql_ret  =  mysql_store_result(&mysql);   
			if(mysql_ret  !=  NULL)   
			{  
			    num_rows  =  mysql_num_rows(mysql_ret);
			  // cout<<"num_rows"<<num_rows<<endl;	
				if(num_rows  !=  0)   
				{   
		//			printf("一共查询出 %d 条记录 \n",num_rows);   
					while(mysql_row  =  mysql_fetch_row(mysql_ret))   
					{   
			                   printf("start :%s\t%s\t%s\t%s\t\n",mysql_row[0],mysql_row[1],mysql_row[2],mysql_row[3]);  
					   ipl.startip=mysql_row[0];
					   ipl.location=mysql_row[2];
					   ipl.countryid=mysql_row[5];
					   ipl.provid=mysql_row[6];
					   ipl.cityid=mysql_row[7];
					   ipl.opflag=mysql_row[8];
					   ipl.ipmask=mysql_row[9];
					   ipl.ispid=mysql_row[3];
					   ipl.start=mysql_row[4];
					}   
				}   
				mysql_free_result(mysql_ret);   
			}   
		}   
	}   
	else{   
		printf("连接失败\n"); 
		mysql_close(&mysql);	
	}    
//	cout << num_rows << endl;
	mysql_close(&mysql);
	return ipl;
}

bool IpmstorI::updatednipdata(const IPinfo& ipinfo  )
{
	IPinfo ii;
	bool bret=true ;
	ii.start=longtostring( iptolong(ipinfo.startip));
	ii.end=longtostring(iptolong(ipinfo.endip));
	if(iptolong(ipinfo.startip)>iptolong(ipinfo.endip)){
		cout<<"用户输入不合法"<<endl;
		return false;
	}else{
	cout<<"用户起始地址"<<longtoip(iptolong(ipinfo.startip))<<"start "<<ii.start<<endl;
	cout<<"用户终止地址"<<longtoip(iptolong(ipinfo.endip))<<"end "<<ii.end<<endl;
	if(ipinfo.startip!=""){
		ii.startip=ipinfo.startip;
	}
	if(ipinfo.endip!=""){
		ii.endip=ipinfo.endip;
	}
	if(ipinfo.ipmask!=""){
		ii.ipmask=ipinfo.ipmask;
	}
	if(ipinfo.location!=""){
		ii.location=ipinfo.location;
	if(ipinfo.provid!=""){
		ii.provid=ipinfo.provid;
	}
	if(ipinfo.cityid!=""){
		ii.cityid=ipinfo.cityid;
	}
	if(ipinfo.ispid!=""){
		ii.ispid=ipinfo.ispid;
	}
//	if(ipinfo.opflag!="")
//cout<<"ipinfo2 endip : "<<ipinfo2.endip <<endl;

IPinfo ipinfo1 = searchdnipdatastart(ii);
IPinfo ipinfo2 = searchdnipdataend(ii);

IPinfo ipinfoin ;
if((ipinfo1.startip=="")&&(ipinfo2.endip=="")){
   ipinfoin=ii;
}
if(ipinfo1.startip!=""){
   ipinfoin.startip=ipinfo1.startip ;
   ipinfoin.start=ipinfo1.start ;
}
if(ipinfo2.endip!=""){
   ipinfoin.endip=ipinfo2.endip;
   ipinfoin.end=ipinfo2.end;
   cout<<"ipinfoin:  endip k "<<ipinfoin.endip<<endl;
}
cout<<"起始"<<ipinfoin.startip<<endl;
cout<<"终止"<<ipinfoin.endip<<endl;
cout<<"起始"<<ipinfo.start<<endl;
cout<<"终止"<<ipinfo.end<<endl;
bret=searchdnipdatach(ipinfo1,ipinfo2,ipinfo);
   }
   return bret ;
}
}

string IpmstorI::ipmerge()
{
	string result ="调用函数测试成功" ;
	return result;
}



void IpmstorI::LoadConfig()
{
printf("#############################################################################\n");
	printf("开始读取配置文件server.conf信息\n");
	Config configSettings(ConfigFile);  
	port = configSettings.Read("port", 0); 
	ipaddress = configSettings.Read("ipaddress", ipaddress);  
	username = configSettings.Read("username", username);  
	password = configSettings.Read("password", password); 
	dbname = configSettings.Read("dbname",dbname);
	cout<<"mysql数据库端口号port:"<<port<<endl;  
	cout<<"mysql数据库地址ipAddress:"<<ipaddress<<endl;  
	cout<<"mysql数据库用户名username:"<<username<<endl;  
	cout<<"mysql该用户密码password:"<<password<<endl;  
	cout<<"mysql使用的数据库dbname:"<<dbname<<endl;
printf("############################################################################\n");
}

string IpmstorI::ipmssea(const string& start ,const string& end)
{
	MYSQL_RES  *mysql_ret;   
	MYSQL_ROW  mysql_row;   
	unsigned  long  num_rows;   
	int  ret; 
	string result="";	
	string mys="";
	string sstart=longtostring(iptolong(start));
	//mys="select prov_id,city_id,isp_id,area from Odie_OK  where '"+sstart+"' >= start  and '"+sstart+"' <= end;";
mys="select isp_id,prov_id  from  Odie_OK  where "+sstart+" between start  and  end;";
	cout<<mys<<endl;
	mysql_query(&mysql,"set names utf8 ;");	
	ret  =  mysql_query(&mysql,mys.c_str());  
	if(!ret)   
	{   
	   mysql_ret  =  mysql_store_result(&mysql);   
	if(mysql_ret  !=  NULL)   
	{   
		num_rows  =  mysql_num_rows(mysql_ret);   
		if(num_rows  !=  0)   
		{   
			printf("一共查询出 %d 条记录 \n",num_rows);   
			while(mysql_row  =  mysql_fetch_row(mysql_ret))   
			{   
                            result=string(mysql_row[1])+" "+string(mysql_row[0]);
                        }   
		}   
		mysql_free_result(mysql_ret);   
	}   
        } 
	return  result ; 
}

