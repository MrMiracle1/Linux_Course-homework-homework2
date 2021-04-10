#ifndef _MY_GETOPT_H
#define _MU_GETOPT_H

#ifndef _MY_SRC
#include<getopt.h>
#else

#if defined(__cplusplus)
extern "C"{
#endif

extern char* optarg; //当前选项参数字符串
extern int optind; //argv的当前索引值

int my_getopt(int argc,char * const argv[],const char* optstring);

#if defined(__cplusplucs)
}
#endif

#define getopt my_getopt

#endif

#endif