#include "../src/title_obj_queue.h"
#include <stdio.h>
#include <string.h>
static unsigned char rom[8388608];
static unsigned word(unsigned n){return (unsigned)rom[n]|(unsigned)rom[n+1]<<8|(unsigned)rom[n+2]<<16|(unsigned)rom[n+3]<<24;}
int main(int argc,char **argv){if(argc!=2)return 2;FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 unsigned group,frame,x,y,z,bucket,priority,flags,mask,tile,palette_base,palette,total,count,seed,width,height;
 while(scanf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",&group,&frame,&x,&y,&z,&bucket,&priority,&flags,&mask,&tile,&palette_base,&palette,&total,&count,&seed,&width,&height)==17){
  unsigned resource=0x70e50,g=resource+word(resource+4+group*4),mpos=resource+word(g+20+frame*4);uint16_t metrics[9];for(unsigned i=0;i<9;i++)metrics[i]=(uint16_t)(rom[mpos+i*2]|rom[mpos+i*2+1]<<8);
  TaxiStartupObjQueue q={0};q.total=(uint8_t)total;for(unsigned b=0;b<3;b++){q.count[b]=(uint8_t)count;for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)q.attributes[b][i][j]=(uint16_t)(seed+b*1000+i*37+j*53);}
  q.last_width=99;q.last_height=165;for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)q.depth[i][j]=(uint16_t)(seed+i*37+j*53);
  TaxiStartupObjQueued out;int result=taxi_title_obj_queue(&q,metrics,(int32_t)x,(int32_t)y,(int32_t)z,(uint16_t)width,(uint16_t)height,(uint8_t)bucket,(uint8_t)priority,(uint8_t)flags,(uint16_t)mask,tile,(int32_t)palette_base,palette,&out);
  printf("%d %u %u %u %u %u %u ",result,q.total,q.count[0],q.count[1],q.count[2],q.last_width,q.last_height);
  const unsigned char *data=(const unsigned char *)q.attributes;for(unsigned i=0;i<sizeof q.attributes;i++)printf("%02x",data[i]);data=(const unsigned char *)q.depth;for(unsigned i=0;i<sizeof q.depth;i++)printf("%02x",data[i]);puts("");
 }return 0;}
