#include<stddef.h>
#include<stdio.h>
#include<string.h>
 
#define _MY_SRC

#include "my_getopt.h"

char *optarg;
int optind=1;
//选项起始地址
static char *optbase;

int my_getopt(int argc,char * const argv[],const char * optstring){

    //返回选项初始化 
    int optchar=EOF;
    //用于判断选项类型  
    // optstring中的格式规范如下：
    // 1) 单个字符，表示选项，
    // 2) 单个字符后接一个冒号”:”，表示该选项后必须跟一个参数值。参数紧跟在选项后或者以空格隔开。
    // 3) 单个字符后跟两个冒号”::”，表示该选项后必须跟一个参数。参数必须紧跟在选项后不能以空格隔开。
    const char *opttype=NULL;
    //返回参数初始化
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
        optind++;
        goto no_more_argument;
    }
    //定位目前选项
    if(optbase==NULL||optbase[0]=='\0')
        optbase=argv[optind]+1;
    //获取选项字符
    optchar=optbase[0];

    opttype=strchr(optstring,optchar);//选项集里定位
    if(opttype){
        if(opttype[1]==':'){//判断参数需带值
            optarg=++optbase;//传参到optarg
            if(*optarg=='\0'){//参数为空
                if (++optind<=argc){//参数选项之间有空格
                    optarg=argv[optind];
                }
                else{//解析到一个必选值参数不带值，返回？/：
                    optarg=NULL;
                    optchar=(optstring[0]==':')?':':'?';
                }
                
            }
            optbase=NULL;
        }
    }
    else{//解析到一个不在optstring里面的参数
        optarg=NULL;
        optchar=(optstring[0]==':')?':':'?';
    }

    if(optbase==NULL||*++optbase=='\0')//进入下一个选项
        ++optind;
    return optchar;
no_more_argument:
    optbase=NULL;
    return EOF;
}
