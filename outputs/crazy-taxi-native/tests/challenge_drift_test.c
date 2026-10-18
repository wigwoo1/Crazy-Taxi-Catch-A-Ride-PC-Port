#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/challenge_drift.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 TaxiChallengeDrift s={9};TaxiOrientationContext o={.delta=3,.settings=128,.submode=3,.counter_164c=17,.counter_1650=22,.amount_1654=123,.last_heading_1658=321};uint8_t option=99;
 assert(!taxi_challenge_drift_begin(&s,&o,&option)&&s.remaining_164e==1800&&option==4);
 assert(!o.counter_164c&&!o.counter_1650&&!o.amount_1654&&!o.last_heading_1658&&o.delta==3&&o.settings==128&&o.submode==3);
 TaxiOrientationContext old=o;assert(taxi_challenge_drift_begin(&s,&o,NULL)==-1&&s.remaining_164e==1800&&!memcmp(&old,&o,sizeof o));
 TaxiChallengeDriftInput in={.delta=1,.effects_enabled=1};TaxiChallengeDriftStatus status={0};TaxiChallengeDriftEffects effects;
 for(unsigned count=0;count<65536;count++){
  s.remaining_164e=1;status=(TaxiChallengeDriftStatus){0};in.count164c=(uint16_t)count;taxi_challenge_drift_step(&s,&in,&status,&effects);
  assert(s.remaining_164e==0&&status.ended1697&&status.success153c==(count>=15&&count<32768)&&effects.request_rank==status.success153c);
 }
 in.count164c=15;s.remaining_164e=2;status=(TaxiChallengeDriftStatus){0};taxi_challenge_drift_step(&s,&in,&status,&effects);
 assert(s.remaining_164e==1&&!status.ended1697&&!status.success153c&&!effects.request_rank);
 taxi_challenge_drift_step(&s,&in,&status,&effects);assert(status.success153c&&effects.request_rank);
 s.remaining_164e=600;status=(TaxiChallengeDriftStatus){0};taxi_challenge_drift_step(&s,&in,&status,&effects);assert(s.remaining_164e==599&&effects.warning_palette&&effects.sound_index==39);
 taxi_challenge_drift_step(&s,&in,&status,&effects);assert(!effects.warning_palette&&effects.sound_index==-1);
 in.effects_enabled=0;s.remaining_164e=600;taxi_challenge_drift_step(&s,&in,&status,&effects);assert(effects.warning_palette&&effects.sound_index==-1);
 in=(TaxiChallengeDriftInput){.delta=1800,.count164c=14};s.remaining_164e=1800;status=(TaxiChallengeDriftStatus){0};taxi_challenge_drift_step(&s,&in,&status,&effects);assert(!status.success153c&&status.ended1697&&effects.warning_palette&&!effects.request_rank);
 for(unsigned gate=0;gate<6;gate++){
  in=(TaxiChallengeDriftInput){.delta=1};status=(TaxiChallengeDriftStatus){0};s.remaining_164e=1800;
  if(gate==0)in.outer1ea8=1;if(gate==1)in.intro1696=1;if(gate==2)in.pause5f20=1;if(gate==3)status.ended1697=1;if(gate==4)status.success153c=1;if(gate==5)in.phase153a=2;
  taxi_challenge_drift_step(&s,&in,&status,&effects);assert(s.remaining_164e==1800&&!effects.request_rank&&!effects.warning_palette);
 }
 in=(TaxiChallengeDriftInput){.phase153a=1,.outer1ea8=1};taxi_challenge_drift_step(&s,&in,&status,&effects);assert(effects.render_phase1&&s.remaining_164e==1800);
 /* Explicit heading fixtures exercise the recovered counter producer;
    these are not earned driving or a playable course. */
 TaxiTrig trig={0};trig.cosine[0]=256;TaxiOrientation car={.flags_358=4,.phase_e6a=2,.state_564=1};TaxiOrientationEffects oe;
 o=(TaxiOrientationContext){.settings=128,.submode=3};assert(!taxi_challenge_drift_begin(&s,&o,&option));
 for(unsigned n=1;n<=30;n++){
  car.heading_1c=(int32_t)((n*1024u)&4095u)*4096;
  taxi_orientation_step(&car,&o,&trig,&oe);
  assert(o.counter_1650==n&&o.counter_164c==n/2&&!oe.score_requested);
 }
 in=(TaxiChallengeDriftInput){.delta=1,.count164c=o.counter_164c};status=(TaxiChallengeDriftStatus){0};
 taxi_challenge_drift_step(&s,&in,&status,&effects);assert(s.remaining_164e==1799&&!status.success153c);
 s.remaining_164e=1;taxi_challenge_drift_step(&s,&in,&status,&effects);assert(status.success153c&&effects.request_rank);
 puts("PASS original Drift30s/init/count65536signedboundaries/timeout-only success/warning/gates; not a playable course or independent GBA oracle.");return 0;
}
