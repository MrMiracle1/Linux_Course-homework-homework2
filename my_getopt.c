#include<stddef.h>
#include<stdio.h>
#include<string.h>
 
#define _MY_SRC

#include "my_getopt.h"

char *optarg;
int optind=1;

static char *optcursor;

int my_getopt(int argc,char * const argv[],const char * optstring){

    //返回用参数
    int optchar=EOF;

    const char *optdecl=NULL;

    optarg=NULL;

    //遍历了全部参数
    if(optind>=argc)
        goto no_more_argument;
    //当前位置无参数
    if(argv[optind]==NULL)
        goto no_more_argument;
    //当前参数为第一位不为‘-’
    if(argv[optind][0]!='-')
        goto no_more_argument;
    //当前参数只为‘-’
    if(strcmp(argv[optind],"-")==0)
        goto no_more_argument;
    //当前参数为‘--’
    if(strcmp(argv[optind],"--")==0){
        goto no_more_argument;
    }

    if(optcursor==NULL||optcursor[0]=='\0')
        optcursor=argv[optind]+1;

    optchar=optcursor[0];

    optdecl=strchr(optstring,optchar);//选项集中定位
    if(optdecl){
        if(optdecl[1]==':'){//判断参数需带值
            optarg=++optcursor;//传参到optarg
            if(*optarg=='\0'){//参数为空
                if (++optind<=argc){//参数选项之间有空格
                    optarg=argv[optind];
                }
                else{//解析到一个必选值参数不带值，返回？/：
                    optarg=NULL;
                    optchar=(optstring[0]==':')?':':'?';
                }
                
            }
            optcursor=NULL;
        }
    }
    else{//解析到一个不在optstring里面的参数
        optarg=NULL;
        optchar=(optstring[0]==':')?':':'?';
    }

    if(optcursor==NULL||*++optcursor=='\0')//进入下一个选项
        ++optind;
    return optchar;
no_more_argument:
    optcursor=NULL;
    return EOF;
}
