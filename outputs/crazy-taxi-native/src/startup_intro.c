#include "startup_intro.h"
#include <string.h>
static int sink(void *ctx,const TaxiStartupAnimationEvent *e){
 TaxiStartupIntro *s=ctx;const uint32_t *v=e->values;
 switch(e->kind){
 case 1:s->scroll=(uint16_t)v[0];break;
 case 2:s->blend=(uint16_t)v[0];break;
 case 3:s->alpha=(uint16_t)v[0];break;
 case 4:case 5:{
  if(v[0]>=33||v[1]||v[7]||v[10])return -1;
  TaxiStartupObjQueued out;int result;
  if(e->kind==5)result=taxi_startup_obj_scaled_queue(&s->queue,&s->transport,s->metrics[v[0]],(int32_t)v[2],(int32_t)v[3],(int32_t)v[4],s->width,s->height,(uint8_t)v[5],(uint8_t)v[9],(uint8_t)v[6],s->obj_mask,s->tile_base,(int16_t)v[8],&out);
  else result=taxi_startup_obj_queue(&s->queue,s->metrics[v[0]],(int32_t)v[2],(int32_t)v[3],(int32_t)v[4],s->width,s->height,(uint8_t)v[5],(uint8_t)v[9],(uint8_t)v[6],s->obj_mask,s->tile_base,&out);
  if(result<0)return -1;if(result)s->accepted++;else s->clipped++;break;
 }
 case 6:if(v[0]>=13)return -1;s->palette[48+v[0]]=(uint16_t)v[1];break;
 case 7:memcpy(s->fade_request,v,sizeof s->fade_request);s->fades++;break;
 default:return -1;
 }return 0;
}
int taxi_startup_intro_animate(TaxiStartupIntro *s,uint8_t fade,uint8_t mode,uint8_t confirm,uint8_t start,uint16_t bg,uint16_t obj){
 if(!s)return -1;
 int result=taxi_startup_animation_step(&s->animation,fade,mode,s->width,s->palette,confirm,start,bg,obj,sink,s);
 return result;
}
int taxi_startup_intro_step(TaxiStartupIntro *s,uint8_t fade,uint8_t mode,uint8_t confirm,uint8_t start,uint16_t bg,uint16_t obj){
 int result=taxi_startup_intro_animate(s,fade,mode,confirm,start,bg,obj);if(result)return result;
 return taxi_startup_obj_publish(&s->queue,&s->transport,s->base_template,s->matrix_template);
}
