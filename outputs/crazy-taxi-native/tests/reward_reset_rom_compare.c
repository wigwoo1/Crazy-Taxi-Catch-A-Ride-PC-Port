#include "reward_session.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned seed;
 while(scanf("%u",&seed)==1){
  TaxiFareSession s={0};s.reward_clock.clock_9aa5=(uint8_t)(seed+1);s.reward_gate_9aa4=(uint8_t)(seed+2);s.end.flag_9aa6=(uint8_t)(seed+3);
  s.reward_clock.active_count_9aa7=(uint8_t)(seed+4);s.trip.voice_25d6=(uint8_t)(seed+5);
  s.combo_whole_9aa8=(int32_t)(seed*12345u);s.combo_fraction_9aac=(int32_t)(seed*6789u);s.fare.pending_whole_16a0=(int32_t)(seed*987u);
  memset(s.tips,(int)(seed&255),sizeof s.tips);
  if(taxi_reward_session_reset(&s))return 2;
  printf("%u %u %u %u %u %u %u %u",s.reward_clock.clock_9aa5,s.reward_gate_9aa4,s.end.flag_9aa6,(unsigned)s.combo_whole_9aa8,(unsigned)s.combo_fraction_9aac,s.reward_clock.active_count_9aa7,s.trip.voice_25d6,(unsigned)s.fare.pending_whole_16a0);
  for(unsigned i=0;i<10;i++)for(unsigned j=0;j<56;j++)printf(" %u",s.tips[i][j]);puts("");
 }return 0;
}
