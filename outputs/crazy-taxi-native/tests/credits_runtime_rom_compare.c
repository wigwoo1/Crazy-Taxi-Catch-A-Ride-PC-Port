#include "credits_runtime.h"
#include "credits_text_fixture.h"
#include <stdio.h>
int main(void){unsigned interval,reload,confirm_tick,start_tick,ticks;
 while(scanf("%u %u %u %u %u",&interval,&reload,&confirm_tick,&start_tick,&ticks)==5){
 TaxiCreditsRuntime s;taxi_credits_runtime_begin(&s,(uint8_t)interval,(uint8_t)reload);
 for(unsigned i=0;i<ticks;i++){
  TaxiCreditsRuntimeFrame f;
  if(taxi_credits_runtime_frame(&s,1,0,s.flow.line<104?(const uint8_t *)credits_text[s.flow.line]:0,1,i==confirm_tick,i==start_tick,&f))return 1;
  printf("%u %d %u %u %u %u %u %u %u %u %u %u %u %u %u\n",i,s.fade.progress_178a,s.fade.active_177b,s.fade.endpoint_count_1774,s.fade.interval_1771,s.bldy,s.exiting,s.complete,s.flow.line,s.scroll[0],s.scroll[1],s.scroll[2],f.running,f.stop_music,f.frame.sound29);
 }
 }return 0;}
