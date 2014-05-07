/*
 * =====================================================================================
 *
 *       Filename:  netmask.h
 *
 *    Description:  netmask 头文件
 *
 *        Version:  1.0
 *        Created:  2011年08月22日 14时52分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  garfield (), garfieldlinux@gmail.com
 *        Company:  江苏省南通天联信息科技有限公司
 *
 * =====================================================================================
 */


#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "errors.h"
#include <string>

using namespace  std;

typedef enum {
	OUT_STD, OUT_CIDR, OUT_CISCO, OUT_RANGE, OUT_HEX, OUT_OCTAL, OUT_BINARY
} output_t;

struct addrmask {
	u_int32_t neta;
	u_int32_t mask;
	struct addrmask *next;
	struct addrmask *prev;
};

int p(void);
int spectoaml(char *addrspec, int dns);
static u_int32_t aspectou32(char *astr, int dns);
static u_int32_t mspectou32(char *mstr);
static int strtou32(u_int32_t *num, char *istr);
static int rangetoaml(u_int32_t low, u_int32_t high);
static int optimize(void);
int addtoaml(u_int32_t addr, u_int32_t mask);
int display(output_t style);
string displayduan(output_t style);
static int dispcidr(struct addrmask *);
string dispcidrduan(struct addrmask *);
static int dispstd(struct addrmask *);
static int dispcisco(struct addrmask *);
static int disprange(struct addrmask *);
static int disphex(struct addrmask *);
static int dispoctal(struct addrmask *);
static int dispbinary(struct addrmask *);

