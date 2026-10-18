#include "../src/pair_column_raster.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
int main(int argc,char **argv){assert(argc==1||argc==2);unsigned char *rom=NULL;if(argc==2){FILE *file=fopen(argv[1],"rb");assert(file);rom=malloc(8388608);assert(rom&&fread(rom,1,8388608,file)==8388608&&fgetc(file)==EOF);assert(!fclose(file));}assert(_setmode(_fileno(stdout),_O_BINARY)!=-1);unsigned seed,dst,rows,mx,my,sx,sy,x,y,dx,dy;
 while(scanf("%u %u %u %u %u %u %u %u %u %u %u",&seed,&dst,&rows,&mx,&my,&sx,&sy,&x,&y,&dx,&dy)==11){
  uint8_t texture[4096],fb[38400];for(unsigned i=0;i<4096;i++)texture[i]=(uint8_t)((i*37u+seed)^(i>>3));memset(fb,173,sizeof fb);
  if(rom)assert(seed<=8388608-4096);const uint8_t *source=rom?rom+seed:texture;
  assert(!taxi_pair_column_raster(fb,dst,source,sizeof texture,mx,my,sx,sy,&x,&y,dx,dy,rows));
  assert(fwrite(fb,1,sizeof fb,stdout)==sizeof fb);assert(fwrite(&x,4,1,stdout)==1&&fwrite(&y,4,1,stdout)==1);
 }free(rom);return ferror(stdin)?1:0;}
