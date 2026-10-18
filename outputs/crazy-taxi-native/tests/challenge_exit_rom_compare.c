#include "challenge_exit.h"
#include <stdio.h>
int main(void){unsigned o,f,z,v;while(scanf("%u%u%u%u",&o,&f,&z,&v)==4){
 TaxiChallengeExit s={(uint8_t)o,(uint8_t)f,(uint8_t)z,9,(uint8_t)v,55,77,88,123};TaxiChallengeExitRequests q;
 int rc=taxi_challenge_exit_step(&s,&q);
 printf("%d %u %u %u %u %u %u %u %u",rc,s.outer,s.ended,s.frozen,s.main_mode,s.redraw,s.results,s.callback,q.count);
 for(unsigned i=0;i<q.count;i++)printf(" %u",q.calls[i]);puts("");
 }return 0;}
