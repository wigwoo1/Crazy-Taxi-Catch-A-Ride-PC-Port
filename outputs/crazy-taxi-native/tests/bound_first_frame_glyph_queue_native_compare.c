#include "taxi_affine_queue.h"
#include "obj_queue_owned.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int main(int argc,char **argv){
 if(argc!=3)return 1;FILE *f=fopen(argv[1],"rb"),*r=fopen(argv[2],"rb");if(!f||!r)return 2;
 uint32_t c[21];uint8_t state[4490];if(fread(c,1,sizeof c,f)!=sizeof c||fread(state,1,sizeof state,f)!=sizeof state)return 3;fclose(f);
 uint8_t *rom=malloc(8388608);if(!rom||fread(rom,1,8388608,r)!=8388608)return 4;fclose(r);
 TaxiStartupObjQueue q={.total=state[3],.count={state[6],state[7],state[8]},.last_width=state[0],.last_height=state[1]};TaxiStartupObjTransport t={.matrix_count=state[5]};assert(sizeof t.matrices==384);
 memcpy(t.matrices,state+10,384);memcpy(q.attributes,state+394,3072);memcpy(q.depth,state+3466,1024);TaxiStartupObjQueued out={0};int result;
 if(c[0]==0x08028658){
  if(c[9]<0x08000000||c[9]>=0x08800000||c[2]!=3)return 5;uint32_t resource=c[9]-0x08000000;uint32_t group=resource+word(rom+resource+4+c[2]*4);if(group+20>8388608||c[1]>=word(rom+group)||word(rom+group+4)!=16)return 6;
  uint32_t metric=resource+word(rom+group+20+c[1]*4);if(metric+18>8388608)return 7;uint16_t m[9];for(unsigned i=0;i<9;i++)m[i]=(uint16_t)(rom[metric+i*2]|(unsigned)rom[metric+i*2+1]<<8);
  result=taxi_affine_queue(&q,&t,m,(int32_t)c[15],(int32_t)c[16],(int32_t)c[17],(uint16_t)c[13],(uint16_t)c[14],(uint8_t)c[4],(uint8_t)c[3],(uint8_t)c[5],(uint16_t)c[12],(c[10]+0xf9ff0000u)>>5,(int32_t)c[11],(uint8_t)c[6],(int16_t)c[7],(int16_t)c[8],(int32_t)c[18],(int32_t)c[19],&out);
 }else if(c[0]==0x08028288){uint32_t bases[4]={0,0,0,c[10]};int32_t palettes[4]={0,0,0,(int32_t)c[11]};TaxiObjQueueOwner owner={rom,8388608,c[9],bases,palettes,4,&q,(uint16_t)c[13],(uint16_t)c[14],(uint16_t)c[12]};result=taxi_obj_queue_owned(&owner,c[1],c[2],c+15,c[3],c[4],c[5],c[6],&out);
 }else return 8;
 if(result<0)return 9;uint32_t pointer=result?0x020081ecu+out.bucket*1024u+out.index*8u:0;if(pointer!=c[20])return 10;
 state[0]=q.last_width;state[1]=q.last_height;state[3]=q.total;state[5]=t.matrix_count;for(unsigned i=0;i<3;i++)state[6+i]=q.count[i];memcpy(state+10,t.matrices,384);memcpy(state+394,q.attributes,3072);memcpy(state+3466,q.depth,1024);free(rom);
 if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 11;return fwrite(state,1,sizeof state,stdout)==sizeof state?0:12;
}
