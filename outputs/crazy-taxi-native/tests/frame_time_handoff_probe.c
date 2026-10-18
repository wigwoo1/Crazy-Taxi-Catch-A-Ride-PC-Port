#include "../src/frame_time.h"
#include <stdio.h>
int main(void){int prior,frame,phase;unsigned active,fade,exiting,freeze,ended,demo;
 while(scanf("%d %d %d %u %u %u %u %u %u",&prior,&frame,&phase,&active,&fade,&exiting,&freeze,&ended,&demo)==9)
 printf("%d %d\n",taxi_frame_time_intro_delta(prior,frame,(int16_t)phase,(uint8_t)active,(uint8_t)fade,(uint8_t)exiting),taxi_frame_time_normal_delta(prior,frame,(uint8_t)active,(uint8_t)freeze,(uint8_t)ended,(uint8_t)demo));
 return 0;}
