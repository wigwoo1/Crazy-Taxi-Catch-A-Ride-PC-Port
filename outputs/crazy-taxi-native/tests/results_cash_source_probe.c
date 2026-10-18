#include "../src/results_state.h"
#include <stdio.h>
int main(void){int d,t,i,target,current;while(scanf("%d%d%d%d%d",&d,&t,&i,&target,&current)==5){TaxiResultsState s={.phase=1,.timer=(int16_t)t,.digit=(int16_t)i};s.target[i]=(char)target;s.cash[i]=(char)current;unsigned e=taxi_results_state_step(&s,d,0,99,0);printf("%d %d %u %u %u\n",s.timer,s.digit,s.phase,(unsigned char)s.cash[i],e);}return 0;}
