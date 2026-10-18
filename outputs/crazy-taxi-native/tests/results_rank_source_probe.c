#include "../src/results_state.h"
#include <stdio.h>
int main(void){int phase,delta,value,rank,target;while(scanf("%d%d%d%d%d",&phase,&delta,&value,&rank,&target)==5){TaxiResultsState s={.phase=(uint8_t)phase,.rank=(uint8_t)rank,.rank_scale=(int16_t)value,.timer=(int16_t)value};unsigned e=taxi_results_state_step(&s,delta,0,(int8_t)target,0);printf("%d %u %u %u %u\n",phase==5?s.rank_scale:s.timer,s.rank,s.phase,e,s.exiting);}return 0;}
