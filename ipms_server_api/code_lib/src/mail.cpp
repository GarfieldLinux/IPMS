//////wrote by Fanxiushu 2007-4-29 
/// MAIL Protocol
//

#ifdef WIN32
#pragma warning(disable:4786)
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <io.h>
#include <direct.h>

#undef  mkdir
#define mkdir(a,b) _mkdir(a)

#define strncasecmp strnicmp

#else
#ifdef __sun
#ifndef INADDR_NONE
	#define INADDR_NONE (uint32_t)-1
#endif
#include <sys/filio.h>
#include <sys/sockio.h>
#include <sys/termios.h>
#include <sys/statvfs.h> 
#endif //solaris

#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/poll.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdarg.h>
#include <resolv.h>

typedef int SOCKET;
#define closesocket(s) close(s)

#endif

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "mail.h"


static const int BUF_SIZE = 4096;

//the two functions copy from internet,and modified by Fanxiushu.
const char* base64_encode(const unsigned char* text,int size,char* buf,int *out_len)
{
	char* head_buf = buf;
	static const char *base64_encoding = 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int buflen = 0;
	while(size>0)
	{
		*buf++ = base64_encoding[ (text[0] >> 2 ) & 0x3f];
		if(size>2)
		{
			*buf++ = base64_encoding[((text[0] & 3) << 4) | (text[1] >> 4)];
			*buf++ = base64_encoding[((text[1] & 0xF) << 2) | (text[2] >> 6)];
			*buf++ = base64_encoding[text[2] & 0x3F];
		}
		else
		{
			switch(size)
			{
			case 1:
				*buf++ = base64_encoding[(text[0] & 3) << 4 ];
				*buf++ = '=';
				*buf++ = '=';
				break;
			case 2:
				*buf++ = base64_encoding[((text[0] & 3) << 4) | (text[1] >> 4)];
				*buf++ = base64_encoding[((text[1] & 0x0F) << 2) | (text[2] >> 6)];
				*buf++ = '=';
				break;
			}
		}
		
		text +=3;
		size -=3;
		buflen +=4;
	}
	*buf = 0;
	*out_len = buflen;
//	return buflen;
	return head_buf;
}
////
static inline 
char GetBase64Value(char ch)
{
	if ((ch >= 'A') && (ch <= 'Z'))
		return ch - 'A';
	if ((ch >= 'a') && (ch <= 'z'))
		return ch - 'a' + 26;
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0' + 52;
	switch (ch)
	{
	case '+':
		return 62;
	case '/':
		return 63;
	case '=': /* base64 padding */
		return 0;
	default:
		return 0;
	}
}
int base64_decode(const char* text,int size,char* buf)
{
	if(size%4)
		return -1;
	unsigned char chunk[4];
	int parsenum=0;
	while(size>0)
	{
		chunk[0] = GetBase64Value(text[0]);
		chunk[1] = GetBase64Value(text[1]);
		chunk[2] = GetBase64Value(text[2]);
		chunk[3] = GetBase64Value(text[3]);
		
		*buf++ = (chunk[0] << 2) | (chunk[1] >> 4);
		*buf++ = (chunk[1] << 4) | (chunk[2] >> 2);
		*buf++ = (chunk[2] << 6) | (chunk[3]);
		
		text+=4;
		size-=4;
		parsenum+=3;
	}
	return parsenum;
}

/////////basic comunication functions.
inline static
int block_read(int fd,char* buf,int len,int tmo= 15 )
{
	int curr_len = 0;

#ifdef WIN32
	fd_set rdst;FD_ZERO(&rdst);FD_SET(fd,&rdst);
	struct timeval timeout;timeout.tv_sec = tmo;timeout.tv_usec = 0;
	int status = select(fd + 1,&rdst,NULL,NULL,tmo != 0 ? &timeout : NULL);
#else
	struct pollfd ufds;
	ufds.fd = fd;ufds.events = POLLIN; ufds.revents = 0;
	int ms = tmo != 0 ? tmo*1000:-1;
	int status = poll(&ufds,1,ms);
#endif
	///
	
    if(status < 0)
		return -1;
	else if( status == 0)
		return 0;
	///
	curr_len = recv(fd,buf,len,0);
	if(curr_len <= 0)
		return -1;
	return curr_len;
}
inline static
int read_complete(int fd,char* buf,int len,int tmo =30 )
{
	int sz = 0;
	while(sz < len){
#ifdef WIN32
		fd_set rdst;FD_ZERO(&rdst);FD_SET(fd,&rdst);
		struct timeval timeout;timeout.tv_sec = tmo;timeout.tv_usec = 0;
		int status = select(fd + 1,&rdst,NULL,NULL,tmo != 0 ? &timeout : NULL);
#else
		struct pollfd ufds;
		ufds.fd = fd;ufds.events = POLLIN; ufds.revents = 0;
		int ms = tmo != 0 ? tmo*1000:-1;
		int status = poll(&ufds,1,ms);
#endif
		///
		if(status <= 0)
			return -1;
		int r = recv(fd,buf + sz,len - sz,0);
		if( r <= 0)
			return -1;
		sz += r;
	}
	return sz;
}
inline static
int read_line(int fd,char* buf,int len,int tmo = 30 )
{
	int sz = 0;
	while(sz < len ){
#ifdef WIN32
		fd_set rdst;FD_ZERO(&rdst);FD_SET(fd,&rdst);
		struct timeval timeout;timeout.tv_sec = tmo;timeout.tv_usec = 0;
		int status = select(fd + 1,&rdst,NULL,NULL,tmo != 0 ? &timeout : NULL);
#else
		struct pollfd ufds;
		ufds.fd = fd;ufds.events = POLLIN; ufds.revents = 0;
		int ms = tmo != 0 ? tmo*1000:-1;
		int status = poll(&ufds,1,ms);
#endif
		////
		if(status <= 0)
			return -1;
		int r = recv(fd,buf + sz,len -1 - sz,0);
		if( r <= 0)
			return -1;
		sz += r;
		buf[sz]='\0';
		char* str = strstr(buf,"\r\n");
		if(str){
			*str = '\0';
			return sz;
		}
	}
	return sz;
}
inline static
int write_complete(int fd,char* buf,int len,int timeout = 30)
{
	int sz = 0;
	while(sz < len){
#ifdef WIN32
		fd_set wrst;FD_ZERO(&wrst);FD_SET((SOCKET)fd,&wrst);
		struct timeval tmo;memset(&tmo,0,sizeof(tmo));tmo.tv_sec = timeout;
		int status = select(fd +1,NULL,&wrst,NULL,timeout != 0 ? &tmo: NULL);
#else
		struct pollfd ufds;
		ufds.fd = fd;ufds.events = POLLOUT;ufds.revents = 0;
		int ms = timeout != 0 ? timeout*1000:-1;
		int status = poll(&ufds,1,ms);
#endif

		if(status <= 0)
			return -1;
		int r = send(fd,buf + sz,len - sz,0);
		if (r <= 0)
			return -1;
		sz += r;
	}
	return sz;
}
inline static
int simple_connect(const char* str_ip,int port,int tmo)
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if( sock < 0)
		return -1;
	sockaddr_in addr;memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	unsigned int ip = inet_addr(str_ip);
	if( ip ==  INADDR_NONE){
#ifndef WIN32
		res_init(); //ȷ���ı� /etc/resolv.conf��������Ч
#endif
		hostent* p= gethostbyname(str_ip);
		if(!p){
			closesocket(sock);
			return -1;
		}
		memcpy(&ip , (p->h_addr_list[0]),sizeof(unsigned int));
	}
	memcpy(&addr.sin_addr,&ip,sizeof(unsigned int));
#ifdef WIN32
	fd_set wrst;FD_ZERO(&wrst);FD_SET((SOCKET)sock,&wrst);
	fd_set exst;FD_ZERO(&exst);FD_SET((SOCKET)sock,&exst);
	DWORD arg = 1;
	ioctlsocket(sock,FIONBIO,&arg);
	
	int r = connect(sock,(sockaddr*)&addr,sizeof(addr));
	if( r< 0){
		if( WSAGetLastError() != WSAEWOULDBLOCK){
			closesocket(sock);
			return -1;
		}
		struct timeval timeout;timeout.tv_sec = tmo;timeout.tv_usec = 0;
		int status = select(sock + 1,NULL,&wrst,&exst,&timeout);
		if( status <= 0){
			closesocket(sock);
			return -1;
		}
		if(FD_ISSET(sock,&exst)){
			closesocket(sock);
			return -1;
		}
		
	}
    ///
#else
	
	fcntl(sock,F_SETFL,fcntl(sock,F_GETFL) | O_NONBLOCK);
	unsigned long arg = 1;
	ioctl(sock,FIONBIO,&arg);

	int r = connect(sock,(sockaddr*)&addr,sizeof(addr));
	if( r < 0){
		if( errno != EINPROGRESS){
			closesocket(sock);
			return -1;
		}
		///
		pollfd ufds;ufds.fd = sock;
		ufds.events = POLLIN|POLLOUT; ufds.revents = 0;
		int status = poll(&ufds,1,tmo * 1000 );
		if( status <= 0 ){
			closesocket(sock);
			return -1;
		}
		if( ufds.revents & POLLERR || ufds.revents&POLLHUP){
			closesocket(sock);
			return -1;
		}
	}
	
#endif
	///
	return sock;
}

/////
inline static
int smtp_cmd(int fd,const char* rep_code,const char* format,...)
{
	char buf[BUF_SIZE];
	va_list valist;
	va_start(valist,format);
	vsprintf(buf,format,valist);
	va_end(valist);
	int r = write_complete(fd,buf,strlen(buf), 15 );
	if( r < 0){
		printf("write socket data [%s] error.\n",buf);
		return -1;
	}
	r = read_line(fd,buf,BUF_SIZE,15);
	if( r < 0){
		printf("read socket data error.\n");
		return -1;
	}
	if( strncmp(buf,rep_code,strlen(rep_code)) != 0){
		printf("command not right <%s> [%s]\n",rep_code,buf);
		return -2;
	}
	return 0;
}
int smtp_login(const char* str_ip,int port,
			   const char* user,const char* passwd)
{
	int fd = -1;
	fd = simple_connect(str_ip,port, 30 );
	if( fd < 0){
		printf("connect [%s] error.\n",str_ip);
		return -1;
	}
	char buf[BUF_SIZE];buf[0]='\0';
	int r= read_line(fd,buf,BUF_SIZE,15 );
	if( r < 0 || strncmp(buf,"220",3) != 0){
		closesocket(fd);
		printf("Get SMTP Server[%s] error.\n",buf);
		return -1;
	}
	if( smtp_cmd(fd,"250","EHLO localhost\r\n" ) < 0){ //EHLO ��չsmtp
//		closesocket(fd);
		printf("Send EHLO error.\n");
//		return -1;
	}
	// auth login ,if nessary.
	char tmp_buf[512];int len;
	if( (r = smtp_cmd(fd,"334","AUTH LOGIN\r\n")) == 0 ){
		
		if( smtp_cmd(fd,"334","%s\r\n",base64_encode((const unsigned char*)user,strlen(user),tmp_buf,&len)) < 0){
			closesocket( fd);
			printf("auth login fail.\n");
			return -1;
		}
		if( smtp_cmd(fd,"235","%s\r\n",base64_encode((const unsigned char*)passwd,strlen(passwd),tmp_buf,&len)) < 0){
			closesocket(fd);
			printf("auth login fail.\n");
			return -1;
		}
	}
	if( r== -1){
		closesocket(fd);
		return -1;
	}
	/////
	return fd;
}
void smtp_bye(int fd)
{
	if( fd < 0)
		return;
	///
	smtp_cmd(fd,"221","QUIT\r\n");
	///
	closesocket(fd);
}
////
const static char* boundary="---$#367.babyxiu2007.&##3901--6#";
///
////////////
int smtp_sendmail(int fd,struct mail_t* mail)
{
	//
	if(!mail || mail->mail_from.empty() ||
		mail->rcpt_to.size() == 0)
	{
		mail->err_msg ="invalid paramter.";
		return -1;
	}
	///
	int r ;
	string str; 
	str.reserve(4096);
	str = "MAIL FROM: <"+mail->mail_from+">\r\n";
	r = smtp_cmd(fd,"250",str.c_str());
	if( r < 0){
		mail->err_msg = "send CMD err: "+str;
		return -1;
	}
	string all_to;all_to.reserve(512);
	list<string>::iterator it;
	for( it = mail->rcpt_to.begin(); it != mail->rcpt_to.end(); ++it){
		 str = "RCPT TO: <"+ *it +">\r\n";
		 r = smtp_cmd(fd,"250",str.c_str());
		 if( r < 0){
	//		 return -1;
		 }
		 ///
		 if(all_to.empty())
			 all_to +=*it;
		 else 
			 all_to +=","+*it;
		 /////
	}
	r = smtp_cmd(fd,"354","DATA\r\n");
	if( r < 0){
		mail->err_msg = "send CMD DATA err";
		return -1;
	}
	///
#define SEND_STR(str) r = write_complete(fd,(char*)str.c_str(),str.length(), 30 );\
	                  if( r < 0){printf("write socket data error.\n");return -1;}
	///header
	char date_str[100];time_t seconds=time(0);
	strftime(date_str, 100, 
             "%a, %d %b %y %H:%M:%S +0800", 
             localtime(&seconds)); 
	str  = "From: "+mail->mail_from+"\r\n";
	str += "To: " + all_to +"\r\n";
	str += "Subject: " + mail->subject + "\r\n";
	str += "Date: " ; str+=date_str ;str+= "\r\n";
	str += "X-Mailer: babyxiu2007\r\n";
	str += "X-Priority: 3\r\n";//��ͨ���ȼ���
	str += "MIME-Version: 1.0\r\n";
	str += "Content-type: multipart/mixed; boundary=\"" ;
	str += boundary ; str += "\"\r\n";
	str += "\r\n";
	//
	SEND_STR(str);
	///sub header
	str  = string("--") + boundary + "\r\n";
	str += "Content-Type: text/html; charset=UTF-8\r\n"; 
	str += "Content-Transfer-Encoding: 8bit\r\n";
	str += "\r\n";

	SEND_STR(str);
	////
	SEND_STR(mail->body);

	str = "\r\n\r\n";
	SEND_STR(str);
	///attachments
	for( it = mail->attachments.begin(); it != mail->attachments.end(); ++it){
		const char* name = strrchr(it->c_str(),'/');
		if(!name)name = strrchr(it->c_str(),'\\');
		if(!name) name = it->c_str();else name +=1;
		str  = string("--") + boundary + "\r\n";
		str += "Content-Type: application/octet-stream; name=";
		str += name; str += "\r\n";
		
		str += "Content-Disposition: attachment; filename=";
		str += name; str += "\r\n";

		str += "Content-Transfer-Encoding: base64\r\n";
		str += "\r\n";
		///
		char base64[ 512 ];
		unsigned char file[ 78 ];
		FILE* fp = fopen(it->c_str(),"rb");
		if(!fp)
			continue;
		r = write_complete(fd,(char*)str.c_str(),str.length(),20 );
		if( r < 0){
			fclose(fp);printf("write socket data error\n");
			mail->err_msg = "write socket data error.";
			return -1;
		}
		//
		while( !feof(fp) && !ferror(fp) ){
			r = fread(file,1, 78 ,fp);
			if( r <= 0 )
				break;
			int len;
			base64_encode(file,r,base64,&len);strcpy(base64+len,"\r\n");
			r = write_complete(fd,base64,len +1,15);
			if( r< 0){
				fclose(fp);printf("write socket data error\n");
				mail->err_msg = "write socket data error.";
				return -1;
			}
			///
		}
		fclose(fp);
		///
		str = "\r\n\r\n";
		SEND_STR( str );
	}
	/////end
	str = "\r\n--";
	str += boundary ;
	str +="--\r\n.\r\n";

	r = smtp_cmd(fd,"250",str.c_str());
	if ( r< 0){
		mail->err_msg = "mail end error.";
		return  -1;
	}
	//
	return 0;
}
//////
////
int smtp_sendmail(struct mailex_t* mailex)
{
	int fd =-1;
	if(!mailex)
		return -1;
	fd = smtp_login( mailex->mail_svrip.c_str(),mailex->mail_svrport,
		mailex->user.c_str(),mailex->passwd.c_str());
	if( fd < 0){
		mailex->err_msg = "login smtp error: " + mailex->mail_svrip + " - "+ mailex->user;
		return -1;
	}
	struct mail_t mail;
	mail.attachments = mailex->attachments;
	mail.body = mailex->body;
	mail.mail_from = mailex->mail_from;
	mail.rcpt_to = mailex->rcpt_to;
	mail.subject = mailex->subject;

	int r = smtp_sendmail(fd,&mail);

	smtp_bye( fd);

	mailex->err_msg = mail.err_msg;
	///
	return r;
}
////////POP3 
////////////
int pop3_login(const char* str_ip,int port,
			   const char* user,const char* passwd)
{
	int fd = -1;
	fd = simple_connect(str_ip,port,15);
	if( fd < 0){
		printf("Can't Connect [%s].\n",str_ip);
		return -1;
	}
	char buf[BUF_SIZE];
	int r = read_line( fd,buf,BUF_SIZE, 15);
	if( r < 0 || strncmp(buf,"+OK",3) != 0 )
		goto err;
	sprintf(buf,"USER %s\r\n",user);
	r = write_complete(fd,buf,strlen(buf),15);
	r = read_line(fd,buf,BUF_SIZE,15);
	if( r < 0 || strncmp(buf,"+OK",3) != 0)
		goto err;

	sprintf(buf,"PASS %s\r\n",passwd);
	r = write_complete(fd,buf,strlen(buf),15);
	r = read_line(fd,buf,BUF_SIZE,15);
	if( r < 0 || strncmp(buf,"+OK",3) != 0)
		goto err;

	return fd;
err:
	closesocket( fd );
	return -1;
}
void pop3_bye(int fd)
{
	char buf[1024];
	sprintf(buf,"QUIT \r\n");
	write_complete(fd,buf,strlen(buf),10);
	read_line(fd,buf,1024,15);
	///
	closesocket(fd);
}
static 
int pop3_mail_count(int fd)
{
	char buf[BUF_SIZE];
	sprintf(buf,"LIST \r\n");
	write_complete(fd,buf,strlen(buf),10);
	int off=0;
	do{
		int r = block_read(fd,buf+off, BUF_SIZE-off );
		if( r < 0){
			return 0;
		}
		off += r;
		if( off > BUF_SIZE -2 ){
			buf[off] ='\0';
			break;
		}
		buf[off] ='\0';
		if( strstr(buf,"\r\n.\r\n") )
			break;
	}while(true);
	int num =0;
	const char*p=strstr(buf,"\r\n");
	if(!p)return 0;
	while( p= strstr(p+2,"\r\n") ){
		num ++;
	}
	if( num > 0)--num;
	return num;
}
/////
static bool ilegal_char(char c)
{
	const char* ilg ="\"*?<>:\\/|";
	if(strchr(ilg,c))
		return true;

	return false;
}
static void getSubject(char* buf,char* subject)
{
	char tmp[300];memset(tmp,0,sizeof(tmp));
	///
	const char* p=strstr(buf,"Subject:");
	if(!p){
		strcpy(subject,"Unknow-Title");return;
	}
	const char *sub=p+9;
	int i=0;
	for( i=0;sub[i];++i){
		char c=sub[i];
		if( c =='\r' || c == '\n')
			break;
		tmp[i] = c;
	//	if( ilegal_char( c ) )tmp[i]='#';
	}
	tmp[i] = '\0';
	char* hdr=tmp;
	i = 0;
	strcpy(subject,tmp);
	do{
		char* hh=strstr(hdr,"=?");
		if(!hh){
			strcpy(subject + i,hdr);
			break;
		}
		strncpy(subject + i,hdr,hh-hdr);
		i+=hh-hdr;
		///
		char* tt=strstr(hh+2,"?=");
		if(!tt){
			strcpy(subject+i,hdr);
			break;
		}
		*tt = 0;
		char* pp=strchr(hh+2,'?');
		if(pp){
			pp=strchr(pp+1,'?');
		}
		if(!pp)pp=hh+2;
		else pp+=1;
		char buf[300];
		int n=base64_decode(pp,strlen(pp),buf);
		if( n < 0){
			strcpy(subject+i,pp); i+= strlen(pp);
		}
		else{
			strncpy(subject+i,buf,n); i+=n; 
		}
		///
		hdr = (char*)tt+2;
	}while(true);
	///
	if( i!=0 )
		subject[i] = '\0';
	///
	for(i=0;i<strlen(subject);++i){
		if(ilegal_char(subject[i])) subject[i] = '$';
	}

}
static void getDate(char* buf,char* date)
{
	const char* p=strstr(buf,"Date:");
	if(!p){
		time_t t=time(0);strcpy(date,asctime(localtime(&t)));
		int len=strlen(date);
		if( len>= 0 && date[len-1]=='\n') date[len-1]=0;
		for(int i=0;i<strlen(date);++i)
			if(ilegal_char(date[i])) date[i] = '-';
		////
		return ;
	}
	const char *d = p+6;
	int i=0;
	for( i=0;d[i];++i){
		if(d[i]=='\r' || d[i]=='\n')
			break;
		date[i] = d[i];
		if( ilegal_char( d[i] ) )date[i] = '-';
	}
	date[i] = '\0';
}
static void getFrom(char* buf,char* from)
{
}
static void getBoundary(char* buf,char* boundary)
{
	strcpy(boundary,"--");
	//
	const char* p=strstr(buf,"Content-Type:");
	if(!p){
		return;
	}
	const char dd[]="multipart/";
	if(strncasecmp(p+14,dd,sizeof(dd)-1 ) != 0){
		return;
	}
	const char bound[] ="boundary=";
	const char* bb=strchr(p+14+sizeof(dd),'=');
	if(!bb|| strncasecmp(bb-sizeof(bound)+2,bound,sizeof(bound)-1) != 0 ){
		return ;
	}
	bb += 1;while(*bb && (*bb=='\"'||*bb=='\t'||*bb==' '))bb++;
	int i=0;
	for( i=0;bb[i]&&bb[i]!='\"'&&bb[i]!='\r';++i){
		boundary[2+i]=bb[i];
	}
	boundary[i+2]='\0';
}

static char* recv_mail(int fd,int* psize)
{
	char* buf=NULL;
	int size = 1024*512;
	buf = (char*)malloc(size + 1 );
	int read_sz = 0;
	while(true){
		int r = block_read(fd,buf + read_sz ,size-read_sz,10 );
		if( r < 0){
			free(buf);
			printf("read socket data error.\n");
			return NULL;
		}
		read_sz += r;
		buf[read_sz] = '\0';
		if( strstr(buf,"\r\n.\r\n") )
			break;
		if( read_sz >= size){
			size=size*2;
			char* bb=(char*)realloc(buf,size);
			buf = bb;
		}
		/////
	}
	////
	*psize = read_sz;
	return buf;
}
int pop3_getmails(int fd,const char* path,bool del_svrmail)
{
	int num = pop3_mail_count(fd);
	if( num == 0){
		printf("No Mail.\n");
		return -1;
	}
	char buf[BUF_SIZE + 2 ];
	
	///
	int off=0;
	int l_recvlen = 0;int l_len=0;char* lptr = buf;
	//

	////
	for( int mail_i=1;mail_i<=num;++mail_i){
		string dir = path;dir += "/";
		//
		sprintf(buf,"RETR %d\r\n",mail_i);
		int r = write_complete(fd,buf,strlen(buf),10);
		////recv mail
		printf("receving the [%d] mail - ",mail_i);

		int size; 
		char* buffer = recv_mail(fd,&size);
		if(!buffer){
			return -1;
		}
		///
		char subject[300]; getSubject(buffer,subject);
		char date[200];    getDate(buffer,date);
		char boundary[512]; getBoundary(buffer,boundary);
		////
		printf("Subject: %s\n",subject);
		////
		dir += subject;dir +=" - ";dir += date; dir +=".eml";
		FILE* fp = fopen(dir.c_str(),"wb");
		if(fp){
			fwrite(buffer,1,size,fp);
			fclose(fp);
		}else{
			printf("can't create [%s] for write mail.\n",dir.c_str());
			free(buffer);
			continue;
		}
		
		///
		free(buffer);
		///del mail 
		if(del_svrmail){
			sprintf(buf,"DELE %d\r\n",mail_i);
			int r = write_complete(fd,buf,strlen(buf),10);
			r = read_line(fd,buf,BUF_SIZE,30);
			if( r < 0)
				return -1;
			if(strncmp(buf,"+OK",3) != 0){
				printf("Del the [%d] mail [%s] fail!\n",mail_i,subject);
			}else{
				printf("Del the [%d] mail [%s] OK!\n",mail_i,subject);
			}
		}
	}
	return 0;
}

///

#if 0   //test 

//#define POP3
#define SMTP

int main(int argc,char** argv)
{
	if(argc != 5 ){
		printf("wrong parameter!\n");
//		return 0;
	}

#ifdef WIN32
	WSADATA data;WSAStartup(0x0202,&data);
#endif

#ifdef POP3
	int fd = pop3_login(argv[1],110,argv[2],argv[3]);
	if( fd < 0){
		printf("can't login [%s]\n",argv[1]);
		return 0;
	}
	pop3_getmails(fd,argv[4] ,false );
	pop3_bye(fd);
#endif //POP3

	////
#ifdef SMTP
	mailex_t mail;

	mail.mail_svrip = "mail.gmail.com";
	mail.mail_svrport = 25;
	mail.user = "example"; mail.passwd ;

	mail.mail_from="example@gmail.com";

	mail.rcpt_to.push_back("example@email.com");

	mail.subject = "";
	mail.body ="test mail";

	mail.attachments.push_back("");
	mail.attachments.push_back("");

	int r= smtp_sendmail(&mail);
	if( r < 0){
		printf("send mail fail.\n");
	}
#endif //SMTP
	/////
	///
	return 0;
}

#endif

///
