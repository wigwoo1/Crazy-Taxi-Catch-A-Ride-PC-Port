#include "../src/reward.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned kind,active,gate,combo,mode,sound,seed,voice,pattern;long whole,fraction,total,frac;
 while(scanf("%u %u %u %u %u %u %u %u %ld %ld %ld %ld %u",&kind,&active,&gate,&combo,&mode,&sound,&seed,&voice,&whole,&fraction,&total,&frac,&pattern)==13){
 TaxiRewardState s={.gate_9aa4=(uint8_t)gate,.combo_9aa6=(uint8_t)combo,.voice_25d6=(uint8_t)voice,.random_state=seed,.whole_9aa8=(int32_t)total,.fraction_9aac=(int32_t)frac};
 for(unsigned i=0;i<10;i++){memset(s.tips[i],0xa5,56);s.tips[i][2]=(uint8_t)(pattern==0?0:pattern==1?(i==4?0:64):pattern==2?64-i:128+i);}
 TaxiRewardInput in={(uint8_t)kind,(uint8_t)active,(uint8_t)mode,(uint8_t)sound,(int32_t)whole,(int32_t)fraction};TaxiRewardEffects e;
 if(taxi_reward(&s,&in,&e))return 1;
 printf("%u %u %u %ld %ld %d %d %u",s.combo_9aa6,s.voice_25d6,s.random_state,(long)s.whole_9aa8,(long)s.fraction_9aac,e.slot,e.sound_table_index,e.random_calls);
 for(unsigned i=0;i<10;i++)for(unsigned j=0;j<56;j++)printf(" %u",(unsigned)s.tips[i][j]);puts("");
 }return 0;}
