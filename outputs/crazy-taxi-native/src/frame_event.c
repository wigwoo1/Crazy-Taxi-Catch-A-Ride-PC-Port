#include "frame_event.h"
static int services(TaxiFrameEvent *s,TaxiFrameEventCall call,void *context){
 if(call(context,s,0x08026888,0))return -1;
 if(!s->preserve_1ea7)s->settings_1eb0=(uint8_t)((s->settings_1eb0&0xe7)|8);
 if(s->callback_352c&&call(context,s,0x0803244c,s->callback_352c))return -1;
 if(call(context,s,0x080161f4,0))return -1;
 s->acknowledge=1;return 0;
}
int taxi_frame_event_callback_is_noop(uint32_t identity){
 return identity==UINT32_C(0x080161d9)||identity==UINT32_C(0x080161dd);
}
int taxi_frame_event_register(TaxiFrameEvent *s,uint32_t identity){
 if(!s)return -1;
 s->callback_352c=identity?identity:UINT32_C(0x080161dd);return 0;
}
int taxi_frame_event(TaxiFrameEvent *s,uint16_t scanline,TaxiFrameEventCall call,void *context){
 if(!s||!call)return -1;
 s->counter_3520+=1;s->scanline_4308=scanline;
 return services(s,call,context);
}
int taxi_frame_event_after_counter(TaxiFrameEvent *s,uint32_t counter,TaxiFrameEventCall call,void *context){
 if(!s||!call||counter!=(uint32_t)(s->counter_3520+1))return -1;
 s->counter_3520=counter;return services(s,call,context);
}
