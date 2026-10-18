#include "../src/results_state.h"
#include <stdio.h>
int main(void){int phase,d,t,current,target;while(scanf("%d%d%d%d%d",&phase,&d,&t,&current,&target)==5){TaxiResultsState s={.phase=(uint8_t)phase,.timer=(int16_t)t,.customers=(uint8_t)current};unsigned e=taxi_results_state_step(&s,d,(int8_t)target,99,0);printf("%d %u %u %u\n",s.timer,s.customers,s.phase,e);}return 0;}
