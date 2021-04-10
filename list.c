#define _LIST_C

#include "my_getopt.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<time.h>

#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>

static const char *instr_name;
static int recursive_flag;
static int all_flag;
static off_t down_size;
static off_t up_size;
static time_t modify_time;
static int init=1;

//用于记录路径的链表节点
typedef struct inode{
    const char *node;
    struct inode *next;
}list_node_t;

typedef list_node_t *list_t;

//组成待遍历目录项
static list_t list_start;
static list_t list_end;

/*
在POSIX中定义了检查mode类型的宏定义：
S_ISLNK (st_mode)    判断是否为符号连接
S_ISREG (st_mode)    是否为一般文件
S_ISDIR (st_mode)    是否为目录
S_ISCHR (st_mode)    是否为字符装置文件
S_ISBLK (st_mode)    是否是一个块设备
S_ISFIFO (st_mode)   是否为先进先出
S_ISSOCK (st_mode)   是否为socket
*/
static char get_type(mode_t mode){
    if(S_ISREG(mode))
        return '-';
    if(S_ISDIR(mode))
        return 'd';
    if(S_ISCHR(mode))
        return 'c';
    if(S_ISBLK(mode))
        return 'b';
    if(S_ISLNK(mode))
        return 'l';
    if(S_ISSOCK(mode))
        return 's';
    if(S_ISFIFO(mode))
        return 'p';
}

/*
stat数据结构
struct stat {
 dev_t st_dev;  存储该文件的块设备的设备号ID 
 ino_t st_ino;  inode号 
 mode_t st_mode;  访问权限及文件类型 
 nlink_t st_nlink;  link数 
 uid_t st_uid;  文件主ID 
 gid_t st_gid;  组ID 
 dev_t st_rdev;  device ID (if special file) 
 off_t st_size;  文件大小（字节数）
 blksize_t st_blksize;  blocksize for filesystem I/O 
 blkcnt_t st_blocks;  分配的512字节尺寸块个数 
 struct timespec st_atime;  access时间 
 struct timespec st_mtime;  modification时间 
 struct timespec st_ctime;  change时间 
};*/

static void list_node(const char *path,const char *file_name){ //查询一个文件的信息
   
   
    //前缀路径的处理
    size_t name_size=strlen(path);
    size_t entry_size=strlen(file_name);
    char *node_name=malloc(name_size+entry_size+2);

    strcpy(node_name,path);
    node_name[name_size]='/';
    strcpy(&node_name[name_size+1],file_name);
    node_name[name_size+1+entry_size]=0; //nodename:path/file_name\0
    
    //获取stat，若无法读取，抛出无法访问的错误
    struct stat status;
    if(stat(node_name,&status)){
        fprintf(stderr,"%s can't access \"%s\": %s 888\n",instr_name,node_name,strerror(errno));
        return;
    }


    //stat参数获取
    mode_t stat_mode=status.st_mode;
    off_t stat_size=status.st_size;
    time_t stat_mtime=status.st_mtime;

    //格式化权限字符串
    char permission[]={'-','-','-','-','-','-','-','-','-','-','\0'};
    permission[0]=get_type(stat_mode);

/*
stat_mode中对于权限的定义
S_IRUSR(S_IREAD) 00400     文件所有者具可读取权限
S_IWUSR(S_IWRITE)00200     文件所有者具可写入权限
S_IXUSR(S_IEXEC) 00100     文件所有者具可执行权限

S_IRGRP 00040             用户组具可读取权限
S_IWGRP 00020             用户组具可写入权限
S_IXGRP 00010             用户组具可执行权限

S_IROTH 00004             其他用户具可读取权限
S_IWOTH 00002             其他用户具可写入权限
S_IXOTH 00001             其他用户具可执行权限
*/

    if(stat_mode & S_IRUSR){
        permission[1]='r';
    }
    if(stat_mode & S_IWUSR){
        permission[1]='w';
    }
    if(stat_mode & S_IXUSR){
        permission[1]='x';
    }
    if(stat_mode & S_IRGRP){
        permission[1]='r';
    }
    if(stat_mode & S_IWGRP){
        permission[1]='w';
    }
    if(stat_mode & S_IXGRP){
        permission[1]='x';
    }
    if(stat_mode & S_IROTH){
        permission[1]='r';
    }
    if(stat_mode & S_IWOTH){
        permission[1]='w';
    }
    if(stat_mode & S_IXOTH){
        permission[1]='x';
    }

    //筛选符合条件的文件
    int filter_flag=(!modify_time||time(NULL)-stat_mtime<=modify_time)&&(!down_size||stat_size>=down_size)&&(!up_size||stat_size<=up_size);//过滤修改时间与文件大小
 //   if((filter_flag&&!S_ISDIR(stat_mode))||(S_ISDIR(stat_mode)&&recursive_flag)){//过滤目录于不满足参数条件的文件（递归条件下不过滤目录）
    if(filter_flag){
        char stat_time_str[64];
        strftime(stat_time_str,64,"%Y-%m-%d %H:%M",localtime(&stat_mtime));
        if(recursive_flag)
        	printf("%s %8ld %s %s\n",permission,stat_size,stat_time_str,node_name);
        else
        	printf("%s %8ld %s %s\n",permission,stat_size,stat_time_str,file_name);
    }

    //判断文件不为.或..
    int dir_flag=0;
    size_t file_name_len=strlen(file_name);
    if((file_name[0]=='.'&&file_name_len==1)||(file_name[0]=='.'&&file_name[1]=='.'&&file_name_len==2)){//目标目录为. ..
        dir_flag=1;
    }


    //如果为下一级递归目录，将其加入当前路径
    if(S_ISDIR(stat_mode)&&recursive_flag &&!dir_flag){
        list_t tmp=malloc(sizeof(list_node_t));
        tmp->node=node_name;
        tmp->next=NULL;
        list_end->next=tmp;
        list_end=tmp;
    }

} 

static void list_dir(const char *name){
    if(!init){
//        printf("\n");
    }
    init=0;
    DIR *dir=opendir(name);
    if(dir==NULL){
        fprintf(stderr,"%s can't access dir \"%s\": %s\n",instr_name,name,strerror(errno));
        return;
    }

//    printf("%s:\n",name);

    struct dirent *entry;
    int count=0;
    while((entry=readdir(dir))!=NULL){
        const char *entry_name=entry->d_name;
        if(!all_flag&&entry_name[0]=='.')//非-a选项，排除.开头文件
            continue;
        list_node(name,entry_name);
        count++;
    }
//    printf("%d files.\n",count);

}

static  void list_main(const char *name){
    struct stat status;
    if(stat(name,&status)){
        fprintf(stderr,"%s can't access dir \"%s\": %s\n",instr_name,name,strerror(errno));
        return;
    }

    if(S_ISDIR(status.st_mode)){//是目录就加入待遍历列表
        list_start=malloc(sizeof(list_node_t));
        list_start->node=name;
        list_start->next=NULL;
        list_end=list_start;
        while(list_start){
            list_dir(list_start->node);
            list_start=list_start->next;
        }
    }
    else{//是文件，直接打印信息
        list_node(".",name);
    }

}

int main(int argc,char **argv){
    instr_name=argv[0];
    int c=0;
    while((c=my_getopt(argc,argv,"ral:h:m:"))!=-1){
        switch(c) {
            case 'r':
                recursive_flag=1;
                break;
            case 'a':
                all_flag=1;
                break;
            case 'l':
                down_size=atoi(optarg);
                break;
            case 'h':
                up_size=atoi(optarg);
                break;
            case 'm':
                modify_time=atoi(optarg)*24*60*60;
                break;
            default:
                break;
        }
    }
    if(optind==argc)//0处理对象
    {
        list_main(".");
    }
    else
    {
        for(int i=optind;i<argc;i++){
        list_main(argv[i]);
        }
    }
    return 0;
}
