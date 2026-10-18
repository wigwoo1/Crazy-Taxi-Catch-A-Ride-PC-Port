#include "../src/reward_update.h"
#include <stdio.h>
int main(void){unsigned p,i,e,m;while(scanf("%u %u %u %u",&p,&i,&e,&m)==4)printf("%d\n",taxi_reward_update_allowed((uint8_t)p,(uint8_t)i,(uint8_t)e,(uint8_t)m));return 0;}
