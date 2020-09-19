#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<bitset>
#include<fstream>
#include<iostream>
#include<math.h>
#define FILE_NAME_SIZE 255
using namespace std;
ifstream infile;
int main()
{
    int datalen=0;
    int group=0;
    int pid,v_m,p_m,pcpu,port;
    char data[152000];
    int i,j;
    infile.open("/home/zyj/桌面/10",ios::in);
    if(!infile.is_open())
    cout<<"open file error";
    while(!infile.eof()&&infile.good())
    infile>>data[datalen++]; 
    group=(datalen-1)/152;
    for (i=0;i<group;i++)
 { 
     for(j=0;j<16;j++)//pid
      {     if(data[(j+152*i)]=='1')
               pid+=pow(2,16-j-1);
      }
      cout<<"pid:"<<pid<<endl;
      for(j=16;j<32;j++)//cpu占用率
      {   if(j<24)
        {  
          if(data[(j+152*i)]=='1')
               pcpu+=pow(2,24-j-1);
        }
          else
          {
                if(data[(j+152*i)]=='1')
                pcpu+=pow(0.5,j-23);
          }
      }
      cout<<"pcpu:"<<pcpu<<endl;
      for(j=32;j<72;j++)//物理内存
      {     if(data[(j+152*i)]=='1')
               p_m+=pow(2,72-j-1);
      }
      cout<<"physical memory:"<<p_m<<endl;
      for(j=72;j<136;j++)//虚拟内存
      {     if(data[(j+152*i)]=='1')
               v_m+=pow(2,136-j-1);
      }
      cout<<"virtual memory:"<<v_m<<endl;
      for(j=136;j<152;j++)//port
      {     if(data[(j+152*i)]=='1')
               port+=pow(2,152-j-1);
      }
      cout<<"port:"<<port<<endl;
      pid=0;
      pcpu=0;
      v_m=0;
      p_m=0;
      port=0;
 }
   infile.close();
return 0;
}
