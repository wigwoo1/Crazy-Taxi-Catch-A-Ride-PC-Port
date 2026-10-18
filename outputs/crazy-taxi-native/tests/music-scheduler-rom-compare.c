#include "../src/music_tracker.h"
#include <stdio.h>
int main(void){unsigned clock,enabled,loaded,playing,phase,speed,delay;while(scanf("%u %u %u %u %u %u %u",&clock,&enabled,&loaded,&playing,&phase,&speed,&delay)==7){uint8_t divider=(uint8_t)clock;TaxiMusicState s={0};s.state[1]=(uint8_t)speed;s.state[3]=(uint8_t)phase;s.state[4]=(uint8_t)delay;int run=taxi_music_clock_gate(&divider,(uint8_t)enabled,(uint8_t)loaded,(uint8_t)playing);taxi_music_tick_finish(&s);printf("%u %d %u %u\n",divider,run,s.state[3],s.state[4]);}return 0;}
