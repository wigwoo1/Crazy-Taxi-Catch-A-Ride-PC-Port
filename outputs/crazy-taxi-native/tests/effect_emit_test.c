#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "effect_emit.h"
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int main(void){
 TaxiEffectPool pool;memset(&pool,0xa5,sizeof pool);unsigned ring=8;
 TaxiBoostEffect s={.scale=4710,.delta=20,.clock=60,.action2=1,
 .attachment_tags={0x3f8,0x408},.velocity_basis={{-1,2,3},{4,-5,6}}};
 assert(taxi_effect_emit_boost(&pool,&ring,&s)==2);
 assert(ring==1&&s.timer==-5); /* Overshoot retained, not reset to 15. */
 assert(word(pool.records[8].bytes)==0x3f8&&word(pool.records[0].bytes)==0x408);
 assert(pool.records[8].bytes[0x34]==5&&pool.records[8].bytes[0x35]==0);
 assert(word(pool.records[8].bytes+0x24)==(uint32_t)-40);
 assert(word(pool.records[0].bytes+0x28)==(uint32_t)-200);
 assert(word(pool.records[8].bytes+0x3c)==0x08021959);
 assert(pool.records[8].bytes[0x37]==64&&pool.records[8].bytes[0x3a]==60);
 assert(word(pool.records[8].bytes+4)==0xa5a5a5a5u);
 assert(pool.records[8].bytes[0x38]==0xa5&&pool.records[8].bytes[0x44]==0xa5);
 TaxiEffectPool saved=pool;
 for(unsigned gate=0;gate<5;gate++){
  TaxiBoostEffect gated=s;gated.timer=7;
  if(gate==0)gated.actor_flags=8;
  if(gate==1)gated.material_flags=2;
  if(gate==2)gated.alternate=255;
  if(gate==3)gated.scale=4096;
  if(gate==4)gated.action2=0;
  assert(taxi_effect_emit_boost(&pool,&ring,&gated)==0);
  assert(gated.timer==7&&ring==1&&memcmp(&pool,&saved,sizeof pool)==0);
 }
 s.timer=10;s.delta=1;
 assert(taxi_effect_emit_boost(&pool,&ring,&s)==0&&s.timer==9);
 assert(memcmp(&pool,&saved,sizeof pool)==0);
 s.timer=1;s.clock=-7;
 assert(taxi_effect_emit_boost(&pool,&ring,&s)==2&&s.timer==-1);
 ring=9;s.timer=10;
 assert(taxi_effect_emit_boost(&pool,&ring,&s)==-1&&s.timer==10);
 ring=8;TaxiSmokeEffect smoke={.common={.scale=4096,.delta=1,.clock=60,
 .attachment_tags={2,3},.velocity_basis={{1,2,-3},{4,5,6}}},.speed=-5*1138,.voice_25d4=1};
 assert(taxi_effect_emit_smoke(&pool,&ring,&smoke)==2&&ring==1&&smoke.common.timer==14);
 assert(pool.records[8].bytes[0x34]==1&&word(pool.records[8].bytes+0x3c)==0x080218ad);
 assert(word(pool.records[8].bytes+0x24)==(uint32_t)-18); /* ASR(-5,2)*9 */
 smoke.common.timer=0;smoke.common.actor_flags=8;smoke.common.material_flags=6;smoke.common.alternate=1;
 smoke.common.scale=4710;smoke.voice_25d4=0;smoke.state_2630=1;smoke.phase_e6a=1;
 assert(taxi_effect_emit_smoke(&pool,&ring,&smoke)==2);
 assert(word(pool.records[1].bytes+0x24)==(uint32_t)-5); /* (-5*7*512)>>12 */
 saved=pool;smoke.phase_e6a=2;int32_t timer=smoke.common.timer;unsigned previous=ring;
 assert(taxi_effect_emit_smoke(&pool,&ring,&smoke)==0);
 assert(ring==previous&&smoke.common.timer==timer&&memcmp(&saved,&pool,sizeof pool)==0);
 smoke.common.actor_flags=smoke.common.material_flags=smoke.common.alternate=0;
 smoke.state_2630=0;smoke.voice_25d4=1;
 assert(taxi_effect_emit_smoke(&pool,&ring,&smoke)==0); /* Scale above4096 gates normal branch. */
 memset(&pool,0,sizeof pool);for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)pool.slots[g][j]=-1;
 ring=8;TaxiSurfaceEffect surface={.material_flags=6,.speed=54901,.clock=60,
 .attachment_tags={2,3,4},.velocity_basis={{-1,2,3},{4,5,6},{7,8,9}}};
 assert(taxi_effect_emit_surface(&pool,&ring,2,&surface)==6&&ring==5&&surface.timer==7);
 assert(pool.slots[0][0]==8&&pool.slots[0][1]==0&&pool.slots[0][5]==4);
 assert(word(pool.records[8].bytes+0x24)==UINT32_MAX);
 assert(word(pool.records[8].bytes+0x3c)==0x08021a05);
 assert(taxi_effect_emit_surface(&pool,&ring,2,&surface)==0&&surface.timer==14);
 pool.slots[0][0]=-1;unsigned retained=pool.slots[0][1];
 assert(taxi_effect_emit_surface(&pool,&ring,2,&surface)==2);
 assert(pool.slots[0][0]==5&&pool.slots[0][1]==6&&pool.slots[0][1]!=(int)retained);
 assert(taxi_effect_emit_surface(&pool,&ring,3,&surface)==6);
 assert(word(pool.records[7].bytes+0x3c)==0x08021ab1);
 assert(word(pool.records[2].bytes+0x3c)==0x08021a05); /* Third pair, shared callback. */
 saved=pool;int32_t original_timer=surface.timer;surface.speed=INT32_MIN;
 assert(taxi_effect_emit_surface(&pool,&ring,2,&surface)==0&&surface.timer==original_timer);
 assert(memcmp(&saved,&pool,sizeof pool)==0);
 surface.speed=54900;
 assert(taxi_effect_emit_surface(&pool,&ring,2,&surface)==0);
 pool.slots[0][0]=9;
 assert(taxi_effect_emit_surface(&pool,&ring,2,&surface)==-1);
 puts("Boost effect emitter: gates, paired ring overwrite, retained fields, velocity and overshoot PASS");
 return 0;
}
