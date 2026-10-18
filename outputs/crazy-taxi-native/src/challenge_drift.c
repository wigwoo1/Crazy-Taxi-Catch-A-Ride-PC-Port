#include "challenge_drift.h"
#include <string.h>
static int16_t half(uint16_t value){int16_t result;memcpy(&result,&value,2);return result;}
int taxi_challenge_drift_begin(TaxiChallengeDrift *s,TaxiOrientationContext *o,uint8_t *option){
 if(!s||!o||!option)return -1;
 s->remaining_164e=1800;o->counter_164c=o->counter_1650=0;o->amount_1654=o->last_heading_1658=0;*option=4;return 0;
}
void taxi_challenge_drift_step(TaxiChallengeDrift *s,const TaxiChallengeDriftInput *in,TaxiChallengeDriftStatus *status,TaxiChallengeDriftEffects *e){
 *e=(TaxiChallengeDriftEffects){.sound_index=-1};
 if(in->phase153a){if(in->phase153a==1)e->render_phase1=1;return;}
 if(in->outer1ea8||status->success153c||in->intro1696||status->ended1697||in->pause5f20)return;
 int before=s->remaining_164e/60;
 s->remaining_164e=half((uint16_t)((uint32_t)(uint16_t)s->remaining_164e-(uint32_t)in->delta));
 if(s->remaining_164e<=0){
  s->remaining_164e=0;status->success153c=(uint8_t)(half(in->count164c)>14);status->ended1697=1;e->request_rank=status->success153c;
 }
 if(before>9&&s->remaining_164e/60<=9){e->warning_palette=1;if(in->effects_enabled)e->sound_index=39;}
}
