#include "../src/taxi_affine_queue.h"
#include <stdlib.h>
static unsigned word(const unsigned char *p){return p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;}
#include <stdio.h>
int main(int argc,char **argv){unsigned effect_group=argc>1?(unsigned)strtoul(argv[1],NULL,10):8;unsigned frames=effect_group==8?4:effect_group==9?3:effect_group==10?9:0;if(!frames)return 1;unsigned char *rom=malloc(8388608);FILE *f=fopen("Crazy Taxi - Catch a Ride (USA).gba","rb");if(!f||!rom)return 1;size_t n=fread(rom,1,8388608,f);fclose(f);if(n!=8388608)return 1;

 unsigned frame,x,y,z,bucket,priority,flags,mask,tile,total,count,seed,width,height,scale,state,angle;int sine,cosine;
 while(scanf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %d %d",&frame,&x,&y,&z,&bucket,&priority,&flags,&mask,&tile,&total,&count,&seed,&width,&height,&scale,&state,&angle,&sine,&cosine)==19){
  if(frame>=frames)return 1;uint16_t m[9];unsigned resource=0x70e50,group=resource+word(rom+resource+4+effect_group*4),metric=resource+word(rom+group+20+frame*4);for(unsigned i=0;i<9;i++)m[i]=(uint16_t)(rom[metric+i*2]|rom[metric+i*2+1]<<8);
  TaxiStartupObjQueue q={.total=(uint8_t)total,.count={(uint8_t)count,(uint8_t)count,(uint8_t)count},.last_width=99,.last_height=165};
  for(unsigned b=0;b<3;b++)for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)q.attributes[b][i][j]=(uint16_t)(seed+b*1000u+i*37u+j*53u);
  for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)q.depth[i][j]=(uint16_t)(seed+i*37u+j*53u);
  TaxiStartupObjTransport t={0};for(unsigned i=0;i<32;i++){t.matrices[i].state=state>=3?1:state;for(unsigned j=0;j<4;j++)t.matrices[i].values[j]=(int16_t)(seed+i*37u+j*53u);}
  if(state>=3){if(state==3)t.matrices[0].state=0;int16_t values[4];taxi_obj_matrix_values(sine,cosine,(int16_t)scale,values);for(unsigned j=0;j<4;j++)t.matrices[31].values[j]=values[j];}
  TaxiStartupObjQueued out={0};int result=taxi_affine_queue(&q,&t,m,(int32_t)x,(int32_t)y,(int32_t)z,(uint16_t)width,(uint16_t)height,(uint8_t)bucket,(uint8_t)priority,(uint8_t)flags,(uint16_t)mask,tile,4,0,(int16_t)scale,(int16_t)angle,sine,cosine,&out);if(result<0)return 1;
  printf("%u %u %u %u %u %u %u",result,q.total,q.count[0],q.count[1],q.count[2],q.last_width,q.last_height);printf(" ");
  for(unsigned b=0;b<3;b++)for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)printf("%02x%02x",q.attributes[b][i][j]&255,q.attributes[b][i][j]>>8);
  for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)printf("%02x%02x",q.depth[i][j]&255,q.depth[i][j]>>8);printf(" %u ",t.matrix_count);const unsigned char *b=(const unsigned char *)t.matrices;for(unsigned i=0;i<sizeof t.matrices;i++)printf("%02x",b[i]);putchar('\n');
 }return 0;}
