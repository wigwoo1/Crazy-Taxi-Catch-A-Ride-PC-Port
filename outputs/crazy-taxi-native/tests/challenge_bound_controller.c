#include "../src/challenge_bound.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL line%d: %s\n",__LINE__,#x);exit(1);}}while(0)
int main(void){
 TaxiChallengeBound s={0};uint8_t option=0;TaxiChallengeBoundInput in={.delta=1,.vehicle_y2638=640,.effects_enabled=1};TaxiChallengeBoundStatus st={0};TaxiChallengeBoundEffects e;
 CHECK(taxi_challenge_bound_begin(&s,&option)==0&&s.remaining_1660==2400&&!s.failure_latch165e&&option==4);
 taxi_challenge_bound_step(&s,&in,&st,&e);CHECK(s.remaining_1660==2399&&!st.ended1697&&!e.sound_count);
 in.vehicle_y2638=639;taxi_challenge_bound_step(&s,&in,&st,&e);CHECK(st.ended1697&&!st.success153c&&!s.failure_latch165e);
 in.vehicle_y2638=26;taxi_challenge_bound_step(&s,&in,&st,&e);CHECK(!s.failure_latch165e);
 in.vehicle_y2638=25;taxi_challenge_bound_step(&s,&in,&st,&e);CHECK(s.failure_latch165e&&e.failure1534&&e.failure1535==-1&&e.sound_count==2&&e.sound_index[0]==78&&e.sound_index[1]==46);
 taxi_challenge_bound_step(&s,&in,&st,&e);CHECK(!e.failure1534&&!e.sound_count);
 /* Warning and terminal latch can occur in the same call; all3 source sounds retained. */
 s=(TaxiChallengeBound){.remaining_1660=600};st=(TaxiChallengeBoundStatus){0};in.delta=600;
 taxi_challenge_bound_step(&s,&in,&st,&e);CHECK(!s.remaining_1660&&st.ended1697&&e.warning_palette&&e.sound_count==3&&e.sound_index[0]==39&&e.sound_index[1]==78&&e.sound_index[2]==46);
 for(unsigned gate=0;gate<4;gate++){
  s=(TaxiChallengeBound){.remaining_1660=600};st=(TaxiChallengeBoundStatus){0};in=(TaxiChallengeBoundInput){.delta=1,.vehicle_y2638=0};
  if(gate==0)in.phase153a=1;else if(gate==1)in.phase153a=2;else if(gate==2)in.outer1ea8=1;else in.pause5f20=1;
  taxi_challenge_bound_step(&s,&in,&st,&e);CHECK(s.remaining_1660==600&&!st.ended1697&&!s.failure_latch165e&&e.render_phase1==(gate==0));
 }
 for(unsigned gate=0;gate<2;gate++){
  s=(TaxiChallengeBound){.remaining_1660=600};st=(TaxiChallengeBoundStatus){.success153c=(unsigned char)(gate==1)};in=(TaxiChallengeBoundInput){.delta=1,.vehicle_y2638=639,.intro1696=(unsigned char)(gate==0)};
  taxi_challenge_bound_step(&s,&in,&st,&e);CHECK(s.remaining_1660==600&&st.ended1697&&!st.success153c);
 }
 puts("PASS Bound timer/639-640 and25-26 thresholds/phase-pause-outer gates/intro-success height branch/once-only latch/three simultaneous source sounds. Native scalar only; no whole GBA oracle.");return 0;
}
