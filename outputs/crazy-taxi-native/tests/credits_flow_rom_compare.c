#include <stdio.h>
#include "credits_flow.h"
#include "credits_text_fixture.h"
int main(void){TaxiCreditsFlow s;int delta,sine,effects,confirm,start,timer,sound;
 while(scanf("%d %d %d %u %u %u %d %d %d %d %d %d %d",&s.y_q8,&s.x_q8,&s.line_q8,&s.angle,&s.row,&s.line,&timer,&sound,&delta,&sine,&effects,&confirm,&start)==13){
 s.timer=(uint8_t)timer;s.sound_started=(uint8_t)sound;TaxiCreditsFrame f;
 if(taxi_credits_flow_step(&s,delta,sine,s.line<104?(const uint8_t *)credits_text[s.line]:0,(uint8_t)effects,confirm,start,&f))return 1;
 printf("%d %d %d %u %u %u %u %u %d %u %u %u %u %u %u",s.y_q8,s.x_q8,s.line_q8,s.angle,s.row,s.line,s.timer,s.sound_started,f.text_id,f.sound29,f.fade_out,f.write_row,f.scroll[0],f.scroll[1],f.scroll[2]);
 for(unsigned j=0;j<30;j++)printf(" %u",f.tiles[j]);puts("");
 }return 0;}
