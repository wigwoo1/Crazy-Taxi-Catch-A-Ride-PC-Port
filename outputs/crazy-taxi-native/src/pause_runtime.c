#include "pause_runtime.h"
#include "palette_fade.h"
int taxi_pause_runtime_control(TaxiPauseRuntime *out,uint16_t held,uint8_t fade,
 uint8_t intro,uint8_t ended,TaxiPauseFrame *frame){
 if(!out||!frame)return -1;
 TaxiPauseRuntime s=*out;TaxiPauseFrame f={0};
 taxi_menu_input_step(&s.input,held,1);
 TaxiPauseInput in={.start=s.input.pressed[3],.confirm=s.input.pressed[0],
  .cancel=s.input.pressed[1],.up=s.input.repeat[6],.down=s.input.repeat[7],
  .fade_active=fade,.ended_1696=intro,.ended_1697=ended};
 if(taxi_pause_menu_step(&s.menu,&in,&f.events))return -1;
 f.palette_mode=s.alternate;
 *out=s;*frame=f;return 0;
}
int taxi_pause_runtime_prepare(TaxiPauseRuntime *out,uint8_t fade,uint16_t width,
 TaxiPauseFrame *frame){
 if(!out||!frame||!width)return -1;
 TaxiPauseRuntime s=*out;TaxiPauseFrame f=*frame;
 if(taxi_pause_render_requests(&s.menu,fade,width,f.draw,&f.draw_count,
  &s.menu.palette_request))return -1;
 f.palette_mode=-1; /*26318 owns consumption after the fresh wait. */
 *out=s;*frame=f;return 0;
}
int taxi_pause_runtime_present(TaxiPauseRuntime *out,uint8_t fade,uint16_t width,
 TaxiPauseFrame *frame){
 if(!out||!frame)return -1;
 TaxiPauseRuntime s=*out;TaxiPauseFrame f=*frame;
 if(taxi_pause_runtime_prepare(&s,fade,width,&f))return -1;
 f.palette_mode=taxi_palette_gameplay_dispatch(&s.menu.palette_request,&s.alternate);
 *out=s;*frame=f;return 0;
}
int taxi_pause_runtime_frame(TaxiPauseRuntime *out,uint16_t held,uint8_t fade,
 uint8_t intro,uint8_t ended,uint16_t width,TaxiPauseFrame *frame){
 if(!out||!frame||!width)return -1;
 TaxiPauseRuntime s=*out;TaxiPauseFrame f;
 if(taxi_pause_runtime_control(&s,held,fade,intro,ended,&f)||
    taxi_pause_runtime_present(&s,fade,width,&f))return -1;
 *out=s;*frame=f;return 0;
}
