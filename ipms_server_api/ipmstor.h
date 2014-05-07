#ifndef __ipmstor_h__
#define __ipmstor_h__

#include <vector>
#include <string>
using namespace std ;

struct IPhebing
{
    ::std::string start;
    ::std::string end;
    ::std::string provid;
    ::std::string cityid;
};

struct AREAinfo
{
    ::std::string areapy;
    ::std::string areaname;

};

struct SSIsp
{
    ::std::string shengpy;
    ::std::string shipy;
    ::std::string isppy;

};

struct VIEWinfo
{
    ::std::string viewname;
    ::std::string viewareaid;
    ::std::string viewispid;
    ::std::string viewpci;

};

struct IPpart
{
    ::std::string ipparts;
    ::std::string startip;
    ::std::string start;
    ::std::string end;
    ::std::string endip;

};

struct PRinfo
{
    ::std::string provname;
    ::std::string provpy;

};

struct SSinfo
{
    ::std::string provname;
    ::std::string provpy;
    ::std::string cityname;
    ::std::string citypy;

};

struct ISPinfo
{
    ::std::string ispname;
    ::std::string isppy;

};

struct IPinfo
{
    ::std::string startip;
    ::std::string start;
    ::std::string endip;
    ::std::string end;
    ::std::string ipmask;
    ::std::string location;
    ::std::string countryid;
    ::std::string provid;
    ::std::string cityid;
    ::std::string ispid;
    ::std::string opflag;

};

struct Odie
{
    ::std::string start;
    ::std::string end;
    ::std::string location;
    ::std::string ispname;
    ::std::string countryid;
    ::std::string provid;
    ::std::string cityid;
    ::std::string ispid;

};


typedef vector<SSIsp> SSIspList ;
typedef vector<IPhebing> IPhebingList;
typedef vector<AREAinfo> AREAinfoList;
typedef vector<IPinfo> IPinfoList;
typedef vector<SSinfo> SSinfoList;
typedef vector<ISPinfo> ISPinfoList;
typedef vector<PRinfo> PRinfoList;
typedef vector<IPpart> IPpartList;
typedef vector<Odie> OdieList;

class IpmstorI  {
	public:
		 void printString(const string& s);
		 string ipmssea(const string& start,const string& end);
		 int ipmsdel(const string& start ,const string& end );
		 int ipmsadd(const string& startip,const string& endip,const string& location,const string& isp );
		 IPpart ipsplit(const string& ipinfo);
		 string ipmerge();
		 int ipvalid(const string& ipinfo);
		 string getResult(const string& startip,const string& endip,int& resuetsource);
		  IPinfoList dnipinfoadd(const  IPinfo& ipinfo   );
		 bool x();
		 bool updatednipdata(const IPinfo& ipinfo  );
		 string updatewhois(const string& ip );
		 string feedbackview(const string& country,const string& prov,const string& city, const string& isp);
	         string feedbackviewlargearea(const string& largearea , const string& isppy );
		 string feedbackviewarea(const string& area );
		 string feedbackipinfo(const string& ip );
                 string feedbackviewsheng(const string& sheng,const string& isppy );
                 string feedbackqitalianxu(const string& prov,const string& city );
		 string ipanalysis(const string& ip  );
		 string feedbackareaview(const string& areapy  );
		 SSinfoList feedbackss();
		 SSIspList feedbackoldss();
		 SSinfoList feedbackhaiwai();
		 SSinfoList feedbacks();	
		 SSinfoList feedbackpushengpy();	
		 ISPinfoList feedbackisp();
		 PRinfoList feedbacksp();
		 AREAinfoList feedbackarea();	
		 SSIspList feedbackssisp();
		 string  tosmallcity(const string& ippart );
		 bool updateprovpy(const string& provname,const string& provpy );
		 bool updatecitypy(const string& provname,const string& provpy,const string& cityname,const string& citypy );	
		 bool updatehaiwaipy(const string& provname,const string& provpy,const string& cityname,const string& citypy );	
		 bool updateisppy(const string& ispname,const string& isppy );
		 bool updateOdie();
                 void LoadConfig();
                 int filecompare();
                 vector <IPinfo> init_ip_data(vector <IPinfo> ipmscanshuone);
};



#endif
