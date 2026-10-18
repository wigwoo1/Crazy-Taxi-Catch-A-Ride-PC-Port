#include "startup_screens.h"
#include <string.h>
typedef struct {TaxiStartupScreens *s;TaxiStartupScreensCall call;void *context;} Calls;
static int begin(TaxiStartupScreens *s,uint32_t mode,uint32_t cap,uint32_t bg,uint32_t obj){
 s->fade.fade.interval_1771=s->sequence.flow.fade_1771;
 s->fade.fade.reload_1772=s->sequence.flow.fade_1772;
 if(taxi_palette_fade_start_normal(&s->fade,mode,cap,bg,obj,240,160))return -1;
 TaxiPaletteSource source={s->descriptor.image+8,s->source_palette,256};
 return taxi_palette_fade_registered_normal(&s->banks,&s->registry,&s->fade,&source,1);
}
static int prepare(void *v,TaxiScreenSequence *seq,TaxiScreenDescriptor *d,uint32_t t,uint32_t a,uint32_t b,uint32_t c,uint32_t e){
 (void)seq;(void)d;if(t!=0x0800e5a8)return -1;return begin(v,a,b,c,e);
}
static int flow(void *v,TaxiScreenFlow *f,uint32_t t,uint32_t a,uint32_t b,uint32_t c,uint32_t d){
 Calls *calls=v;(void)f;
 if(t==0x0800e5a8)return begin(calls->s,a,b,c,d);
 return calls->call(calls->context,calls->s,t,a);
}
static int present(void *v,TaxiPresentationService *p,uint32_t t,uint32_t a){
 Calls *calls=v;TaxiStartupScreens *s=calls->s;(void)p;
 if(t==0x0800ee10){
  TaxiPaletteSource source={s->descriptor.image+8,s->source_palette,256};
  int result=taxi_palette_fade_service_normal(&s->banks,&s->registry,&s->fade,&source,1,(int32_t)s->presentation.delta_3b00,240,160,&s->mosaic);
  s->sequence.flow.fade_1771=s->fade.fade.interval_1771;s->sequence.flow.fade_1772=s->fade.fade.reload_1772;return result;
 }
 return calls->call(calls->context,s,t,a);
}
int taxi_startup_screens_load(TaxiStartupScreens *s,const TaxiScreenDescriptor *d,const uint8_t *image,size_t bytes){
 if(!s||!d||!d->image||d->type!=-1||d->callback||(d->flags&8))return -1;
 s->descriptor=*d;TaxiPaletteTransfer transfer;
 if(taxi_startup_bitmap_load_registered(&s->display,&s->registry,&s->banks,image,bytes,d->image,s->source_palette,s->bitmap[0],s->bitmap[1],&transfer)!=1)return -1;
 s->presentation.mode_7541=4;s->presentation.bank_7548=0;s->presentation.display_control=s->display.control;
 s->presentation.first_754c=0x06000000;s->presentation.second_7550=0x0600a000;
 s->presentation.elapsed_755c=0;
 return taxi_screen_sequence_prepare(&s->sequence,&s->descriptor,prepare,s);
}
int taxi_startup_screens_frame(TaxiStartupScreens *s,uint8_t confirm,uint8_t start,TaxiStartupScreensCall call,void *context){
 if(!s||!call)return -1;Calls calls={s,call,context};
 if(taxi_presentation_service(&s->presentation,present,&calls))return -1;
 s->sequence.audio_755c=s->presentation.elapsed_755c;
 s->display.control=s->presentation.display_control;s->display.globals[8]=s->presentation.bank_7548;
 s->display.globals[0]=s->presentation.parity_7540;
 uint32_t owners[3]={s->presentation.first_754c,s->presentation.second_7550,s->presentation.elapsed_755c};
 for(unsigned i=0;i<4;i++){
  s->display.globals[12+i]=(uint8_t)(owners[0]>>(8*i));
  s->display.globals[16+i]=(uint8_t)(owners[1]>>(8*i));
  s->display.globals[28+i]=(uint8_t)(owners[2]>>(8*i));
 }
 if(taxi_screen_flow_step(&s->sequence.flow,(int32_t)s->presentation.delta_3b00,s->descriptor.flags,s->descriptor.callback,160,s->fade.fade.active_177b,confirm,start,0,s->descriptor.fade_out_bg,s->descriptor.fade_out_obj,flow,&calls))return -1;
 return s->sequence.flow.remaining_359c<=0&&!s->fade.fade.active_177b;
}
static int advance(void *v,TaxiScreenSequence *seq,TaxiScreenDescriptor *d,uint32_t t,uint32_t a,uint32_t b,uint32_t c,uint32_t e){
 Calls *calls=v;(void)seq;(void)d;(void)b;(void)c;(void)e;return calls->call(calls->context,calls->s,t,a);
}
int taxi_startup_screens_advance(TaxiStartupScreens *s,TaxiScreenDescriptor *next,TaxiStartupScreensCall call,void *context){
 if(!s||!call||s->sequence.flow.remaining_359c>0||s->fade.fade.active_177b)return -1;
 Calls calls={s,call,context};return taxi_screen_sequence_advance(&s->sequence,next,advance,&calls);
}
