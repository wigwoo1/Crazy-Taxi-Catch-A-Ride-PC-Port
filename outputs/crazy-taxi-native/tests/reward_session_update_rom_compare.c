#include "../src/reward_session.h"
#include <stdio.h>
#include <string.h>
static void put(uint8_t *p,int32_t v){for(unsigned j=0;j<4;j++)p[j]=(uint8_t)((uint32_t)v>>(8*j));}
int main(void){unsigned tick,pattern,sound;long delta;
 while(scanf("%u %ld %u %u",&tick,&delta,&pattern,&sound)==4){
 TaxiRewardState s={.gate_9aa4=7};TaxiRewardClock c={(uint8_t)tick,77};TaxiFareState f={.pending_whole_16a0=123,.pending_fraction_16a4=95};TaxiCashGlyphs g;taxi_cash_glyphs_init(&g);
 for(unsigned i=0;i<10;i++){memset(s.tips[i],0xa5,56);s.tips[i][2]=(uint8_t)(pattern==0?0:pattern==1?64:pattern==2?i+1:128+i);s.tips[i][1]=(uint8_t)(i*23);put(s.tips[i]+4,(int32_t)i);
 put(s.tips[i]+32,30720);put(s.tips[i]+36,20480);put(s.tips[i]+8,28672);put(s.tips[i]+12,35840);put(s.tips[i]+44,288);put(s.tips[i]+48,272);put(s.tips[i]+20,0);put(s.tips[i]+24,-496);}
 TaxiFareSession session={0};session.reward_gate_9aa4=s.gate_9aa4;session.reward_clock=c;session.fare=f;memcpy(session.tips,s.tips,sizeof s.tips);
 TaxiRewardUpdateEffects e;if(taxi_reward_session_update(&session,&g,(int32_t)delta,(uint8_t)sound,&e))return 1;
 s.gate_9aa4=session.reward_gate_9aa4;c=session.reward_clock;f=session.fare;memcpy(s.tips,session.tips,sizeof s.tips);
 printf("%u %u %u %ld %ld %u %u",s.gate_9aa4,c.clock_9aa5,c.active_count_9aa7,(long)f.pending_whole_16a0,(long)f.pending_fraction_16a4,e.expired,e.sound_requests);
 for(unsigned i=0;i<9;i++)printf(" %u",g.pending_target[i]);for(unsigned i=0;i<10;i++)for(unsigned j=0;j<56;j++)printf(" %u",s.tips[i][j]);puts("");
 }return 0;}
