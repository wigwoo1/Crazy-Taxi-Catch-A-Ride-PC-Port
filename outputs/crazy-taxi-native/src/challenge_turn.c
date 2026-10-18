#include "challenge_turn.h"
#include <string.h>
static int16_t half(uint16_t v){int16_t result;memcpy(&result,&v,2);return result;}
int taxi_challenge_turn_begin(TaxiChallengeTurn *s,uint8_t *option){if(!s||!option)return -1;s->remaining_165c=2100;*option=4;return 0;}
void taxi_challenge_turn_step(TaxiChallengeTurn *s,const TaxiChallengeTurnInput *in,TaxiChallengeTurnStatus *status,TaxiChallengeTurnEffects *e){
 *e=(TaxiChallengeTurnEffects){.sound_index=-1};
 if(in->phase153a){if(in->phase153a==1)e->render_phase1=1;return;}
 if(in->outer1ea8||status->success153c||in->intro1696||status->ended1697||in->pause5f20)return;
 int before=s->remaining_165c/60;
 s->remaining_165c=half((uint16_t)((uint32_t)(uint16_t)s->remaining_165c-(uint32_t)in->delta));
 if(s->remaining_165c<=0){s->remaining_165c=0;status->success153c=0;status->ended1697=1;}
 if(before>9&&s->remaining_165c/60<=9){e->warning_palette=1;if(in->effects_enabled)e->sound_index=39;}
}
