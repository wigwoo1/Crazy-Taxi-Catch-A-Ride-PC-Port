#include "normal_restart_flow.h"
#include <stdio.h>
typedef struct {unsigned random,wait_in,wait_out,left,fades,setup_calls;} Context;
static void state(TaxiNormalRestartFlow *s){
 unsigned entering=s->phase==TAXI_RESTART_ENTRY||s->phase==TAXI_RESTART_READY;
 printf(" %u %u %u %u %u %u %u %u %u %u %u %u",s->prefix.qualified_1691,s->prefix.flag_153c,s->prefix.flag_153d,
 entering?s->entry.frame_34d4:s->frame.first_34d4,entering?s->entry.frame_34d8:s->frame.second_34d8,
 s->graphics.driver_graphics_187c,s->graphics.intro_graphics_1804,s->entry.width_7542,s->entry.height_7544,s->entry.scroll_x,s->entry.scroll_y,
 entering?s->entry.fade_177b:s->graphics.fade_177b);
}
static int request(void *opaque,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t args[7],uint32_t *value){
 Context *c=opaque;printf(" %u",target);for(unsigned j=0;j<7;j++)printf(" %u",args[j]);state(s);*value=0;
 if(target==0x0801e72c)*value=c->random;
 if(target==0x0802ac6c){s->entry.width_7542=240;s->entry.height_7544=160;}
 if(target==0x08026318){s->entry.width_7542++;s->entry.height_7544++;}
 if(target==0x0800e5a8){c->left=c->fades==0?c->wait_in:c->fades==1?c->wait_out:1;c->fades++;
  if(s->phase==TAXI_RESTART_ENTRY)s->entry.fade_177b=(uint8_t)(c->left!=0);else s->graphics.fade_177b=(uint8_t)(c->left!=0);
 }
 if(target==0x0800d764){if(c->left)c->left--;
  if(s->phase==TAXI_RESTART_ENTRY)s->entry.fade_177b=(uint8_t)(c->left!=0);else s->graphics.fade_177b=(uint8_t)(c->left!=0);
 }
 if(s->phase==TAXI_RESTART_SETUP){c->setup_calls++;s->frame.first_34d4+=c->setup_calls;s->frame.second_34d8^=c->setup_calls;}
 return 0;
}
int main(void){unsigned city,random,viewport,wait_in,wait_out;
 while(scanf("%u%u%u%u%u",&city,&random,&viewport,&wait_in,&wait_out)==5){
  TaxiNormalRestartFlow s={.prefix={99,98,97},.graphics={.retained_sl=1,.context_base=0x02002634,.driver_graphics_187c=123,.intro_graphics_1804=456,.city=(uint8_t)city,.viewport_1780=(uint8_t)viewport},.frame={123,456},.entry={.scroll_x=55,.scroll_y=77}};
  Context c={.random=random,.wait_in=wait_in,.wait_out=wait_out};printf("%u",40+wait_in+wait_out);
  int rc=taxi_normal_restart_flow_begin(&s,request,&c);
  for(unsigned n=0;!rc&&n<100;n++)rc=taxi_normal_restart_flow_step(&s,request,&c);
  if(rc!=1||s.phase!=TAXI_RESTART_READY)return 2;
  /* phase READY uses final entry owners too. */
  s.phase=TAXI_RESTART_ENTRY;state(&s);puts("");
 }return 0;
}
