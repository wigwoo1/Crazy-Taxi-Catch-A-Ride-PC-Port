#include "../src/indexed_pixel.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){
 if(argc!=2)return 2;
 if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 10;
 FILE *f=fopen(argv[1],"rb");if(!f)return 3;
 unsigned char *rom=malloc(8388608);if(!rom)return 4;
 if(fread(rom,1,8388608,f)!=8388608)return 5;
 fclose(f);
 int frame,l,t,r,b,c;
 while(scanf("%d%d%d%d%d%d",&frame,&l,&t,&r,&b,&c)==6){
  if(frame<0||frame>=8384512)return 6;
  uint32_t ptr=0x08000000u+(unsigned)frame;
  if(ptr<0x08000000||ptr>0x087ff000)return 7;
  TaxiPairColumnWork work={0};uint8_t pixels[38400];memset(pixels,173,sizeof pixels);
  if(argc==2){
   if(!taxi_indexed_billboard_work(pixels,l,t,r,b,rom+ptr-0x08000000,2048,64,32,6,c,&work))return 8;
  }else{
   TaxiIndexedSpan spans[160];uint8_t lookup[2560];
   for(unsigned j=0;j<2560;j++)lookup[j]=(uint8_t)j;
   if(!taxi_indexed_billboard_rows_bounds(spans,l,t,r,b,64,64,0,240,c,0))return 11;
   for(unsigned y=0;y<160;y++){
    TaxiIndexedSpan *q=spans+y;if(!q->pairs)continue;
    if(!taxi_indexed_keyed_span(pixels+y*240+(unsigned)q->x,q->pairs,
     rom+ptr-0x08000000,4096,lookup,q->u,q->v,q->shade,q->du,q->dv,q->dshade,63,4032,14,8))return 12;
   }
  }
  if(fwrite(pixels,1,sizeof pixels,stdout)!=sizeof pixels)return 9;
  if(fwrite(&work,1,sizeof work,stdout)!=sizeof work)return 13;
 }
 free(rom);return 0;
}
