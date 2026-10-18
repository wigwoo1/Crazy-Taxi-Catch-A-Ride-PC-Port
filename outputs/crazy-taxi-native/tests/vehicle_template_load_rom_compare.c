#include "vehicle_template_load.h"
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv){assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);unsigned offset,seed;
 while(scanf("%u %u",&offset,&seed)==2){uint8_t data[584],actor[0xea0];assert(!fseek(f,(long)offset,SEEK_SET));assert(fread(data,1,584,f)==584);
 for(unsigned i=0;i<sizeof actor;i++)actor[i]=(uint8_t)(seed+i*37);
 assert(!taxi_vehicle_template_prepare(actor,sizeof actor,data,sizeof data));for(unsigned i=0;i<sizeof actor;i++)printf("%02x",actor[i]);puts("");
 }fclose(f);return 0;}
