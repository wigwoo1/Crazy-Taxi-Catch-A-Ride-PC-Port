#include "../src/render_history.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){TaxiRenderHistory h={0};TaxiRenderBracket b={0};assert(taxi_render_history_bracket(&h,0,&b)==-1);char cmd;unsigned long long time,tag;while(scanf(" %c",&cmd)==1){if(cmd=='P'){assert(scanf("%llu%llu",&time,&tag)==2);assert(!taxi_render_history_push(&h,time,tag));TaxiRenderHistory old=h;assert(taxi_render_history_push(&h,time,tag)==-1);assert(!memcmp(&old,&h,sizeof(h)));}else if(cmd=='Q'){assert(scanf("%llu",&time)==1);assert(!taxi_render_history_bracket(&h,time,&b));printf("%llu %llu %.17g %u\n",(unsigned long long)b.before_tag,(unsigned long long)b.after_tag,b.alpha,b.held);}else if(cmd=='R')taxi_render_history_clear(&h);else return 1;}return 0;}
