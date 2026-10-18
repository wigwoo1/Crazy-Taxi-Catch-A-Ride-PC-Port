#include "presentation_service.h"
#include "timing.h"
int taxi_presentation_service(TaxiPresentationService *s,TaxiPresentationCall call,void *context){
 if(!s||!call)return -1;
 if(call(context,s,0x08016208,1))return -1;
 return taxi_presentation_service_after_wait(s,call,context);
}
int taxi_presentation_service_after_wait(TaxiPresentationService *s,TaxiPresentationCall call,void *context){
 if(!s||!call)return -1;
 if(s->pending_5f24==2){if(call(context,s,0x08022508,1))return -1;}
 else if(s->pending_5f24==1){if(call(context,s,0x08022508,0))return -1;}
 s->pending_5f24=0;
 if(call(context,s,0x08015c3c,0))return -1;
 if(s->mode_7541==3||s->mode_7541==4){
  uint32_t value=s->first_754c;s->first_754c=s->second_7550;s->second_7550=value;
  if(s->mode_7541==4){
   if(s->bank_7548&1)s->display_control&=(uint16_t)0xffef;
   else s->display_control|=16;
  }
  s->bank_7548=(uint8_t)(s->bank_7548+1);
 }
 if(call(context,s,0x0800ee10,0)||call(context,s,0x08027cd0,0)||call(context,s,0x08028ee0,0))return -1;
 s->delta_3b00=(uint32_t)taxi_frame_delta(s->current_3520,s->previous_3524);
 s->previous_3524=s->current_3520;
 if(!s->frozen_5f20)s->elapsed_755c+=s->delta_3b00;
 s->cursor_1e10+=4;s->parity_7540=(uint8_t)((s->parity_7540+1)&1);
 return 0;
}

int taxi_presentation_blend_load(TaxiPresentationBlend *s,const uint8_t *header,size_t bytes){
 if(!s||!header||bytes<132)return -1;
 s->alpha_00e0=(uint16_t)(header[128]|((uint16_t)header[129]<<8));
 s->alpha_00e2=(uint16_t)(header[130]|((uint16_t)header[131]<<8));return 0;
}
int taxi_presentation_reset(TaxiPresentationService *s,uint16_t alpha_00e0,uint16_t alpha_00e2){
 if(!s)return -1;
 s->blend_control=0x1f4f;
 s->blend_alpha=(uint16_t)(((uint32_t)alpha_00e0<<8)|alpha_00e2);
 s->delta_3b00=1;s->previous_3524=s->current_3520;return 0;
}
