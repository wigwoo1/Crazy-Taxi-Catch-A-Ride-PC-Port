#include "../src/game_start.h"
#include <stdio.h>
int main(void){int a[15];while(1){for(unsigned i=0;i<15;i++)if(scanf("%d",a+i)!=1)return i?2:0;TaxiGameStart s={.phase=(int16_t)a[0],.hold=(int16_t)a[1],.offset=(int16_t)a[2],.scale=(int16_t)a[3],.rotation=(uint16_t)a[4],.axes={a[5],a[6],a[7]},.active=(uint8_t)a[8]};if((a[14]&255)!=4)return 3;unsigned events=taxi_game_start_challenge_step(&s,a[9],(int16_t)a[10],a[11],a[12],a[13]);printf("%d %d %d %d %u %ld %ld %ld %u %u\n",s.phase,s.hold,s.offset,s.scale,s.rotation,(long)s.axes[0],(long)s.axes[1],(long)s.axes[2],(unsigned)s.active,events);}}
