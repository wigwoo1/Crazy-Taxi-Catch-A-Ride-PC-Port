#include "challenge_exit.h"
#include <stdio.h>
int main(void){unsigned a,b,c;while(scanf("%u%u%u",&a,&b,&c)==3){TaxiChallengeRestartStatus s={(uint8_t)a,(uint8_t)b,(uint8_t)c};if(taxi_challenge_restart_clear_status(&s))return 1;printf("%u %u %u\n",s.qualified_1691,s.success_153c,s.rank_153d);}return 0;}
