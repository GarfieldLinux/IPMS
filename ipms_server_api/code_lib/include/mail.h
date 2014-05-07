//////wrote by Fanxiushu 2007-4-29 
/// MAIL Protocol

#pragma once



#include <string>
#include <list>
#include  <stdlib.h>
#include <string.h>

using namespace std;

struct mail_t
{
	string mail_from;
	list<string> rcpt_to;
	string subject;
	string body;
	list<string> attachments;
	string err_msg;
};

struct mailex_t
{
	string mail_svrip;
	int mail_svrport;
	string user;
	string passwd;
	///
	string mail_from;
	list<string> rcpt_to;
	string subject;
	string body;
	list<string> attachments;
	///
	string err_msg;
};
////
const char*  base64_encode(const unsigned char* text,
						   int size,char* buf,int* out_len);
int base64_decode(const char* text,int size,char* buf);
//SMTP
int smtp_login(const char* str_ip,int port,
			   const char* user,const char* passwd);
void smtp_bye(int fd);
int smtp_sendmail(int fd,struct mail_t* mail);
int smtp_sendmail(struct mailex_t* mail);

////POP3
int pop3_login(const char* str_ip,int port,
			   const char* user,const char* passwd);
void pop3_bye(int fd);
int pop3_getmails(int fd,const char* path,bool del_svrmail);
