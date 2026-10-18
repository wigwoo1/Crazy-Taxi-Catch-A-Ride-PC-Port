/* Compare native 080126DC motion preparation with one retained original call. */
#include "../src/motion.h"
#include "../src/vehicle_math.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define ACTOR 0xe88
#define GLOBAL 0x980
#define DELTA 4
#define COMMAND 16
#define FLAG25D4 4
#define STATE262C 8
#define TRIG 32768
#define ANGLE 514
#define TOTAL (ACTOR+GLOBAL+DELTA+COMMAND+FLAG25D4+STATE262C+TRIG+ANGLE)
static uint8_t raw[TOTAL];
static uint32_t u32(const uint8_t*p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int32_t s32(const uint8_t*p){uint32_t u=u32(p);int32_t v;memcpy(&v,&u,4);return v;}
static uint16_t u16(const uint8_t*p){return (uint16_t)(p[0]|((uint16_t)p[1]<<8));}
static void p32(uint8_t*p,int32_t v){uint32_t u;memcpy(&u,&v,4);for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(u>>(8*i));}
static void p16(uint8_t*p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
int main(int argc,char**argv){
 if(argc!=3)return 1;
 FILE*f=fopen(argv[1],"rb");if(!f)return 2;
 size_t n=fread(raw,1,sizeof raw,f);int extra=fgetc(f),bad=ferror(f);fclose(f);
 if(n!=sizeof raw||extra!=EOF||bad)return 3;
 uint8_t*a=raw,*g=a+ACTOR,*d=g+GLOBAL,*cmdbytes=d+DELTA,*t=cmdbytes+COMMAND+FLAG25D4+STATE262C;
 TaxiTrig trig;for(unsigned i=0;i<4096;i++){trig.sine[i]=s32(t+4*i);trig.cosine[i]=s32(t+16384+4*i);}
 TaxiCommand command;memcpy(command.bytes,cmdbytes,sizeof command.bytes);
 TaxiMotionState v={0};TaxiOrientation*o=&v.orientation;TaxiSteeringState*s=&v.steering;
 o->position=(TaxiPoint){s32(a),s32(a+4),s32(a+8)};
 o->angle_18=u16(a+0x18);o->angle_1a=u16(a+0x1a);o->heading_1c=s32(a+0x1c);o->heading_20=s32(a+0x20);
 o->drift_69c=s32(a+0x69c);o->force_540=s32(a+0x540);o->state_564=s32(a+0x564);o->speed_544=s32(a+0x544);
 uint16_t timer=u16(a+0x2a);memcpy(&o->timer_2a,&timer,2);o->flags_358=a[0x358];o->flags_28=a[0x28];memcpy(&o->phase_e6a,a+0xe6a,1);
 s->speed_544=o->speed_544;s->state_564=o->state_564;s->limit_7b0=s32(a+0x7b0);s->state_7b8=s32(a+0x7b8);
 s->angle_578=u16(a+0x578);s->flags_28=o->flags_28;s->flags_358=o->flags_358;memcpy(&s->phase_e6a,a+0xe6a,1);memcpy(&s->phase_e6b,a+0xe6b,1);
 v.scale_7c8=s32(a+0x7c8);v.scale_e84=s32(a+0xe84);v.dimension_594=s32(a+0x594);
 v.response_57c=(int16_t)u16(a+0x57c);v.displacement=(TaxiPoint){s32(a+0xc),s32(a+0x10),s32(a+0x14)};
 TaxiOrientationContext c={0};
 c.delta=s32(d);c.last_heading_1658=s32(g+0x1658-0x1534);c.amount_1654=s32(g+0x1654-0x1534);
 c.counter_1650=u16(g+0x1650-0x1534);c.counter_164c=u16(g+0x164c-0x1534);
 c.settings=g[0x1eb0-0x1534];c.submode=g[3];c.flag_1697=g[0x1697-0x1534];
 c.flag_1ea8=g[0x1ea8-0x1534];c.flag_177b=g[0x177b-0x1534];c.flag_1694=g[0x1694-0x1534];
 TaxiOrientationEffects effects={0};int rc=taxi_motion_prepare(&v,&c,&command,&trig,&effects);if(rc)return 10;
 p32(a,o->position.x);p32(a+4,o->position.y);p32(a+8,o->position.z);
 p32(a+0xc,v.displacement.x);p32(a+0x10,v.displacement.y);p32(a+0x14,v.displacement.z);
 p16(a+0x18,o->angle_18);p16(a+0x1a,o->angle_1a);p32(a+0x1c,o->heading_1c);p32(a+0x20,o->heading_20);
 p16(a+0x2a,(uint16_t)o->timer_2a);a[0x28]=o->flags_28;a[0x358]=o->flags_358;p32(a+0x544,o->speed_544);
 p32(a+0x540,o->force_540);p32(a+0x564,o->state_564);p16(a+0x57c,(uint16_t)v.response_57c);
 p16(a+0x578,s->angle_578);p32(a+0x7b0,s->limit_7b0);p32(a+0x7b8,s->state_7b8);p32(a+0x69c,o->drift_69c);
 memcpy(a+0xe6a,&o->phase_e6a,1);memcpy(a+0xe6b,&s->phase_e6b,1);
 p32(g+0x1658-0x1534,c.last_heading_1658);p32(g+0x1654-0x1534,c.amount_1654);
 p16(g+0x1650-0x1534,c.counter_1650);p16(g+0x164c-0x1534,c.counter_164c);
 f=fopen(argv[2],"wb");if(!f)return 4;n=fwrite(raw,1,sizeof raw,f);bad=ferror(f);int closed=fclose(f);
 if(n!=sizeof raw||bad||closed)return 5;
 return 0;
}
