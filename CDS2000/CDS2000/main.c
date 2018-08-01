//
//  main.c
//  CDS2000
//
//  Created by shifrank1989 on 2018/7/31.
//  Copyright © 2018 shifrank1989. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "cJSON.h"
#define MAXBUFSIZE 1024
typedef  struct occupy
{
    char name[20];
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
} CPU_OCCUPY ;
typedef struct PACKED
{
    char name[20];
    long total;
    char name2[20];
    long free;
}MEM_OCCUPY;
float  g_cpu_used;
void   cal_occupy(CPU_OCCUPY *, CPU_OCCUPY *);
void   get_occupy(CPU_OCCUPY *);
int    get_cpu_num();
float  get_cpu_occupy();
char  * get_cpu_temperature(char ** cpu_id);
void get_mem_occupy(MEM_OCCUPY * mem);
double get_memory_total();
double get_memory_free();
float  g_cpu_used;
char  * cpu_temperature;


int main(int argc, const char * argv[]) {
    printf("-- It time to test -- \n");
    printf("cpu num: %i\n",get_cpu_num());
    //printf("cpu used: %4.2f \n",get_cpu_occupy());
    char a[20]="'CPU1 Temp'";
    char *cpu_id=a;
    //printf("%s\n",cpu_id);
    printf("cpu1 temp: %s\n",get_cpu_temperature(&cpu_id));
    printf("memory total :%4.2f\n",get_memory_total());
     printf("memory free :%4.2f\n",get_memory_free());
    return 0;
}

//cpu核心数
int get_cpu_num(){
    int cpu_num;
    cpu_num = sysconf(_SC_NPROCESSORS_ONLN);
    //printf("cpu mum:%d\n",cpu_num);
    return cpu_num;
}

//cpu占用比
float get_cpu_occupy(){
    
    CPU_OCCUPY ocpu,ncpu;
    get_occupy(&ocpu);
    sleep(1);
    get_occupy(&ncpu);
    cal_occupy(&ocpu, &ncpu);
    //printf("%4.2f\n",g_cpu_used);
    return g_cpu_used;
}
void  get_occupy (CPU_OCCUPY *o) {
    FILE *fd;
    //int n;
    char buff[MAXBUFSIZE];
    fd = fopen ("/proc/stat", "r"); //这里只读取stat文件的第一行及cpu总信息，如需获取每核cpu的使用情况，请分析stat文件的接下来几行。
    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%s %u %u %u %u", o->name, &o->user, &o->nice,&o->system, &o->idle);
    fclose(fd);
}
void  cal_occupy (CPU_OCCUPY *o, CPU_OCCUPY *n){
    double od, nd;
    double id, sd;
    double scale;
    od = (double) (o->user + o->nice + o->system +o->idle);//第一次(用户+优先级+系统+空闲)的时间再赋给od
    nd = (double) (n->user + n->nice + n->system +n->idle);//第二次(用户+优先级+系统+空闲)的时间再赋给od
    scale = 100.0 / (float)(nd-od);       //100除强制转换(nd-od)之差为float类型再赋给scale这个变量
    id = (double) (n->user - o->user);    //用户第一次和第二次的时间之差再赋给id
    sd = (double) (n->system - o->system);//系统第一次和第二次的时间之差再赋给sd
    g_cpu_used = ((sd+id)*100.0)/(nd-od); //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
}

//cpu温度
char * get_cpu_temperature(char ** cpu_id){
    FILE * cpu_temp;
    char buff[MAXBUFSIZE];
    char a[20];
    char b[20];
    char c[20];
    char d[20];
    //printf("%s\n",*cpu_id);
    char cpu_temp_cmd[50]="sudo ipmitool sensor get ";
    strcat (cpu_temp_cmd,*cpu_id);
    //printf("%s\n",cpu_temp_cmd);
    cpu_temp = popen(cpu_temp_cmd,"r");
    int i=0;
    while(i++<30){
        if(fgets (buff, sizeof(buff), cpu_temp)!=NULL){
            sscanf (buff, "%s %s %s %s",a,b,c,d);
            if(strcmp(b, "Reading")==0){
                //printf("cpu :%s \n",d);
                 //cpu_temperature=*d;
                //printf("%s\n",cpu_temperature);
                cpu_temperature=d;
                //printf("%s\n",cpu_temperature);
                break;
            }
        }
    }
    fclose(cpu_temp);
    return cpu_temperature;
}

//内存总量
double get_memory_total(){
    MEM_OCCUPY mem;
    get_mem_occupy(&mem);
    double memory_total=mem.total;
    return memory_total;
}
//内存剩余
double get_memory_free(){
    MEM_OCCUPY mem;
    get_mem_occupy(&mem);
    double memory_free=mem.free;
    return memory_free;
}
void get_mem_occupy(MEM_OCCUPY * mem){
    FILE * fd;
    char buff[MAXBUFSIZE];
    fd = fopen("/proc/meminfo","r");
    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%s %ld", mem->name,&mem->total);
    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%s %ld", mem->name2,&mem->free);
    fclose(fd);
}
//网口数量
int get_net_num(){
    int net_num;
    
    return net_num;
}

