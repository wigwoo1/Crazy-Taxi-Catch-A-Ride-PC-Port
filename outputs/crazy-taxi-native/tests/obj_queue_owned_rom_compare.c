#include "../src/obj_queue_owned.h"
#include <stdio.h>
#include <string.h>
static unsigned char rom[8388608];
int main(int argc,char **argv){if(argc!=2)return 2;FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 unsigned group,frame,x,y,z,bucket,priority,flags,mask,tile,palette_base,palette,total,count,seed,width,height;
 while(scanf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",&group,&frame,&x,&y,&z,&bucket,&priority,&flags,&mask,&tile,&palette_base,&palette,&total,&count,&seed,&width,&height)==17){
  TaxiStartupObjQueue q={0};q.total=(uint8_t)total;for(unsigned b=0;b<3;b++){q.count[b]=(uint8_t)count;for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)q.attributes[b][i][j]=(uint16_t)(seed+b*1000+i*37+j*53);}
  q.last_width=99;q.last_height=165;for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)q.depth[i][j]=(uint16_t)(seed+i*37+j*53);
  uint32_t bases[32];int32_t palettes[32];for(unsigned i=0;i<32;i++){bases[i]=0x06010000+(tile<<5);palettes[i]=(int32_t)palette_base;}
  TaxiObjQueueOwner owner={rom,sizeof rom,0x08070e50,bases,palettes,32,&q,(uint16_t)width,(uint16_t)height,(uint16_t)mask};uint32_t position[3]={x,y,z};TaxiStartupObjQueued out;
  int result=taxi_obj_queue_owned(&owner,frame,group,position,priority,bucket,flags,palette,&out);
  printf("%d %u %u %u %u %u %u ",result,q.total,q.count[0],q.count[1],q.count[2],q.last_width,q.last_height);
  const unsigned char *data=(const unsigned char *)q.attributes;for(unsigned i=0;i<sizeof q.attributes;i++)printf("%02x",data[i]);data=(const unsigned char *)q.depth;for(unsigned i=0;i<sizeof q.depth;i++)printf("%02x",data[i]);puts("");
 }return 0;}
