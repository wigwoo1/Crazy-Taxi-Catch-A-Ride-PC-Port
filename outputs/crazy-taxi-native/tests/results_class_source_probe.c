#include "../src/results_state.h"
#include <stdio.h>
int main(void){int delta,scale,hold;while(scanf("%d%d%d",&delta,&scale,&hold)==3){TaxiResultsState s={.phase=3,.class_scale=(int16_t)scale,.class_hold=(int16_t)hold};unsigned event=taxi_results_state_step(&s,delta,0,99,0);printf("%d %d %u %u\n",s.class_scale,s.class_hold,s.phase,event);}return 0;}
