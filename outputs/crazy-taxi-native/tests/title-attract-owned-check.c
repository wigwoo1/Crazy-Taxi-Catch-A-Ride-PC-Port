#include "title_attract_owned.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){for(unsigned active=0;active<2;active++){
 TaxiTitleParent state={0};TaxiTitleParentContinuation c={0};TaxiAttractPrepare p;memset(&p,165,sizeof p);
 assert(taxi_title_parent_begin(&c,&state)==TAXI_TITLE_PARENT_TITLE);
 assert(taxi_title_attract_prepare_owned(&c,&p)==-1&&p.recorder[0]==165);
 assert(taxi_title_parent_resume(&c)==TAXI_TITLE_PARENT_PREPARE_ATTRACT);state.active_3898=(uint8_t)active;
 assert(taxi_title_attract_prepare_owned(&c,&p)==TAXI_TITLE_PARENT_ATTRACT);
 assert(p.recorder[0]==1&&!p.recorder[1]&&!p.recorder[2]&&!p.flag_1ea8&&!p.flag_3895);
 for(unsigned i=0;i<20;i++)assert(p.recorder[i]==(i==0?1:i==1||i==2||(i>=8&&i<12)?0:i==16||i==17?255:165));
 assert(taxi_title_attract_prepare_owned(&c,&p)==-1);
 assert(taxi_title_parent_resume(&c)==(active?TAXI_TITLE_PARENT_TITLE:TAXI_TITLE_PARENT_DONE));
 }puts("PASS native title->attract preparation retains source unconditional ABEC handoff");return 0;}
