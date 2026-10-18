#include "../src/effect_update.h"
#include "../src/effect_emit.h"
#include <stdio.h>
#include <string.h>
static int events[100],count;static unsigned ring;static int32_t timers[2];static int clock,step_delta;
static void put(unsigned char*p,unsigned v){for(unsigned j=0;j<4;j++)p[j]=(unsigned char)(v>>(j*8));}
static int height(void*c,TaxiPoint p,int32_t*y){(void)c;(void)p;*y=1234;return 1;}
static void cb(void*c,TaxiEffectPool*p,unsigned i,uint32_t tag){(void)c;events[count++]=(int)i;taxi_effect_update_tag(&p->records[i],tag,(TaxiPoint){20,100,300},step_delta,(uint32_t)clock,height,0);}
static void emit(void*c,TaxiEffectPool*p,unsigned type){(void)c;events[count++]=100+(int)type;TaxiSurfaceEffect s={.material_flags=6,.speed=100000,.timer=timers[type-2],.clock=clock,.attachment_tags={2,3,4},.velocity_basis={{-256,256,-3072},{256,256,-3072},{0,256,-2048}}};taxi_effect_emit_surface(p,&ring,type,&s);timers[type-2]=s.timer;}

int main(void){unsigned life,pattern;int delta;while(scanf("%u%d%u%u%d",&life,&delta,&pattern,&ring,&clock)==5){TaxiEffectPool p;memset(&p,0x5a,sizeof p);count=0;step_delta=delta;timers[0]=timers[1]=0;for(unsigned i=0;i<9;i++){p.records[i].bytes[0x34]=(uint8_t)(i%5+1);p.records[i].bytes[0x3a]=(uint8_t)life;p.records[i].bytes[0x3b]=(uint8_t)(life>>8);put(p.records[i].bytes,2);put(p.records[i].bytes+0x3c,(unsigned[]){0x80218ad,0x8021959,0x8021a05,0x8021ab1}[i%4]);put(p.records[i].bytes+0x40,0);}for(unsigned g=0;g<2;g++)for(unsigned i=0;i<6;i++)p.slots[g][i]=(pattern&(1<<i))?(int)i:-1;if(taxi_effect_pool_step(&p,delta,cb,emit,0))return 1;printf("%u %d %d %d",ring,timers[0],timers[1],count);for(int i=0;i<count;i++)printf(" %d",events[i]);for(unsigned g=0;g<2;g++)for(unsigned i=0;i<6;i++)printf(" %d",p.slots[g][i]);for(unsigned i=0;i<648;i++)printf(" %u",((unsigned char*)p.records)[i]);puts("");}return 0;}
