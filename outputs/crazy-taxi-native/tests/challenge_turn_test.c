#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "challenge_turn.h"
int main(void){
 TaxiChallengeTurn s;uint8_t option=99;assert(!taxi_challenge_turn_begin(&s,&option)&&s.remaining_165c==2100&&option==4);
 TaxiChallengeTurnInput in={.delta=1,.effects_enabled=1};TaxiChallengeTurnStatus status={0};TaxiChallengeTurnEffects e;unsigned warnings=0;
 for(unsigned i=0;i<2100;i++){taxi_challenge_turn_step(&s,&in,&status,&e);warnings+=e.warning_palette;if(i<2099)assert(!status.ended1697);}
 assert(!s.remaining_165c&&status.ended1697&&!status.success153c&&warnings==1);
 for(int value=-32768;value<=32767;value++){
  s.remaining_165c=(int16_t)value;status=(TaxiChallengeTurnStatus){0};taxi_challenge_turn_step(&s,&in,&status,&e);
  int after=value==-32768?32767:value-1;assert(s.remaining_165c==(after>0?after:0));assert(status.ended1697==(after<=0));assert(!status.success153c);
  assert(e.warning_palette==((value/60)>9&&(after<=0?0:after/60)<=9));
 }
 for(unsigned gate=0;gate<5;gate++){
  s.remaining_165c=2100;status=(TaxiChallengeTurnStatus){0};in=(TaxiChallengeTurnInput){.delta=50};
  if(gate==0)in.outer1ea8=1;if(gate==1)status.success153c=1;if(gate==2)in.intro1696=1;if(gate==3)status.ended1697=1;if(gate==4)in.pause5f20=1;
  TaxiChallengeTurnStatus before=status;taxi_challenge_turn_step(&s,&in,&status,&e);assert(s.remaining_165c==2100&&!memcmp(&status,&before,sizeof status)&&!e.warning_palette&&e.sound_index==-1);
 }
 in=(TaxiChallengeTurnInput){.delta=1};status=(TaxiChallengeTurnStatus){0};s.remaining_165c=600;taxi_challenge_turn_step(&s,&in,&status,&e);assert(e.warning_palette&&e.sound_index==-1);
 for(unsigned phase=1;phase<256;phase++){in.phase153a=(uint8_t)phase;s.remaining_165c=2100;taxi_challenge_turn_step(&s,&in,&status,&e);assert(s.remaining_165c==2100&&e.render_phase1==(phase==1));}
 s.remaining_165c=77;option=88;assert(taxi_challenge_turn_begin(&s,0)==-1&&s.remaining_165c==77&&option==88);assert(taxi_challenge_turn_begin(0,&option)==-1&&option==88);
 puts("PASS original Crazy Turn scalar:2100tick timeout/onewarning,65536signedtimer wrap/boundaries,255phases/allgates/audiooff/invalidinitretention. Guidance/world/success/course/viewer dependencies pending.");return 0;
}
