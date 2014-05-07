#include "netmask.h"
#include <string>
#include <iostream>
using namespace std;

/*  
struct option longopts[] = {
	{ "version",	0, 0, 'v' },
	{ "help",	0, 0, 'h' },
	{ "debug",	0, 0, 'd' },
	{ "standard",	0, 0, 's' },
	{ "cidr",	0, 0, 'c' },
	{ "cisco",	0, 0, 'i' },
	{ "range",	0, 0, 'r' },
	{ "hex",	0, 0, 'x' },
	{ "octal",	0, 0, 'o' },
	{ "binary",	0, 0, 'b' },
	{ "nodns",	0, 0, 'n' },
	{ "max",	1, 0, 'M' },
	{ "min",	1, 0, 'm' },
	{ NULL,  	0, 0,  0  }
};

*/
  
//char version[] = "netmask, version  garfield version 2011-08-21 will add IPV6 support. "; //此段后原本 有个VERSIOn 的字符变量连接 被garfield 注销 以便 编译
//char vversion[] = __DATE__" "__TIME__;
//char usage[] = "Try `%s --help' for more information.\n";
//char *progname = NULL;
static struct addrmask *aml;
  

int p(void)
{

printf("Running ----- \n");
return 0 ;

}
/*    

int main(int argc, char *argv[]) {
	int optc, h = 0, v = 0, debug = 0, dns = 1, lose = 0;
	//  u_int32_t min = ~0, max = 0;
	output_t output = OUT_CIDR;
	progname = argv[0];//程序名
	initerrors(progname, 0, 0); // stderr, nostatus  程序初始化错误 ?  
	while((optc = getopt_long(argc, argv, "shoxdrvbincM:m:", longopts,(int *) NULL)) != EOF) switch(optc) 
	{
		case 'h': h = 1;   break;
		case 'v': v++;     break;
		case 'n': dns = 0; break;
   //   case 'M': max = mspectou32(optarg); break;
   //   case 'm': min = mspectou32(optarg); break;
		case 'd':
				  initerrors(NULL, -1, 1); // showstatus 
				  debug = 1;
				  break;
		case 's': output = OUT_STD;    break;
		case 'c': output = OUT_CIDR;   break;
		case 'i': output = OUT_CISCO;  break;
		case 'r': output = OUT_RANGE;  break;
		case 'x': output = OUT_HEX;    break;
		case 'o': output = OUT_OCTAL;  break;
		case 'b': output = OUT_BINARY; break;
		default: lose = 1; break;
	}
	if(v) {
		if(v == 1) fprintf(stderr, "%s\n", version);
		else fprintf(stderr, "%s, %s\n", version, vversion);
		if(!h) exit(0);
	}
	if(h) {
		fprintf(stderr,
				"This is netmask, an address netmask generation utility\n"
				"Usage: %s spec [spec ...]\n"
				"  -h, --help\t\t\tPrint a summary of the options\n"
				"  -v, --version\t\t\tPrint the version number\n"
				"  -d, --debug\t\t\tPrint status/progress information\n"
				"  -s, --standard\t\tOutput address/netmask pairs\n"
				"  -c, --cidr\t\t\tOutput CIDR format address lists\n"
				"  -i, --cisco\t\t\tOutput Cisco style address lists\n"
				"  -r, --range\t\t\tOutput ip address ranges\n"
				"  -x, --hex\t\t\tOutput address/netmask pairs in hex\n"
				"  -o, --octal\t\t\tOutput address/netmask pairs in octal\n"
				"  -b, --binary\t\t\tOutput address/netmask pairs in binary\n"
				"  -n, --nodns\t\t\tDisable DNS lookups for addresses\n"
				//      "  -M, --max mask\t\tLimit maximum mask size\n"
				//      "  -m, --min mask\t\tLimit minimum mask size (drop small ranges)\n"
				"Definitions:\n"
				"  a spec can be any of:\n"
				"    address\n"
				"    address:address\n"
				"    address:+address\n"
				"    address/mask\n"
				"  an address can be any of:\n"
				"    N\t\tdecimal number\n"
				"    0N\t\toctal number\n"
				"    0xN\t\thex number\n"
				"    N.N.N.N\tdotted quad\n"
				"    hostname\tdns domain name\n"
				"  a mask is the number of bits set to one from the left\n", progname);
		exit(0);
	}
	if(lose || optind == argc) {
		fprintf(stderr, usage, progname);
		exit(1);
	}
	while(optind < argc) spectoaml(argv[optind++], dns);
	display(output);
	return(0);
}

  */

/**************************************
 * PART I - Read input                *
 **************************************/

static u_int32_t aspectou32(char *, int);
static int       rangetoaml(u_int32_t, u_int32_t);
static int       strtou32(u_int32_t *, char *);
#ifndef HAVE_STRTOUL
//static u_int32_t strtoul(const char *nptr, char **endptr, int base); //此段被garfield 注销 以便编译
#endif

/* spectoaml adds a spec to the aml.
 * deals with:
 * "address"
 * "address:address"
 * "address:+address"
 * "address/mask" */
int spectoaml(char *addrspec, int dns) {

//cout<<"addrspec:"<<addrspec<<"----dns:"<<dns<<endl;
//addrspec="192.168.0.0:192.168.0.255";	
char *sep;
	u_int32_t addr;

	if((sep = strchr(addrspec, ':')) != NULL) {		/* range */
u_int32_t addr2;
		*sep++ = '\0';
		addr = aspectou32(addrspec, dns);
//		cout<<"over"<<endl;
		if(*sep == '+') {
			sep++;
			addr2 = addr;
		} else addr2 = 0;
		addr2 += aspectou32(sep, dns);
		rangetoaml(addr, addr2);
	} else if((sep = strchr(addrspec, '/')) != NULL) {	/* mask */
		u_int32_t mask;

		*sep++ = '\0';
		addr = aspectou32(addrspec, dns);
		mask = mspectou32(sep);
		addtoaml(addr, mask);
	} else {						/* host */
		addr = aspectou32(addrspec, dns);
		addtoaml(addr, ~0);
	}
	return(0);
}
/* aspectou32 should convert the address portion of a spec...
 * "base10"
 * "0base8"
 * "0xbase16"
 * "hostname"
 * "address" */
static u_int32_t aspectou32(char *astr, int dns) {
	u_int32_t addr;
	struct hostent *h;
	struct in_addr s;

	if(strtou32(&addr, astr));
	else if(dns  && ((h = gethostbyname(astr)) != NULL))
		addr = ntohl(*((u_int32_t *)h->h_addr_list[0]));
	else if(inet_aton(astr, &s))
		addr = ntohl(s.s_addr);
	else panic("unable to parse \"%s\"", astr);
	return(addr);
}
/* mspectou32 should convert the mask portion of a spec...
 * "base10"
 * "0base8"
 * "0xbase16"
 * "address" */
static u_int32_t mspectou32(char *mstr) {
	u_int32_t mask = 0, num;
	struct in_addr s;

	if(strtou32(&num, mstr)) mask = num ? ~0 << (32 - num) : num;
	else if(inet_aton(mstr, &s)) {
		mask = ntohl(s.s_addr);
		for(num = ~mask; num & 1; num >>= 1);
		if(num) {
			for(num = mask; num & 1; num >>= 1);
			if(num) panic("invalid mask 0x%08x from \"%s\"", mask, mstr);
			mask = ~mask;
		}
	} else panic("unable to parse \"%s\"", mstr);
	return(mask);
}
/* strtou32 shuld take an int pointer and a string looking like...
 *   "base10"
 *   "0base8"
 *   "0xbase16"
 * and write the value as an unsigned 32 bit int
 * returning true on success and false on failure */
static int strtou32(u_int32_t *num, char *istr) {
	u_int32_t value, base;
	char *str, *endp;

	if(istr == NULL || *istr == '\0') return(0);
	if(istr[0] == '0' && istr[1]) {
		if(istr[1] == 'x' && istr[2]) {
			base = 16;
			str = istr + 2;
		} else {
			base = 8;
			str = istr + 1;
		}
	} else {
		base = 10;
		str = istr;
	}
	value = strtoul(str, &endp, base);
	if(*endp != '\0') return(0);
	*num = value;
	return(1);
}
/* range to aml should take two addresses
 * and add the shortest list of address/mask pairs between them */
static int rangetoaml(u_int32_t low, u_int32_t high) {
u_int32_t i, j, lxh;

	if(low > high) {
		i = low;
		low = high;
		high = i;
	}
	for(lxh = i = low ^ high; i & 1; i >>= 1);
	if(i == 0 && (low | lxh) == high) addtoaml(low, ~(high - low));
	else {
		for(i = lxh, j = 0; i >> 1; j++) i >>= 1;
		i <<= j;
		i = ~(i - 1) & high;
		rangetoaml(low, i - 1);
		rangetoaml(i, high);
	}
	return(0);
}
#ifndef HAVE_STRTOUL
//#warning no ISO 9899 strtoul()? enabling sub-optimal workaround.
/*
   static u_int32_t strtoul(const char *nptr, char **endptr, int base) {
   char *fmt;
   u_int32_t val;

   switch(base) {
   case 8:  fmt = "%lo"; break;
   case 10: fmt = "%lu"; break;
   case 16: fmt = "%lx"; break;
   }
   if(sscanf(nptr, fmt, &val) > 0) *endptr = "";
   else *endptr = nptr;
   return(val);
   }
   *///以上被garfield 注销 以便编译  能否产生BUG 未知
#endif

/**************************************
 * PART II - List management          *
 **************************************/

static int optimize(void);

/* addtoaml takes an address and mask
 * and adds it to the list
 * note: it's a little bigger than it needs to be,
 * but it's very modest about memory use
 * and efficient enough considering it's use in optimize() */
int addtoaml(u_int32_t addr, u_int32_t mask) {
	int neta = addr & mask;
	char first = 0;
	struct addrmask *cur = aml, *lst = NULL,
					*src = NULL, *dst = NULL, *old = NULL;

	if(aml == NULL || aml->mask > mask ||
			(aml->mask == mask && aml->neta > neta)) first = 1;
	else {
		for(cur = aml; cur && (cur->mask < mask ||
					(cur->mask == mask && cur->neta <= neta)); lst = cur, cur = cur->next)
			if(cur->neta == (neta & cur->mask)) {
				status("skip %08x/%08x (%08x/%08x exists)",
						neta, mask, cur->neta, cur->mask);
				return(0);
			}
		dst = lst;
	}
	while(cur) {
		if(neta == (cur->neta & mask)) {
			status("pull %08x/%08x (%08x/%08x added)",
					cur->neta, cur->mask, neta, mask);
			old = cur;
			cur = cur->next;
			if(old->prev) old->prev->next = old->next;
			if(old->next) old->next->prev = old->prev;
			if(aml == old) aml = cur; // suggested by Erik Gavert <erik@jsdata.se>
			if(!old->prev && !old->next) {
				aml = cur = NULL;
				first = 1;
			}
			if(!src) src = old;
			else free(old);
		} else cur = cur->next;
	}
	if(!src && (src = (struct addrmask *)malloc(sizeof(struct addrmask)))
			== NULL) panic("malloc failure");
	src->neta = neta;
	src->mask = mask;
	if(first) {
		status("add first                   {%08x/%08x}%08x/%08x",
				neta, mask, aml?aml->neta:0, aml?aml->mask:0);
		src->prev = NULL;
		src->next = aml;
		if(aml) aml->prev = src;
		aml = src;
	} else if(dst) {
		status("add middle %08x/%08x{%08x/%08x}%08x/%08x",
				dst->neta, dst->mask,
				neta, mask,
				dst->next?dst->next->neta:0, dst->next?dst->next->mask:0);
		src->prev = dst;
		src->next = dst->next;
		dst->next = src;
		if(src->next) src->next->prev = src;
	} else {
		status("add last   %08x/%08x{%08x/%08x}",
				lst->neta, lst->mask, neta, mask);
		src->prev = lst;
		src->next = NULL;
		lst->next = src;
	}
	while(optimize());
	return(0);
}

/* optimize - joins two AMs that can be expressed by a larger mask. */
static int optimize(void) {
	struct addrmask *cur;

	for(cur = aml; cur->next; cur = cur->next)
		if(cur->mask == cur->next->mask &&
				cur->neta == (cur->next->neta & (cur->mask << 1))) {
			addtoaml(cur->neta, cur->mask << 1);
			return(1);
		}
	return(0);
}

/**************************************
 * PART III - Output formatting       *
 **************************************/


/* display - shows the aml in a format specified by style
 * it destroys the list as it sorts */


string  displayduan(output_t style) {
//cout<<"进入函数内部:"<<style<<endl;
	struct addrmask *am, *list;
	string (*disp)(struct addrmask *) = NULL;
	switch(style) {
		case OUT_CIDR:   disp = &dispcidrduan;  break;
		default: panic("memfrob() apparently called on code segment");
	}
string s="";
	while(aml) {
		am = NULL;
		for(list = aml; list; list = list->next)
			if(!am || am->neta > list->neta) am = list;
		if(am->next) am->next->prev = am->prev;
		if(am->prev) am->prev->next = am->next;
		else aml = am->next;
		s=s+disp(am);
		free(am);
	}
	//printf("\n");//garfield 修改 以便编译输出结果集后换行  还木有经过仔细的测试 不知道对其他的函数调用结果会产生的什么的影响 
	return s;
}


int display(output_t style) {
	struct addrmask *am, *list;
	int (*disp)(struct addrmask *) = NULL;
	//printf("%d------\n",style);
	switch(style) {
		case OUT_STD:    disp = &dispstd;    break;
		case OUT_CIDR:   disp = &dispcidr;   break;
		case OUT_CISCO:  disp = &dispcisco;  break;
		case OUT_RANGE:  disp = &disprange;  break;
		case OUT_HEX:    disp = &disphex;    break;
		case OUT_OCTAL:  disp = &dispoctal;  break;
		case OUT_BINARY: disp = &dispbinary; break;
		default: panic("memfrob() apparently called on code segment");
	}
	while(aml) {
		am = NULL;
		for(list = aml; list; list = list->next)
			if(!am || am->neta > list->neta) am = list;
		if(am->next) am->next->prev = am->prev;
		if(am->prev) am->prev->next = am->next;
		else aml = am->next;
		disp(am);
		free(am);
	}
	printf("\n");//garfield 修改 以便编译输出结果集后换行  还木有经过仔细的测试 不知道对其他的函数调用结果会产生的什么的影响 
	return(0);
}


string  dispcidrduan(struct addrmask *am)
{
	u_int32_t mask;
	int ctr = 0;
	string s="";
	for(mask = am->mask; mask; mask <<= 1)
	   ctr++;
//	printf("%s/%d;", inet_ntoa((struct in_addr){htonl(am->neta)}), ctr);
        string s1=inet_ntoa((struct in_addr){htonl(am->neta)});
        char cr[256];
	sprintf(cr,"%d",ctr);
	string s2=cr;
        s=s1+"/"+s2+";";
	return s;
}

static int dispcidr(struct addrmask *am) {
	u_int32_t mask;
	int ctr = 0;
	for(mask = am->mask; mask; mask <<= 1) ctr++;
	printf("%s/%d;", inet_ntoa((struct in_addr){htonl(am->neta)}), ctr);
	return(0);
}



static int dispstd(struct addrmask *am) {
	char buf[32];

	sprintf(buf, "%15s/", inet_ntoa((struct in_addr){htonl(am->neta)}));
	sprintf(buf + 16, "%-15s", inet_ntoa((struct in_addr){htonl(am->mask)}));
	puts(buf);
	return(0);
}
static int dispcisco(struct addrmask *am) {
	char buf[32];

	sprintf(buf,      "%15s ", inet_ntoa((struct in_addr){htonl(am->neta)}));
	sprintf(buf + 16, "%-15s", inet_ntoa((struct in_addr){htonl(~am->mask)}));
	puts(buf);
	return(0);
}
static int disprange(struct addrmask *am) {
	char buf[80];
	u_int32_t range = ~am->mask + 1;

	sprintf(buf,      "%15s-", inet_ntoa((struct in_addr){htonl(am->neta)}));
	sprintf(buf + 16, "%-15s (%u)",
			inet_ntoa((struct in_addr){htonl(am->neta + range - 1)}), range);
	puts(buf);
	return(0);
}
static int disphex(struct addrmask *am) {
	printf("0x%08x/0x%08x\n", am->neta, am->mask);
	return(0);
}
static int dispoctal(struct addrmask *am) {
	printf("0%10o/0%10o\n", am->neta, am->mask);
	return(0);
}
static int dispbinary(struct addrmask *am) {
	char abuf[36], mbuf[36];
	int  i, j;

	for(i = 0; i < 32; i++) {
		j = 34 - (int)(i * 9 / 8);  /* array index skips every 9th element */
		abuf[j] = am->neta & 1 ? '1' : '0';
		mbuf[j] = am->mask & 1 ? '1' : '0';
		am->neta >>= 1;
		am->mask >>= 1;
	}
	abuf[8]  = abuf[17] = abuf[26] = mbuf[8] = mbuf[17] = mbuf[26] = ' ';
	abuf[35] = mbuf[35] = '\0';
	printf("%s / %s\n", abuf, mbuf);
	return(0);
}
