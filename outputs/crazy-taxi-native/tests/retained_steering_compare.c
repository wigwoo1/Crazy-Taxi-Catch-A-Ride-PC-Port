/* Consumes actual retained steering snapshots; no synthetic gameplay fields. */
#include "../src/driving.h"
#include <stdio.h>
#include <string.h>
static uint8_t raw[0xe88+16+4+32768];
static TaxiTrig trig;
static uint32_t u32(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int32_t s32(const uint8_t *p){uint32_t u=u32(p);int32_t v;memcpy(&v,&u,4);return v;}
int main(int argc,char **argv){
 if(argc!=2)return 1;
 FILE *f=fopen(argv[1],"rb");if(!f)return 2;
 size_t n=fread(raw,1,sizeof raw,f);int extra=fgetc(f),bad=ferror(f);fclose(f);
 if(n!=sizeof raw||extra!=EOF||bad)return 3;
 TaxiSteeringState v={0};
 v.speed_544=s32(raw+0x544);v.state_564=s32(raw+0x564);
 v.limit_7b0=s32(raw+0x7b0);v.state_7b8=s32(raw+0x7b8);
 v.angle_578=(uint16_t)(raw[0x578]|((uint16_t)raw[0x579]<<8));
 v.flags_28=raw[0x28];v.flags_358=raw[0x358];
 memcpy(&v.phase_e6a,raw+0xe6a,1);memcpy(&v.phase_e6b,raw+0xe6b,1);
 TaxiCommand cmd;memcpy(cmd.bytes,raw+0xe88,16);
 const uint8_t *tables=raw+0xe88+20;
 for(unsigned i=0;i<4096;i++){trig.sine[i]=s32(tables+4*i);trig.cosine[i]=s32(tables+16384+4*i);}
 int32_t response=0;int rc=taxi_steering_response(&v,&cmd,raw[0xe88+16],&trig,&response);
 printf("%d %d %u %d\n",rc,response,(unsigned)v.flags_28,v.state_7b8);
 return ferror(stdout)?4:0;
}
