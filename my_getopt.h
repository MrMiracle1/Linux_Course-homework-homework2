#ifndef _MY_GETOPT_H
#define _MU_GETOPT_H

#ifndef _MY_SRC
#include<getopt.h>
#else

#if defined(__cplusplus)
extern "C"{
#endif

#define no_argument 1
#define required_argument 2
#define optional_argument 3

extern char* optarg; //当前选项参数字符串
extern int optind; //argv的当前索引值

struct option{
    const char *name;
    int has_arg;
    int *flag;
    int val;
}

int my_getopt(int argc,char *argv[],const char* optstring);

#if defined(__cplusplucs)
}
#endif

#define getopt my_getopt

#endif

#endif