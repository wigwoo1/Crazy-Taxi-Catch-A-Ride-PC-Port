#include "../src/boot_memory.h"
#include <stdio.h>
#include <stdlib.h>
static TaxiBootMemory s;
int main(int argc,char **argv){if(argc!=4)return 2;unsigned flag=(unsigned)atoi(argv[1]),seed=(unsigned)atoi(argv[2]);
 for(unsigned i=0;i<sizeof s.external;i++)s.external[i]=(uint8_t)(seed+i*13);
 for(unsigned i=0;i<sizeof s.internal;i++)s.internal[i]=(uint8_t)(seed+i*19);
 s.external[0x1ea5]=(uint8_t)flag;if(taxi_boot_memory_prepare(&s))return 3;
 FILE *f=fopen(argv[3],"wb");if(!f)return 4;fwrite(s.external,1,sizeof s.external,f);fwrite(s.internal,1,sizeof s.internal,f);fputc(s.wait_control&255,f);fputc(s.wait_control>>8,f);return fclose(f)!=0;}
