#include "../src/trip_end.h"
#include "../src/challenge_delivery.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int32_t word(const uint8_t *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t v;memcpy(&v,&u,4);return v;}
int main(void){
 for(unsigned success=0;success<256;success++)for(unsigned count=0;count<256;count++){
  TaxiTripStartState t={.active_customer=0,.direction_753c=44,.time_16ac=1234,.initial_time_16b0=5678,.allowance_16e0=77,.scale_5310=1,.scale_5312=2,.phase_5314=3,.active_1694=1,.active_1695=1,.voice_25d6=99};
  TaxiTripEndState e={.result_16d0=3,.phase_16d1=2,.completed_1692=(uint8_t)count,.flag_9aa6=1,.result_timer_16d2=88,.bonus_16e8=120,.bonus_display_timer_16e4=444,.displayed_bonus_16ec=222};
  TaxiPassengerState p;memset(&p,0xa5,sizeof p);TaxiTripEndEffects effects;
  TaxiTripEndInput in={.settings_0=128,.success=(uint8_t)success,.exit_x=-123,.exit_y=456,.exit_z=-789};
  assert(!taxi_trip_end_turn(&t,&e,&p,&in,&effects));
  assert(t.scale_5310==256&&t.scale_5312==256&&t.direction_753c==-1&&!t.active_1694&&!t.active_1695);
  assert(t.time_16ac==1234&&t.initial_time_16b0==5678&&t.allowance_16e0==77&&t.voice_25d6==99&&t.phase_5314==3&&t.active_customer==0);
  assert(e.result_16d0==3&&e.phase_16d1==2&&e.result_timer_16d2==88&&e.bonus_16e8==120&&e.bonus_display_timer_16e4==444&&e.displayed_bonus_16ec==222&&!e.flag_9aa6);
  assert(e.completed_1692==(uint8_t)(count+(success!=0)));
  assert(p.bytes[0]==((0xa5|65)&253)&&p.bytes[1]==0xa5&&p.bytes[2]==0xa5&&p.bytes[10]==1&&p.bytes[12]==10&&!p.bytes[13]&&p.bytes[9]==10&&!p.bytes[8]&&!p.bytes[11]&&!p.bytes[14]&&!p.bytes[15]);
  assert(word(p.bytes+16)==-123&&word(p.bytes+20)==456&&word(p.bytes+24)==-789&&word(p.bytes+28)==0x087ec484);
  assert(!effects.palette_refresh&&!effects.random_calls&&!effects.handler_2901c_argument&&!effects.handler_2901c_fraction);
 }
 TaxiTripStartState t={.time_16ac=111};TaxiTripEndState e={0};TaxiPassengerState p={0};TaxiTripEndEffects effect={.palette_refresh=77};TaxiTripEndInput in={.settings_0=0};
 assert(taxi_trip_end_turn(&t,&e,&p,&in,&effect)==-1&&t.time_16ac==111&&effect.palette_refresh==77);
 /* Source success is checked AFTER trip end against external retained16A8. */
 for(int timer=-1;timer<=1;timer++)assert(taxi_challenge_delivery_rank_request(4,timer)==(timer!=0));
 puts("PASS Turn challenge trip end 65536success/count cases, original fields retained/customer exit writes/wrapped counter/no ordinary fare effects/invalid atomicity. Explicit exit outputs, not earned delivery or full GBA execution.");
}
