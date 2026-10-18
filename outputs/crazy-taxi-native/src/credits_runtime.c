#include "credits_runtime.h"
#include <string.h>
void taxi_credits_runtime_begin(TaxiCreditsRuntime *s,uint8_t interval,uint8_t reload){
 if(!s)return;
 *s=(TaxiCreditsRuntime){.scroll={0,0,256},.bldy=16};
 taxi_credits_flow_begin(&s->flow);s->fade.interval_1771=interval;s->fade.reload_1772=reload;
 taxi_palette_fade_begin(&s->fade,1);
}
int taxi_credits_runtime_frame(TaxiCreditsRuntime *out,int32_t delta,int32_t sine,
 const uint8_t text[31],uint8_t effects,int confirm,int start,TaxiCreditsRuntimeFrame *frame){
 if(!out||!frame)return -1;
 TaxiCreditsRuntime s=*out;TaxiCreditsRuntimeFrame f={.frame={.text_id=-1}};
 if(s.complete){*frame=f;return 0;}
 if(!s.exiting&&!s.fade.active_177b){
  f.row=s.flow.row;f.running=1;
  if(taxi_credits_flow_step(&s.flow,delta,sine,text,effects,confirm,start,&f.frame))return -1;
  memcpy(s.scroll,f.frame.scroll,sizeof s.scroll);
  for(unsigned j=0;j<f.frame.fade_out;j++)taxi_palette_fade_begin(&s.fade,0);
  if(f.frame.fade_out)s.exiting=1;
 }
 /* Original hardware coefficient uses the raw updated signed progress,
    before endpoint clamping. Retain last BLDY when service is inactive. */
 if(s.fade.active_177b){
  int16_t raw=s.fade.progress_178a;uint8_t next=(uint8_t)(s.fade.interval_1771-1);
  if(!next||next>=128){uint16_t bits=(uint16_t)((uint32_t)(uint16_t)raw+
      (s.fade.mode_1773? (uint32_t)delta:0u-(uint32_t)delta));memcpy(&raw,&bits,2);}
  taxi_palette_fade_step(&s.fade,delta);
  if((!next||next>=128)&&!taxi_palette_fade_bldy(s.fade.mode_1773,raw,&s.bldy))return -1;
 }
 if(s.exiting&&!s.fade.active_177b){
  memset(s.scroll,0,sizeof s.scroll);f.stop_music=f.returned=1;s.complete=1;
 }
 *out=s;*frame=f;return 0;
}
