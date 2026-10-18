/* Actual captured RAM plus original angle table; diagnostic only. */
#include "../src/traction.h"
#include <stdio.h>
#include <string.h>
static unsigned char raw[0xe88+0x980+4+8+32768+514];
static TaxiTrig trig;
static uint32_t u32(const unsigned char*p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int32_t s32(const unsigned char*p){uint32_t u=u32(p);int32_t v;memcpy(&v,&u,4);return v;}
static void put32(unsigned char*p,int32_t v){uint32_t u;memcpy(&u,&v,4);for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(u>>(i*8));}
int main(int argc,char**argv){
 if(argc!=3)return 1;
 FILE*f=fopen(argv[1],"rb");if(!f)return 2;
 size_t n=fread(raw,1,sizeof raw,f);int extra=fgetc(f),bad=ferror(f);fclose(f);
 if(n!=sizeof raw||extra!=EOF||bad)return 3;
 unsigned char*g=raw+0xe88,*d=g+0x980,*l=d+4;const unsigned char*t=l+8;
 TaxiTraction v={0};TaxiTractionContext c={0};TaxiOrientationEffects e;TaxiAngleTable table;
 v.force_53c=s32(raw+0x53c);v.force_540=s32(raw+0x540);
 v.capacity_570=s32(raw+0x570);v.capacity_574=s32(raw+0x574);
 v.speed_544=s32(raw+0x544);v.state_564=s32(raw+0x564);
 memcpy(&v.phase_e6a,raw+0xe6a,1);memcpy(&v.phase_e6b,raw+0xe6b,1);v.flags_28=raw[0x28];
 c.settings=g[0x1eb0-0x1534];c.flag_1696=g[0x1696-0x1534];c.flag_1697=g[0x1697-0x1534];
 c.flag_25d4=d[0];c.flag_262c=l[0];c.state_2630=s32(l+4);
 for(unsigned i=0;i<4096;i++){trig.sine[i]=s32(t+4*i);trig.cosine[i]=s32(t+16384+4*i);}
 taxi_angle_table_decode(&table,t+32768);
 int rc=taxi_traction_step(&v,&c,&trig,&table,&e);if(rc)return 7;
 put32(raw+0x544,v.speed_544);put32(raw+0x564,v.state_564);d[0]=c.flag_25d4;l[0]=c.flag_262c;
 f=fopen(argv[2],"wb");if(!f)return 4;
 n=fwrite(raw,1,sizeof raw,f);bad=ferror(f);int closed=fclose(f);
 if(n!=sizeof raw||bad||closed)return 5;
 printf("%d %u %u %u\n",e.score_requested,e.score_a,e.score_b,e.score_c);
 return ferror(stdout)?6:0;
}
