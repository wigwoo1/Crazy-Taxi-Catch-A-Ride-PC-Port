#include "vehicle_template_load.h"
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv){assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);unsigned offset,seed,settings;
 while(scanf("%u %u %u",&offset,&seed,&settings)==3){uint8_t data[584],actor[0xea0];assert(!fseek(f,(long)offset,SEEK_SET));assert(fread(data,1,584,f)==584);
 for(unsigned i=0;i<sizeof actor;i++)actor[i]=(uint8_t)(seed+i*37);
 int32_t state=12345;assert(!taxi_vehicle_template_initialize(actor,sizeof actor,data,sizeof data,(uint8_t)settings,&state));for(unsigned i=0;i<sizeof actor;i++)printf("%02x",actor[i]);printf(" %d\n",state);
 }fclose(f);return 0;}
