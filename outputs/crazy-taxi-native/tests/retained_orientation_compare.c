/* Actual captured RAM inputs only; this helper is not a game runtime. */
#include "../src/orientation.h"
#include <stdio.h>
#include <string.h>
static unsigned char raw[0xe88+0x980+4+32768];
static TaxiTrig trig;
static uint32_t u32(const unsigned char*p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int32_t s32(const unsigned char*p){uint32_t u=u32(p);int32_t v;memcpy(&v,&u,4);return v;}
static uint16_t u16(const unsigned char*p){return (uint16_t)(p[0]|((uint16_t)p[1]<<8));}
static void put32(unsigned char*p,int32_t v){uint32_t u;memcpy(&u,&v,4);for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(u>>(i*8));}
static void put16(unsigned char*p,uint16_t v){p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8);}
int main(int argc,char**argv){
 if(argc!=3)return 1;
 FILE*f=fopen(argv[1],"rb");if(!f)return 2;
 size_t n=fread(raw,1,sizeof raw,f);int extra=fgetc(f),bad=ferror(f);fclose(f);
 if(n!=sizeof raw||extra!=EOF||bad)return 3;
 unsigned char*g=raw+0xe88;const unsigned char*t=g+0x980+4;
 TaxiOrientation v={0};TaxiOrientationContext c={0};TaxiOrientationEffects e;
 v.position=(TaxiPoint){s32(raw),s32(raw+4),s32(raw+8)};
 v.angle_18=u16(raw+0x18);v.angle_1a=u16(raw+0x1a);
 v.heading_1c=s32(raw+0x1c);v.heading_20=s32(raw+0x20);
 v.drift_69c=s32(raw+0x69c);v.force_540=s32(raw+0x540);
 v.state_564=s32(raw+0x564);v.speed_544=s32(raw+0x544);
 uint16_t timer=u16(raw+0x2a);memcpy(&v.timer_2a,&timer,2);
 v.flags_358=raw[0x358];v.flags_28=raw[0x28];memcpy(&v.phase_e6a,raw+0xe6a,1);
 c.delta=s32(g+0x980);c.settings=g[0x1eb0-0x1534];c.submode=g[3];
 c.last_heading_1658=s32(g+0x1658-0x1534);c.amount_1654=s32(g+0x1654-0x1534);
 c.counter_1650=u16(g+0x1650-0x1534);c.counter_164c=u16(g+0x164c-0x1534);
 c.flag_1697=g[0x1697-0x1534];c.flag_1ea8=g[0x1ea8-0x1534];
 c.flag_177b=g[0x177b-0x1534];c.flag_1694=g[0x1694-0x1534];
 for(unsigned i=0;i<4096;i++){trig.sine[i]=s32(t+4*i);trig.cosine[i]=s32(t+16384+4*i);}
 taxi_orientation_step(&v,&c,&trig,&e);
 put32(raw,v.position.x);put32(raw+4,v.position.y);put32(raw+8,v.position.z);
 put32(raw+0x1c,v.heading_1c);put32(raw+0x20,v.heading_20);
 put32(raw+0x69c,v.drift_69c);put32(raw+0x544,v.speed_544);
 memcpy(&timer,&v.timer_2a,2);put16(raw+0x2a,timer);
 put32(g+0x1658-0x1534,c.last_heading_1658);put32(g+0x1654-0x1534,c.amount_1654);
 put16(g+0x1650-0x1534,c.counter_1650);put16(g+0x164c-0x1534,c.counter_164c);
 f=fopen(argv[2],"wb");if(!f)return 4;
 size_t written=fwrite(raw,1,sizeof raw,f);int close_result=fclose(f);
 if(written!=sizeof raw||close_result)return 5;
 printf("%d %u %u %u\n",e.score_requested,e.score_a,e.score_b,e.score_c);
 return ferror(stdout)?6:0;
}
