#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<sys/time.h>
#include<stdlib.h>
#include<bitset>
#include<fstream>
#include<iostream>
#define FILE_NAME_SIZE 255
using namespace std;


typedef struct {
    unsigned long size;//虚拟地址空间
    unsigned long  resident; //物理内存
   
}statm_t;

typedef struct{
    unsigned long user;//正常进程在用户态下执行时间累计
    unsigned long nice;//NICED的进程在用户态下执行时间
    unsigned long system;//进程在内核态的执行时间累计
    unsigned long idle;//空闲时间累计
}Total_Cpu_Occupy_t;

typedef struct {
    unsigned int pid;
    unsigned long utime;//user time
    unsigned long stime;//kernel time
    unsigned long cutime;//all user time 
    unsigned long cstime;//all dead time

}Proc_Cpu_Occupy_t;

char *filename="/home/zyj/桌面/10";//输出文件路径
ofstream f(filename);

//将十进制数据转为指定位数的二进制数据(bit)
void transform(int data,const int digit, const char*filename)//转换的数据，转换位数，写入的文件
{
   int i=0;
   bitset<64>bitset1(data);
   if(f.fail())
   {
       cout<<"error\n";
   }
   for(i=digit-1;i>=0;i--)
   {
     f<<bitset1[i];
   }
}

//cpu占用率转为二进制(bit)
void trans_pcpu(float data,int digit,char*filename)
{
    if(data==0)
  {  
    bitset<16>bitset_0(0);
     for(int i=0;i<16;i++)
     f<<bitset_0[i];
   }
    else{
    transform((int)data,8,filename);//cpu占用率整数部分
    float pcpu_d=data-(int)data;//小数部分
    int len=0;
    int a[8];
    if (pcpu_d!=0)
   { while(pcpu_d)
      { 
       int temp=(int)(pcpu_d*2);
       a[len++]=temp;
       if(len==digit)
       break;//控制小数点位数
       pcpu_d=2*pcpu_d-temp;
       }
   }
   else
   {
     for(int i=0;i<8;i++)
     a[i]=0; 
   }
   
      char s[9];
     for(int i=0;i<8;i++)
     {
        s[i]=a[i]+'0';
     }
     s[8]='\0';//整形数组转为字符串
     bitset<8>bitset2(s);
      if(f.fail())
     {
       cout<<"error\n";
     }
     for(int i=digit-1;i>=0;i--)
     {
     f<<bitset2[i];
     }
    }
}

//获取第n项开始的指针
const char* get_items(const char* buffer, unsigned int item)
{
    const char * p=buffer;
    int len =strlen(buffer);
    int count =0;
    for(int i=0;i<len;i++)
    {
          if(' ' ==*p)
          {
              count++;
              if(count ==item-1)
              {
                  p++;
                  break;
              }
          }
          p++;
    }
    return p;
}

//获取进程pid
int get_pid()
{ 
     int pid=0;
    FILE *f =popen("ps -e | grep \'firefox\' | awk \'{print $1}\' ","r");//进程名称:seu
    char buffer[10]={0};
    while(NULL!=fgets(buffer,10,f));
    pid=atoi(buffer);
    pclose(f);
    return pid;
}

//获取总CPU时间
unsigned long get_cpu_total_occupy()
{
    FILE *f;
    char buff[1024]={0};
    Total_Cpu_Occupy_t t;
    f=fopen("/proc/stat","r");
    if(NULL==f)
    {
        return 0;
    }
    fgets(buff, sizeof(buff),f);
    char name[64]={0};
    sscanf(buff, "%s %ld %ld %ld %ld",name,&t.user,&t.nice,&t.system,&t.idle);
    fclose(f);
    return(t.user+t.nice+t.system+t.idle);
}

//获取进程CPU时间
unsigned long get_cpu_proc_occupy(unsigned int pid)
{
    char file_name[64]={0};
    Proc_Cpu_Occupy_t t;
    FILE *f;
    char p_buff[1024]={0};
    sprintf(file_name, "/proc/%d/stat",pid);
    f=fopen(file_name,"r");
       if(NULL==f)
    {
        return 0;
    }
    fgets(p_buff, sizeof(p_buff),f);
    sscanf(p_buff,"%u",&t.pid);
    const char*q=get_items(p_buff,14);//从第14项开始
    sscanf(q,"%ld %ld %ld %ld", &t.utime,&t.stime,&t.cutime,&t.cstime );
    fclose(f);
    return(t.utime+t.stime+t.cutime+t.cstime);
}

//获取CPU占用率
float get_proc_cpu(unsigned int pid)
{
    unsigned long totalcputime1, totalcputime2;
    unsigned long procputime1, procputime2;
    totalcputime1=get_cpu_total_occupy();
    procputime1=get_cpu_proc_occupy(pid);
    sleep(1);
    totalcputime2=get_cpu_total_occupy();
    procputime2=get_cpu_proc_occupy(pid);
    float pcpu =0.0;
    pcpu=4*100.0*(procputime2-procputime1)/(totalcputime2-totalcputime1);//进程时间差与cpu时间差比值
   
    return pcpu;
}

//获取内存使用情况
statm_t get_statm (pid_t pid)
{
    statm_t result={0,0};
    char FILE_NAME[FILE_NAME_SIZE]="/proc/self/statm";
    sprintf(FILE_NAME, "/proc/%d/statm", pid);
    FILE*f=fopen(FILE_NAME,"r");
    fscanf(f, "%lu%lu", &result.size, &result.resident);
    result.size=result.size*1024;
    result.resident=result.resident*1024;//单位：Byte
    fclose(f);
    return result;
}

//查看进程端口调用
int get_port(int pid)
{   
    int port=0;//存放端口号
    char str[30];//存放command
    char buff1[10];//存放pid
    char buff2[]="| awk 'NR==1{print $4}'";//筛选端口号
    strcpy(str,"sudo netstat -nap | grep ");
    sprintf(buff1,"%d",pid);//整数转换为字符串
    strcat(str,buff1);
    strcat(str,buff2);//字符串拼接
    while(port==0)
 {
    FILE *f =popen(str,"r");//进程名称
    char buffer[1024]={0};
    while(NULL!=fgets(buffer,1024,f));//存入buffer
    char getport[256];
    int i,j,k;
    for(i=0;i<20;i++)//筛选出端口号
    {
       if (buffer[i]==':')
       break;
    }
    k=i;
    j=0;
    while(buffer[i+1])
    {
      getport[j++]=buffer[i+1];
      i++;  
    }
    port=atoi(getport);
    pclose(f);
 }
   return port;
}

int main()
{  
    //while(get_pid()==0);
    int pid=23124;
for(int i=0;i<2;i++)
   { 
     cout<<"pid:"<<pid<<endl;
     transform(pid,16,filename);
     int pcpu=get_proc_cpu(pid);
     cout<<"pcpu:"<<pcpu<<endl;
     trans_pcpu(pcpu,8,filename);
       statm_t statm=get_statm(pid);
       transform(statm.resident ,40 ,filename);//物理内存
       transform(statm.size, 64,filename);//虚拟内存
       cout<<"physical  memory:"<<statm.resident<<" Byte"<<endl;
       cout<<"virtual memory:"<<statm.size<<" Byte"<<endl;
       transform(get_port(pid),16,filename);
       cout<<"port:"<<get_port(pid)<<endl;
   }
  /*  while(get_pid()!=0)
    {
        cout<<"pid:"<<get_pid()<<endl;
    transform(get_pid(),16,filename);
     cout<<"pcpu:"<<get_proc_cpu(get_pid())<<endl;
       trans_pcpu(get_proc_cpu(get_pid()),8,filename);
       statm_t statm=get_statm(get_pid());
       transform(statm.resident ,40 ,filename);//物理内存
       transform(statm.size, 64,filename);//虚拟内存
       cout<<"physical  memory:"<<statm.resident<<" Byte"<<endl;
       cout<<"virtual memory:"<<statm.size<<" Byte"<<endl;
       transform(get_port(get_pid()),16,filename);
       cout<<"port:"<<get_port(get_pid())<<endl;
     }*/
    f.close();
    return 0;
}