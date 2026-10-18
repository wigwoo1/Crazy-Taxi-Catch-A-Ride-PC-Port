#include "title_runtime.h"
void taxi_title_runtime_begin(TaxiTitleRuntime *s){
 if(!s)return;
 *s=(TaxiTitleRuntime){.bg_palette_mask=0x2001,.obj_palette_mask=0x2001};taxi_title_flow_init(&s->flow);
 taxi_palette_fade_begin(&s->fade,1);s->bldy=16;
}
int taxi_title_runtime_frame(TaxiTitleRuntime *s,uint32_t *random,
 uint16_t physical,int sound,TaxiTitleRuntimeOutput *out){
 if(!s||!random||!out)return -1;
 *out=(TaxiTitleRuntimeOutput){.callback={.sound_index=-1},.confirm_sound=-1};
 if(s->finished){out->parent=s->finished;return 0;}
 /* 16454 callback,26348 input,263c6 fade,26400 clock,16542 flow. */
 if(taxi_title_callback(&s->callback,random,sound,1,s->clock,
    s->fade.active_177b,240,160,&out->callback))return -1;
 taxi_menu_input_step(&s->input,physical,1);
 if(s->fade.active_177b){
  uint8_t interval=(uint8_t)(s->fade.interval_1771-1);
  if(!interval||interval>=128){
   int raw=s->fade.progress_178a+(s->fade.mode_1773?1:-1);
   if((s->bg_palette_mask|s->obj_palette_mask)&0x2000)
    taxi_palette_fade_bldy(s->fade.mode_1773,(int16_t)raw,&s->bldy);
  }
 }
 taxi_palette_fade_step(&s->fade,1);s->clock++;
 TaxiTitleActions a;
 if(taxi_title_flow_step(&s->flow,1,s->fade.active_177b,1,
    s->input.repeat[0],s->input.repeat[3],sound,&a))return -1;
 out->confirm_sound=a.sound_index;
 if(a.begin_exit_fade){
  s->fade.interval_1771=s->fade.reload_1772=0;
  taxi_palette_fade_begin(&s->fade,0);s->bldy=0;
 }
 if(s->flow.remaining<=0&&!s->fade.active_177b){
  s->finished=s->flow.menu_requested?1:2;out->parent=s->finished;
 }
 return 0;
}
