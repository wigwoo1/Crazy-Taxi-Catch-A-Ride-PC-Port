#include "normal_exit.h"
#include <stdio.h>
int main(void){unsigned outer,fade,frozen,qualified,exit;
 while(scanf("%u%u%u%u%u",&outer,&fade,&frozen,&qualified,&exit)==5){
  TaxiNormalExit s={.outer=(uint8_t)outer,.fade=(uint8_t)fade,.frozen=(uint8_t)frozen,.qualified=(uint8_t)qualified,.exit_flag=(uint8_t)exit,.ended=9,.main_mode=55,.redraw=77,.results=88,.callback=123};
  TaxiNormalExitRequests a,b={0};int rc=taxi_normal_exit_begin(&s,&a);if(rc<0)return 2;
  if(rc==1&&taxi_normal_exit_finish(&s,1,&b)!=1)return 3;
  printf("%u %u %u %u %u %u %u %u",s.outer,s.ended,s.frozen,s.main_mode,s.redraw,s.results,s.callback,a.count+b.count);
  for(unsigned i=0;i<a.count;i++)printf(" %u",a.calls[i]);for(unsigned i=0;i<b.count;i++)printf(" %u",b.calls[i]);puts("");
 }return 0;}
