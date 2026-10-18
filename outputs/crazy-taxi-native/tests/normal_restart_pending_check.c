#include "normal_restart_flow.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {uint32_t target,args[7],state[16];} Call;
typedef struct {unsigned random,wait_in,wait_out,left,fades,setup_calls,yields,pending,left_polls,polls,error_target,allow_mismatch,bad_target,forced_yield_target;Call log[256];unsigned count,started;} Context;
static int request(void *opaque,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t args[7],uint32_t *value){
 Context *c=opaque;*value=0;
 if(c->pending){if(c->allow_mismatch&&c->pending!=target){c->bad_target=target;return -1;}assert(c->pending==target&&!memcmp(c->log[c->count].args,args,sizeof c->log[0].args));c->polls++;if(c->left_polls-- >1)return 1;c->pending=0;}
 else{
  assert(c->count<256);Call *r=c->log+c->count;memset(r,0,sizeof *r);r->target=target;memcpy(r->args,args,sizeof r->args);
  const uint32_t state[16]={s->prefix.qualified_1691,s->prefix.flag_153c,s->prefix.flag_153d,s->frame.first_34d4,s->frame.second_34d8,s->graphics.driver_graphics_187c,s->graphics.intro_graphics_1804,s->entry.frame_34d4,s->entry.frame_34d8,s->entry.width_7542,s->entry.height_7544,s->entry.scroll_x,s->entry.scroll_y,s->graphics.fade_177b,s->entry.fade_177b,s->phase};memcpy(r->state,state,sizeof state);
  if(c->yields&&(target==0x0800d764||target==0x08026318||target==0x080227ac||target==c->forced_yield_target)){c->pending=target;c->left_polls=c->yields;c->started++;return 1;}
 }
 if(c->error_target==target)return -1;
 c->count++;
 if(target==0x0801e72c)*value=c->random;
 if(target==0x0802ac6c){s->entry.width_7542=240;s->entry.height_7544=160;}
 if(target==0x08026318){s->entry.width_7542++;s->entry.height_7544++;}
 if(target==0x0800e5a8){c->left=c->fades==0?c->wait_in:c->fades==1?c->wait_out:1;c->fades++;if(s->phase==TAXI_RESTART_ENTRY)s->entry.fade_177b=(uint8_t)(c->left!=0);else s->graphics.fade_177b=(uint8_t)(c->left!=0);}
 if(target==0x0800d764){if(c->left)c->left--;if(s->phase==TAXI_RESTART_ENTRY)s->entry.fade_177b=(uint8_t)(c->left!=0);else s->graphics.fade_177b=(uint8_t)(c->left!=0);}
 if(s->phase==TAXI_RESTART_SETUP){c->setup_calls++;s->frame.first_34d4+=c->setup_calls;s->frame.second_34d8^=c->setup_calls;}
 return 0;
}
static TaxiNormalRestartFlow initial(unsigned city,unsigned viewport){return (TaxiNormalRestartFlow){.prefix={99,98,97},.graphics={.retained_sl=1,.context_base=0x02002634,.driver_graphics_187c=123,.intro_graphics_1804=456,.city=(uint8_t)city,.viewport_1780=(uint8_t)viewport},.frame={123,456},.entry={.scroll_x=55,.scroll_y=77}};}
static int run(TaxiNormalRestartFlow *s,Context *c){int rc=taxi_normal_restart_flow_begin(s,request,c);for(unsigned n=0;!rc&&n<4096;n++)rc=taxi_normal_restart_flow_step(s,request,c);return rc;}
int main(void){unsigned cases=0,polls=0;const unsigned values[]={0,1,33};const unsigned random[]={0,65535};const unsigned view[]={0,17,255};
 Context *reference=calloc(1,sizeof *reference),*pending=calloc(1,sizeof *pending);assert(reference&&pending);
 for(unsigned city=0;city<2;city++)for(unsigned r=0;r<2;r++)for(unsigned v=0;v<3;v++)for(unsigned a=0;a<3;a++)for(unsigned b=0;b<3;b++){
  *reference=(Context){.random=random[r],.wait_in=values[a],.wait_out=values[b]};TaxiNormalRestartFlow expected=initial(city,view[v]);assert(run(&expected,reference)==1&&expected.phase==TAXI_RESTART_READY);
  for(unsigned yields=1;yields<=3;yields++){
   *pending=(Context){.random=random[r],.wait_in=values[a],.wait_out=values[b],.yields=yields};TaxiNormalRestartFlow actual=initial(city,view[v]);assert(run(&actual,pending)==1&&actual.phase==TAXI_RESTART_READY);
   assert(!memcmp(&actual,&expected,sizeof actual)&&pending->count==reference->count&&!memcmp(pending->log,reference->log,reference->count*sizeof(Call))&&pending->started>0&&pending->polls==pending->started*yields);cases++;polls+=pending->polls;
  }
 }
 /* Real elapsed service may clear fade before the child continuation is
    acknowledged. The parent must still resume that child, never skip it. */
 *pending=(Context){.wait_in=1,.wait_out=1,.yields=1,.allow_mismatch=1};TaxiNormalRestartFlow externally_completed=initial(0,17);
 unsigned completed_during_wait=0;int rc=taxi_normal_restart_flow_begin(&externally_completed,request,pending);
 for(unsigned n=0;!rc&&n<256;n++){
  if(pending->pending==0x0800d764){
   if(externally_completed.phase==TAXI_RESTART_GRAPHICS)externally_completed.graphics.fade_177b=0;
   else externally_completed.entry.fade_177b=0;
   pending->left=0;completed_during_wait++;
  }
  rc=taxi_normal_restart_flow_step(&externally_completed,request,pending);
 }
 if(rc!=1||completed_during_wait!=2){fprintf(stderr,"FAIL externally completed fade skipped pending child%08x and reached%08x, rc%d completions%u\n",pending->pending,pending->bad_target,rc,completed_during_wait);return 2;}
 for(unsigned target=0;target<3;target++){
  *pending=(Context){.random=0,.wait_in=1,.wait_out=1,.yields=3,.error_target=target==2?0x080227ac:target?0x08026318:0x0800d764};TaxiNormalRestartFlow actual=initial(0,17);assert(run(&actual,pending)==-1&&actual.phase==TAXI_RESTART_ERROR);TaxiNormalRestartFlow before=actual;unsigned count=pending->count;assert(taxi_normal_restart_flow_step(&actual,request,pending)==-1&&!memcmp(&before,&actual,sizeof actual)&&pending->count==count);
 }
 *pending=(Context){.yields=1,.forced_yield_target=0x08014a58};TaxiNormalRestartFlow invalid_child=initial(0,17);assert(run(&invalid_child,pending)==-1&&invalid_child.phase==TAXI_RESTART_ERROR);
 free(reference);free(pending);printf("PASS %u suspended restart flows,%u polls: exact completed helper order/arguments/source fields vs synchronous flow, no repeated clears/setup (including227AC), external fade completion during child suspension, terminal child failures and unsupported setup suspension rejected\n",cases,polls);return 0;}
