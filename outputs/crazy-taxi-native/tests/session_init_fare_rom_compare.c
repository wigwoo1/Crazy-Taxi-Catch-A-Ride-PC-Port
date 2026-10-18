#include "session_init_fare.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){unsigned mode,seconds,seed;while(scanf("%u %u %u",&mode,&seconds,&seed)==3){
 TaxiSessionInit init={0};TaxiFareSession f;memset(&f,0xa5,sizeof f);init.settings[0]=(uint8_t)mode;init.settings[5]=(uint8_t)seconds;
 f.trip.active_customer=3;f.trip.direction_753c=77;f.clock.remaining_16a8=(int32_t)seed;
 assert(!taxi_session_init_fare(&init,&f,240));
 printf("%lld ",(long long)f.fare.qualified_1691);
 printf("%lld ",(long long)f.fare.rating_1693);
 printf("%lld ",(long long)f.fare.total_whole_1698);
 printf("%lld ",(long long)f.fare.total_fraction_169c);
 printf("%lld ",(long long)f.fare.pending_whole_16a0);
 printf("%lld ",(long long)f.fare.pending_fraction_16a4);
 printf("%lld ",(long long)f.clock.remaining_16a8);
 printf("%lld ",(long long)f.trip.time_16ac);
 printf("%lld ",(long long)f.trip.initial_time_16b0);
 printf("%lld ",(long long)f.trip.allowance_16e0);
 printf("%lld ",(long long)f.trip.active_1694);
 printf("%lld ",(long long)f.trip.active_1695);
 printf("%lld ",(long long)f.end.completed_1692);
 printf("%lld ",(long long)f.end.result_16d0);
 printf("%lld ",(long long)f.end.phase_16d1);
 printf("%lld ",(long long)f.end.result_timer_16d2);
 printf("%lld ",(long long)f.end.bonus_16e8);
 printf("%lld ",(long long)f.end.bonus_display_timer_16e4);
 printf("%lld ",(long long)f.end.displayed_bonus_16ec);
 printf("%lld ",(long long)f.gate_1700);
 printf("%lld ",(long long)f.ended_1697);
 printf("%lld ",(long long)f.rank_16d4);
 printf("%lld ",(long long)f.expiry.phase_16bc);
 printf("%lld ",(long long)f.expiry.hold_16be);
 printf("%lld ",(long long)f.expiry.scale_16c0);
 printf("%lld ",(long long)f.distance_display.phase_16c2);
 printf("%lld ",(long long)f.distance_display.offset_16c4);
 printf("%lld ",(long long)f.distance_display.hold_16c8);
 printf("%u %u %d %d %u %u %u\n",f.outer_state_1ea8,f.audio_muted_1534,f.trip.active_customer,f.trip.direction_753c,f.trip.scale_5310,f.trip.scale_5312,f.trip.phase_5314);
 assert(f.trip.voice_25d6==0xa5&&f.ranking_loaded==0xa5&&f.ranking_scores[0]==0xa5a5a5a5u&&f.tips[0][0]==0xa5&&f.reward_gate_9aa4==0xa5&&f.end.flag_9aa6==0xa5);
}return 0;}
