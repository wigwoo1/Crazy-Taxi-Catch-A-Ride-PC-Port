#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "actor_effects.h"
typedef struct {int queries,randoms,fail;} Hooks;
static void put(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static int height(void *ctx,TaxiPoint p,int32_t *y){Hooks *h=ctx;(void)p;h->queries++;*y=-256;return h->fail?-1:1;}
static int32_t random_value(void *ctx,int32_t lo,int32_t hi){Hooks *h=ctx;unsigned n=(unsigned)h->randoms++%3;
 assert(lo==(n==0?0:n==1?-5:4)&&hi==(n==0?65535:n==1?5:8));return n==0?5:n==1?-3:7;
}
int main(void){
 static TaxiTrig trig;trig.cosine[0]=256;
 TaxiBodyPoints body;assert(taxi_body_points(&body,(TaxiPoint){0,0,1000},0,0,0,0,&trig)==1);
 TaxiActorEffects s;assert(taxi_actor_effects_cold_init(&s)==0);
 Hooks hooks={0};TaxiActorEffectInput input={.material_flags=6,.action2=1,.phase_e6a=1,
 .state_2630=1,.speed=60000,.scale=4710,.delta=1,.clock=60};
 assert(taxi_actor_effects_step(&s,&body,input,height,&hooks)==14);
 assert(s.ring==5&&s.timers[0]==14&&s.timers[1]==7&&s.timers[2]==7&&s.timers[3]==0);
 assert(s.pool.records[0].bytes[0x34]==3&&s.pool.records[5].bytes[0x34]==2&&s.pool.records[8].bytes[0x34]==3);
 assert(s.pool.slots[0][0]==2&&s.pool.slots[1][0]==8); /* Aliased ring references retained. */
 for(unsigned j=0;j<9;j++)assert(s.pool.records[j].bytes[0x3a]==59&&s.pool.records[j].bytes[0x35]==1);
 assert(hooks.queries==0);
 TaxiEffectSprite out[9];assert(taxi_actor_effects_sprites(&s,(TaxiPoint){0},0,0,0,&trig,
 100,120,80,0,0,random_value,&hooks,out)==9&&hooks.randoms==27);
 assert(out[0].palette==1&&out[5].palette==0);
 assert(taxi_actor_effects_session_reset(&s)==0&&s.timers[0]==14&&s.pool.slots[0][0]==2);
 assert(taxi_actor_effects_cold_init(&s)==0);
 input.material_flags=0;input.state_2630=0;input.phase_e6a=0;
 assert(taxi_actor_effects_step(&s,&body,input,height,&hooks)==2&&hooks.queries==2);
 TaxiActorEffects saved=s;hooks.fail=1;
 assert(taxi_actor_effects_step(&s,&body,input,height,&hooks)==-1&&memcmp(&s,&saved,sizeof s)==0);
 assert(taxi_actor_effects_cold_init(&s)==0);s.ring=6;
 for(unsigned j=0;j<6;j++)s.pool.slots[0][j]=(int)j;
 uint8_t *b=s.pool.records[0].bytes;b[0x34]=2;b[0x3a]=1;put(b,2);put(b+0x3c,0x08021a05);
 hooks.fail=0;input.material_flags=2;input.scale=4096;input.action2=0;
 assert(taxi_actor_effects_step(&s,&body,input,height,&hooks)==0);
 assert(!s.pool.records[0].bytes[0x34]&&s.ring==8&&s.timers[1]==14);
 assert(s.pool.slots[0][0]==6&&s.pool.slots[0][1]==7);
 assert(s.pool.records[6].bytes[0x3a]==59&&s.pool.records[7].bytes[0x35]==1);
 s.pool.records[0].bytes[0x34]=6;saved=s;
 assert(taxi_actor_effects_step(&s,&body,input,height,&hooks)==-1&&memcmp(&s,&saved,sizeof s)==0);
 assert(taxi_actor_effects_cold_init(&s)==0);s.body=body;
 assert(taxi_actor_effects_impact(&s,2,0,1,60)==1&&s.impact_timer==6&&s.ring==1);
 assert(s.pool.records[0].bytes[0x34]==4&&s.pool.records[0].bytes[0x3a]==30);
 assert(s.pool.records[0].bytes[0x24]==0&&s.pool.records[0].bytes[0x25]==247); /* -2304 low16. */
 TaxiBodyPoints moved;assert(taxi_body_points(&moved,(TaxiPoint){1000,0,1000},0,0,0,0,&trig)==1);
 input.actor_flags=8;input.material_flags=0;input.scale=4096;input.state_2630=0;
 int queries=hooks.queries;
 assert(taxi_actor_effects_step(&s,&moved,input,height,&hooks)==0&&hooks.queries==queries);
 assert(s.impact_timer==6&&s.pool.records[0].bytes[0x3a]==29);
 assert(taxi_actor_effects_sprites(&s,(TaxiPoint){0},0,0,0,&trig,100,120,80,0,0,NULL,NULL,out)==1);
 assert(out[0].group==10&&out[0].frame==1&&out[0].position.x==249&&out[0].position.z==635);
 assert(taxi_actor_effects_impact(&s,2,1,1,60)==0&&s.impact_timer==6);
 assert(taxi_actor_effects_impact(&s,6,0,1,60)==-1&&s.impact_timer==6);
 assert(taxi_actor_effects_impact(&s,2,0,1,60)==0&&s.impact_timer==5);
 puts("Actor effects: shared ring order/aliasing, live updates, ordered RNG, warm reset and query rollback PASS");
 return 0;
}
