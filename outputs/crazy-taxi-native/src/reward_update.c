#include "reward_update.h"
#include <string.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t word(const uint8_t *p){return bits(p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24));}
static void put(uint8_t *p,int32_t v){for(unsigned j=0;j<4;j++)p[j]=(uint8_t)((uint32_t)v>>(8*j));}
static int sb(uint8_t b){return b<128?b:(int)b-256;}
int taxi_reward_update_allowed(uint8_t paused,uint8_t intro,uint8_t ended,uint8_t mode){return !paused&&!intro&&!ended&&!mode;}
int taxi_reward_update(TaxiRewardState *s,TaxiRewardClock *clock,TaxiFareState *f,TaxiCashGlyphs *g,int32_t delta,uint8_t sound,TaxiRewardUpdateEffects *out){
 if(!s||!clock||!f||!g||!out)return -1;
 TaxiRewardUpdateEffects e={0};uint32_t elapsed=(uint32_t)clock->clock_9aa5+(uint32_t)delta;
 clock->clock_9aa5=(uint8_t)elapsed;
 if(sb((uint8_t)elapsed)>39){clock->clock_9aa5=(uint8_t)(elapsed-40);s->gate_9aa4=1;}else s->gate_9aa4=0;
 clock->active_count_9aa7=0;
 for(unsigned i=0;i<10;i++){
  uint8_t *p=s->tips[i];if(!sb(p[2]))continue;
  clock->active_count_9aa7++;
  const unsigned positions[4]={32,36,8,12},velocities[4]={44,48,20,24};
  for(unsigned j=0;j<4;j++)put(p+positions[j],bits((uint32_t)word(p+positions[j])+(uint32_t)word(p+velocities[j])*(uint32_t)delta));
  p[2]=(uint8_t)((uint32_t)p[2]-(uint32_t)delta);
  if(sb(p[2])>0)continue;
  p[2]=0;put(p+32,192*256);put(p+36,148*256);put(p+8,112*256);put(p+12,16*256);
  f->pending_whole_16a0=bits((uint32_t)f->pending_whole_16a0+(uint32_t)word(p+4));
  f->pending_fraction_16a4=bits((uint32_t)f->pending_fraction_16a4+(uint32_t)sb(p[1]));
  taxi_fare_normalize_pending(f);
  /* C540mode2 updates pending targets only. Preserve unrelated total targets. */
  uint16_t total[10];memcpy(total,g->total_target,sizeof total);taxi_cash_glyphs_targets(g,f);memcpy(g->total_target,total,sizeof total);
  e.expired++;if(sound)e.sound_requests++;
 }
 *out=e;return 0;
}

int taxi_normal_movement_allowed(int32_t gate,int32_t pending){return !gate&&!pending;}
