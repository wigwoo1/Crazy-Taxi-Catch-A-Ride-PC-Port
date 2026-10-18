#include "source_frame_native.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
static void get(void *p,size_t n,FILE *f){if(fread(p,1,n,f)!=n)exit(2);}
int main(int argc,char **argv){if(argc!=3)return 1;FILE *r=fopen(argv[1],"rb"),*f=fopen(argv[2],"rb");if(!r||!f)return 2;uint8_t *rom=malloc(8388608);if(!rom)return 3;get(rom,8388608,r);fclose(r);
 uint32_t h[9],count;get(h,sizeof h,f);TaxiSourceFrameConfig c={0};if(h[0]<0x08000000||h[0]+h[1]*240>0x08800000)return 4;c.pixels=rom+h[0]-0x08000000;c.height=h[1];c.size=h[1]*240;c.pitch=(int32_t)h[2];c.yaw=(int32_t)h[3];c.bounds=(TaxiTrafficScreenBounds){(int32_t)h[4],(int32_t)h[5],(int32_t)h[6],(int32_t)h[7]};c.fill=h[8];get(&c.projection,sizeof c.projection,f);get(&count,4,f);if(count!=101)return 5;
 TaxiSourceFramePacket packets[101]={0};for(unsigned i=0;i<count;i++){uint32_t p[9];get(p,sizeof p,f);TaxiSourceFramePacket *out=packets+i;out->bucket=p[0];out->flags=(uint16_t)p[1];out->material_flags=(uint8_t)p[2];out->level=p[3];size_t size=(p[4]>>p[3])*(p[5]>>p[3]);if(p[7]<0x08000000||p[7]+size>0x08800000||p[8]<3||p[8]>4)return 6;out->texture=(TaxiIndexedTexture){p[4],p[5],p[6],0,rom+p[7]-0x08000000,size};get(out->vertices,p[8]*sizeof *out->vertices,f);}
 fclose(f);uint8_t frame[38400];TaxiSourceFrameResult result;if(taxi_source_frame_native(frame,&c,packets,count,&result))return 7;free(rom);if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 8;fprintf(stderr,"callbacks%u halfwordwrites%u\n",result.callbacks,result.halfword_writes);return fwrite(frame,1,sizeof frame,stdout)==sizeof frame?0:9;}
