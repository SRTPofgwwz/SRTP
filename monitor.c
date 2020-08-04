#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<sys/time.h>
#include<stdlib.h>

#define FILE_NAME_SIZE 255

//数据包
int pid_data[16];//pid
int port_data[16];//端口占用
int physical_memory[40];//物理内存
int virtual_memory[64];//虚拟内存
int pcpu_i[8];//cpu占用率整数部分
int pcpu_d[8];//cpu占用率小数部分
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

//将十进制数转为指定位数的二进制数
void transform(int a[],int number,int digit)//转换的数组，转换的十进制数，转换位数
{
    int i,b[digit];
   for(i=0;i<digit;i++)
   {
       b[i]=number%2;
       number/=2;
   }
   for(i=0;i<digit;i++)
   {
       a[i]=b[digit-i-1];
   }
}
//十进制小数转为二进制
void trans_d(int a[], float number,int digit)
{
   int len=0;
   while(number)
   {
       int temp=(int)(number*2);
       a[len++]=temp;
       if(len==digit)
       break;//控制小数点位数
       number=2*number-temp;
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
{  int pid;
    FILE *f =popen("ps -e | grep \'code\' | awk \'{print $1}\' ","r");//进程名称
    char buffer[10]={0};
    while(NULL!=fgets(buffer,10,f))
    {
    printf("PID:%s\n",buffer);
    }
    pid=atoi(buffer);
    pclose(f);
    transform(pid_data,pid,16);
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
    transform(pcpu_i,(int)pcpu,8);
    float temp=pcpu-(int)pcpu;
    trans_d(pcpu_d,temp,8);
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
    fclose(f);
    return result;
 
}

void displayStatm(pid_t pid, statm_t statm)
{
    printf("Process%dMemory Use:\n", pid);
    printf("\t size \t resident  \n");
    printf("\t %lu \t %lu \n", statm.size*1024, statm.resident*1024);
    transform(physical_memory,statm.resident*1024,40);
    transform(virtual_memory,statm.size*1024,64);
}
//查看进程端口调用
void get_port(int pid, FILE*f1)
{   int port=0;//存放端口号
    char str[30];//存放command
    char buff1[10];//存放pid
    char buff2[]="| awk 'NR==1{print $4}'";//筛选端口号
    strcpy(str,"sudo netstat -nap | grep ");
    sprintf(buff1,"%d",pid);//整数转换为字符串
    strcat(str,buff1);
    strcat(str,buff2);//字符串拼接
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
      getport[j]=buffer[i+1];
      i++;
      j++;
    }
    port=atoi(getport);
    printf("%d",port);
    transform(port_data,port,16);
    for(i=0;i<16;i++)
        {
            fprintf(f1,"%d", port_data[i]);
        }
    pclose(f);
}

int main()

{  //while(get_pid()==0);
    //int pid=get_pid();
 int pid=2106;
 transform(pid_data,pid,16);
    const char*path="/home/zyj/桌面/8";
    
   // while(1)
 //   {
        int i=0; 
        FILE*fd=fopen(path,"a");
        statm_t statm=get_statm(pid);
        displayStatm(pid,statm);
       //   for(i=0;i<16;i++)
      //  {
        //    fprintf(fd,"%d", pid_data[i]);
      //  }
       // for(i=0;i<40;i++)
      //  {
      //      fprintf(fd,"%d", physical_memory[i]);
      //  }
       //   for(i=0;i<64;i++)
       // {
         //   fprintf(fd,"%d", virtual_memory[i]);
     //   }
      //  printf("cputime=%lu\n", get_cpu_total_occupy());
     //   printf("protime=%lu\n", get_cpu_proc_occupy(pid));
       printf("pcpu=%f\n",get_proc_cpu(pid));
      for(i=0;i<8;i++)
       {
            fprintf(fd,"%d", pcpu_i[i]);
       }
       for(i=0;i<8;i++)
       {
            fprintf(fd,"%d", pcpu_d[i]);
       }
      //  fprintf(fd,"cputime=%lu\n", get_cpu_total_occupy());
    //    fprintf(fd,"protime=%lu\n", get_cpu_proc_occupy(pid));
    //    fprintf(fd,"pcpu=%f\n",get_proc_cpu(pid));
     //get_port(pid,fd);
       fclose(fd);
      // sleep(1);
  //  }
    return 0;
}