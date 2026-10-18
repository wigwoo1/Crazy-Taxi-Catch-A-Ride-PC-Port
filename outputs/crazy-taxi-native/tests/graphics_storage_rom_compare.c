#include "graphics_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
int main(int argc,char **argv){
 if(argc!=2)return 1;
 uint32_t seed=(uint32_t)strtoul(argv[1],NULL,16);TaxiGraphicsStorage s;
 s.state_6844=s.background_6850=s.state_6904=seed;
 for(unsigned i=0;i<12288;i++)s.pool_1b6ec[i]=seed+i;
 for(unsigned i=0;i<4096;i++)s.pool_276ec[i]=seed-i;
 for(unsigned i=0;i<128;i++)s.heads_2b6ec[i]=seed^i;
 if(taxi_graphics_storage_reset(&s))return 2;
 _setmode(_fileno(stdout),_O_BINARY);
 if(fwrite(&s,1,sizeof s,stdout)!=sizeof s)return 3;
 return 0;
}
