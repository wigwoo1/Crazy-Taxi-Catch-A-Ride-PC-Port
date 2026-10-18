/* Native recovered routines only; original instruction runners stay offline. */
#include "indexed_pixel.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
static void read_exact(void *p,size_t n,FILE *f){if(fread(p,1,n,f)!=n)exit(2);}
int main(int argc,char **argv){
 if(argc!=3)return 1;
 FILE *romfile=fopen(argv[1],"rb"),*input=fopen(argv[2],"rb");if(!romfile||!input)return 2;
 uint8_t *rom=malloc(8388608);if(!rom)return 3;read_exact(rom,8388608,romfile);fclose(romfile);
 uint8_t fb[38400];read_exact(fb,sizeof fb,input);uint32_t count;read_exact(&count,4,input);
 unsigned writes=0;
 for(uint32_t i=0;i<count;i++){
  uint32_t h[7];read_exact(h,sizeof h,input);unsigned entry=h[0],n=h[1],width=h[2],height=h[3],log=h[4],level=h[5],ptr=h[6];
  if(n<3||n>8||ptr<0x08000000||ptr>=0x08800000)return 4;
  TaxiIndexedVertex vertices[8];read_exact(vertices,n*sizeof *vertices,input);
  TaxiIndexedSampling sampling;if(!taxi_indexed_sampling(&sampling,width,height,log,level))return 5;
  size_t size=(width>>level)*(height>>level),offset=ptr-0x08000000;if(offset+size>8388608)return 6;
  TaxiPairColumnWork work={0};int ok=0;
  if(entry==0x03000564)ok=taxi_indexed_perspective_rows_polygon_work(fb,vertices,n,rom+offset,size,sampling,&work);
  else if(entry==0x03000d50)ok=taxi_indexed_unshaded_clipped_polygon_work(fb,vertices,n,rom+offset,size,sampling,&work);
  else if(entry==0x03001038)ok=taxi_indexed_unshaded_polygon_work(fb,vertices,n,rom+offset,size,sampling,&work);
  if(!ok){fprintf(stderr,"native polygon failed %u entry%08x\n",i,entry);return 7;}
  writes+=work.framebuffer_halfword_writes;
 }
 fclose(input);free(rom);if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 8;
 if(fwrite(fb,1,sizeof fb,stdout)!=sizeof fb)return 9;
 fprintf(stderr,"native draws%u halfwordwrites%u\n",count,writes);return 0;
}
