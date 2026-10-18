#include "../src/reward_update.h"
#include <stdio.h>
int main(void){long gate,pending;while(scanf("%ld %ld",&gate,&pending)==2)printf("%d\n",taxi_normal_movement_allowed((int32_t)gate,(int32_t)pending));return 0;}
