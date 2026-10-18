#include "../src/pair_column_draw.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);uint8_t *rom=malloc(8388608);assert(rom&&fread(rom,1,8388608,f)==8388608&&fgetc(f)==EOF);assert(!fclose(f));assert(_setmode(_fileno(stdout),_O_BINARY)!=-1);unsigned first,count,top,bottom,offset,mx,my,sx,sy;
 while(scanf("%u %u %u %u %u %u %u %u %u",&first,&count,&top,&bottom,&offset,&mx,&my,&sx,&sy)==9){uint32_t d[960];assert(count<=120&&offset<=8388608-4096);for(unsigned i=0;i<count*8;i++)assert(scanf("%u",d+i)==1);uint8_t fb[38400];memset(fb,173,sizeof fb);assert(!taxi_pair_column_batch(fb,first,count,d,top,bottom,rom+offset,4096,mx,my,sx,sy));assert(fwrite(fb,1,sizeof fb,stdout)==sizeof fb);}free(rom);return 0;}
