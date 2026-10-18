#include "../src/startup_obj_queue.h"
#include <stdio.h>
int main(void){unsigned char bytes[602];FILE *f=fopen("outputs/crazy-taxi-native/assets/startup-animation/sprites.crs","rb");if(!f)return 1;size_t n=fread(bytes,1,sizeof bytes,f);fclose(f);if(n!=sizeof bytes)return 1;
 unsigned frame,x,y,z,bucket,priority,flags,mask,tile,total,count,seed,width,height;
 while(scanf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u",&frame,&x,&y,&z,&bucket,&priority,&flags,&mask,&tile,&total,&count,&seed,&width,&height)==14){
  if(frame>=33)return 1;uint16_t m[9];for(unsigned i=0;i<9;i++){unsigned a=8+frame*18+i*2;m[i]=(uint16_t)(bytes[a]|bytes[a+1]<<8);}
  TaxiStartupObjQueue q={.total=(uint8_t)total,.count={(uint8_t)count,(uint8_t)count,(uint8_t)count},.last_width=99,.last_height=165};
  for(unsigned b=0;b<3;b++)for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)q.attributes[b][i][j]=(uint16_t)(seed+b*1000u+i*37u+j*53u);
  for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)q.depth[i][j]=(uint16_t)(seed+i*37u+j*53u);
  TaxiStartupObjQueued out={0};int result=taxi_startup_obj_queue(&q,m,(int32_t)x,(int32_t)y,(int32_t)z,(uint16_t)width,(uint16_t)height,(uint8_t)bucket,(uint8_t)priority,(uint8_t)flags,(uint16_t)mask,tile,&out);if(result<0)return 1;
  printf("%u %u %u %u %u %u %u",result,q.total,q.count[0],q.count[1],q.count[2],q.last_width,q.last_height);printf(" ");
  for(unsigned b=0;b<3;b++)for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)printf("%02x%02x",q.attributes[b][i][j]&255,q.attributes[b][i][j]>>8);
  for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)printf("%02x%02x",q.depth[i][j]&255,q.depth[i][j]>>8);putchar('\n');
 }return 0;}
