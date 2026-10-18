#include "../src/pair_column_draw.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){assert(argc==2);FILE *file=fopen(argv[1],"rb");assert(file);uint8_t *rom=malloc(8388608);assert(rom&&fread(rom,1,8388608,file)==8388608&&fgetc(file)==EOF);assert(!fclose(file));assert(_setmode(_fileno(stdout),_O_BINARY)!=-1);
 unsigned d[8],top,bottom,col,offset,mx,my,sx,sy;
 while(scanf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",d,d+1,d+2,d+3,d+4,d+5,d+6,d+7,&top,&bottom,&col,&offset,&mx,&my,&sx,&sy)==16){uint8_t fb[38400];memset(fb,173,sizeof fb);assert(offset<=8388608-4096);assert(taxi_pair_column_draw(fb,col,d,top,bottom,rom+offset,4096,mx,my,sx,sy)>=0);assert(fwrite(fb,1,sizeof fb,stdout)==sizeof fb);}free(rom);return 0;}
