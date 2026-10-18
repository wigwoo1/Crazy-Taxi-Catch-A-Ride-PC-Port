#include "../src/boot_memory.h"
#include <stdio.h>
#include <stdlib.h>
static TaxiBootMemory s;static unsigned char rom[8388608];
int main(int argc,char **argv){if(argc!=4)return 2;unsigned seed=(unsigned)atoi(argv[2]);FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 for(unsigned i=0;i<sizeof s.external;i++)s.external[i]=(uint8_t)(seed+i*13);
 if(taxi_boot_trig_load(&s,rom,sizeof rom))return 4;
 f=fopen(argv[3],"wb");if(!f)return 5;fwrite(s.external,1,sizeof s.external,f);return fclose(f)!=0;}
