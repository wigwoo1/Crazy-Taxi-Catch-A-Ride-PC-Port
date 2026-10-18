#include "challenge_bound.h"
#include <string.h>
int taxi_challenge_bound_begin(TaxiChallengeBound *s,uint8_t *o){if(!s||!o)return -1;s->remaining_1660=2400;s->failure_latch165e=0;*o=4;return 0;}
void taxi_challenge_bound_step(TaxiChallengeBound *s,const TaxiChallengeBoundInput *i,TaxiChallengeBoundStatus *status,TaxiChallengeBoundEffects *e){
 *e=(TaxiChallengeBoundEffects){.sound_index={-1,-1,-1}};
 if(i->phase153a){if(i->phase153a==1)e->render_phase1=1;return;}
 if(i->outer1ea8||i->pause5f20)return;
 if(!status->success153c&&!i->intro1696&&!status->ended1697){
  int before=s->remaining_1660/60;uint16_t bits=(uint16_t)((uint32_t)(uint16_t)s->remaining_1660-(uint32_t)i->delta);memcpy(&s->remaining_1660,&bits,2);
  if(s->remaining_1660<=0){s->remaining_1660=0;status->success153c=0;status->ended1697=1;}
  if(before>9&&s->remaining_1660/60<=9){e->warning_palette=1;if(i->effects_enabled)e->sound_index[e->sound_count++]=39;}
 }
 /* Source first comparison falls straight out after marking failure. Latch is next-frame. */
 if(i->vehicle_y2638<=639&&!status->ended1697){status->success153c=0;status->ended1697=1;return;}
 if(i->vehicle_y2638<=25&&status->ended1697&&!s->failure_latch165e){s->failure_latch165e=1;e->failure1534=1;e->failure1535=-1;if(i->effects_enabled){e->sound_index[e->sound_count++]=78;e->sound_index[e->sound_count++]=46;}}
}
