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
typedef struct ClusterInfo{
    char physicalCapacity[50];
    char PhysicalUsage[50];
    //int  nodeCount;
}CLUSTER ;

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
char * POST_Authorization();
//char * get_Cluster_Status();
CLUSTER * get_Cluster_Status();
char * Splice_character();
//char *character;
char * get_accessToken();
float  g_cpu_used;
char  * cpu_temperature;
char * Authorization;
char * get_Fan_Status();
char fanstatus[5];
char authorization_cmd[1024]="curl -s -k -X GET 'https://192.168.0.13/irisservices/api/v1/public/cluster?fetchStats=true' -H 'accept: application/json' -H 'Authorization:Bearer ";


int main(int argc, const char * argv[]) {
    printf("-- It time to test -- \n");
    //printf("cpu num: %i\n",get_cpu_num());
    //printf("cpu used: %4.2f \n",get_cpu_occupy());
    //char a[20]="'CPU1 Temp'";
    //char *cpu_id=a;
    //printf("%s\n",cpu_id);
    //printf("cpu1 temp: %s\n",get_cpu_temperature(&cpu_id));
    //printf("memory total :%4.2f\n",get_memory_total());
    //printf("memory free :%4.2f\n",get_memory_free());
    //printf("authorization info : %s\n",POST_Authorization());
    
    //printf("physicalCapacityBytes :%s\n",get_Cluster_Status()->physicalCapacity);
    //printf("PhysicalUsage :%s\n",get_Cluster_Status()->PhysicalUsage);
    //free(get_Cluster_Status());
    CLUSTER *pclu = get_Cluster_Status();
    printf("physicalCapacityBytes :%s\n",pclu->physicalCapacity);
    printf("PhysicalUsage :%s\n",pclu->PhysicalUsage);
    //Splice_character();
    //printf("fanstatus %s \n",get_Fan_Status());
    
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
//集群总容量
CLUSTER * get_Cluster_Status(){
    FILE * cluster_status;
    CLUSTER *pCluinfo = malloc(sizeof(CLUSTER));
    //CLUSTER *buff = malloc(sizeof(buff));
    char buff[2048];
    char a[20];
    //char b[20];
    char c[50];
    char d[50];
    //int e;
    //Splice_character();
    POST_Authorization();
    char * cmd ;
    cmd=authorization_cmd;
    cluster_status = popen(cmd,"r");
    //free(cmd);
    //printf("拼接完命令 ：%s\n",authorization_cmd);
    if(fgets (buff, sizeof(buff), cluster_status)!=NULL){
        //printf("标记：%s\n","mmmmm");
        sscanf (buff, "%s\n",a);
        printf("cluster info: %s\n",a);
        cJSON * root = NULL;
        cJSON * item = NULL;//cjson对象
        cJSON * physicalCapacity=NULL;
        cJSON * totalPhysicalUsageBytes=NULL;
        root = cJSON_Parse(a);
        if (!root)
        {
            printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        }
        else
        {
            //printf("标记：%s\n","jjjjj");
            item = cJSON_GetObjectItem(root, "stats");
            printf("1: %s\n", cJSON_Print(item));
            item = cJSON_GetObjectItem(item, "localUsagePerfStats");
            //printf("2: %s\n", cJSON_Print(item));
            physicalCapacity = cJSON_GetObjectItem(item, "physicalCapacityBytes");
            sprintf(c,"%4.2f",((physicalCapacity->valuedouble)/1024/1024/1024/1024));
            //sprintf(e,"%d",c);
            strcpy(pCluinfo->physicalCapacity, c) ;
            totalPhysicalUsageBytes=cJSON_GetObjectItem(item, "totalPhysicalUsageBytes");
            sprintf(d,"%4.1f",totalPhysicalUsageBytes->valuedouble);
            strcpy(pCluinfo->PhysicalUsage,d) ;
            //printf("3: %s \n",c);
            //sprintf(c, "%g", item->valuedouble);
            //strcpy(*clusterstatus,c);
            //printf("3: %f \n",item->valuedouble);
            //free(buff);
            cJSON_Delete(root);
            //cJSON_Delete(item);
        }
    }
    pclose(cluster_status);
    free(pCluinfo);
    //free(buff);
    return pCluinfo;
}

char * POST_Authorization(){
    //char * Authorization=NULL;
    char buff[MAXBUFSIZE];
    char a[20];
    char * accessToken;
    FILE * authorization;
    
    char * cmd= "curl -s -k -X POST 'https://192.168.0.13/irisservices/api/v1/public/accessTokens' -H 'accept: application/json' -H 'content-type: application/json' -d '{ \"domain\": \"LOCAL\", \"password\": \"admin\", \"username\": \"admin\"}'";
    authorization = popen(cmd, "r");
    //cJSON * root=cJSON_CreateObject();
    //cJSON * item = NULL;
    if(fgets (buff, sizeof(buff), authorization)!=NULL){
        sscanf (buff, "%s\n",a);
        //free(buff);
        cJSON * root = NULL;
        cJSON * item = NULL;
        //printf("quan %s\n",a);
        root = cJSON_Parse(a);
        
        if (!root)
        {
            printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        }
        else
        {
            item = cJSON_GetObjectItem(root, "accessToken");
            //printf("1: %s\n", cJSON_Print(item));
            accessToken=item->valuestring;
            //strcpy(accessToken,item->valuestring);
            //printf("authorization info: %s\n",accessToken);
        }
        //if (root)
        // cJSON_Delete(root);
        //cJSON_Delete(root);
        //cJSON_Delete(item);
       // printf("authorization info: %s\n",accessToken);
    }
    //Authorization=a;
    //cJSON_Delete(root);
    pclose(authorization);
    //    if (root)
    //        cJSON_Delete(root);
    //free(buff);
    strcat (authorization_cmd,accessToken);
    strcat (authorization_cmd,"'");
    return authorization_cmd;
    //return accessToken;
}
//char * Splice_character(){
//    //char * character;
//    strcat (authorization_cmd,POST_Authorization());
//   // printf(POST_Authorization());
//    //free(POST_Authorization());
//    strcat (authorization_cmd,"'");
//    //character=authorization_cmd;
//    return authorization_cmd;
//}
//风扇状态
char * get_Fan_Status(){
    //char * fanstatus;
    FILE * fan_status;
    char buff[MAXBUFSIZE];
    char a[20];
    char b[20];
    char c[20];
    //char d[20];
    //fan_status = popen("sudo ipmitool sensor get 'Sys Fan 1A'","r");
    fan_status = popen("sudo ipmitool sensor get 'Sys Fan 1A'","r");
    
    int i=0;
    
    while(i++<30){
        if(fgets (buff, sizeof(buff), fan_status)!=NULL){
            sscanf (buff, "%s %s %s ",a,b,c);
            if(strcmp(a, "Status")==0){
                printf("fan status :%s \n",c);
                //fanstatus=c;
                strcpy(fanstatus,c);
            }
        }
    }
    fclose(fan_status);
    return fanstatus;
}
