#include "../src/boot_services.h"
#include <stdio.h>
#include <stdlib.h>
static TaxiBootMemory s;
int main(int argc,char **argv){if(argc!=4)return 2;unsigned mode=(unsigned)atoi(argv[1]),seed=(unsigned)atoi(argv[2]);
 for(unsigned i=0;i<sizeof s.external;i++)s.external[i]=(uint8_t)(seed+i*13);
 if(mode?taxi_boot_shade_init(&s):taxi_boot_timing_init(&s))return 3;
 FILE *f=fopen(argv[3],"wb");if(!f)return 4;fwrite(s.external,1,sizeof s.external,f);return fclose(f)!=0;}
