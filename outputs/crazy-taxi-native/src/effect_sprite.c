#include "effect_sprite.h"
#include <string.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t shift(uint32_t u,unsigned n){return bits((u>>n)|((u&0x80000000u)?(~0u<<(32-n)):0));}
static int32_t div32(int32_t a,int32_t b){return bits((uint32_t)((int64_t)a/b));}
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int taxi_effect_world_sprite(const TaxiEffectRecord *record,TaxiPoint camera,
 int16_t pitch,int16_t yaw,int16_t roll,const TaxiTrig *trig,
 int32_t focal,int32_t cx,int32_t cy,int32_t phase,int disabled,
 TaxiEffectRandom random,void *context,TaxiEffectSprite *out){
 if(!record||!trig||!out)return -1;
 if(!record->bytes[0x34])return 0;
 TaxiPoint p={bits(word(record->bytes+4)-(uint32_t)camera.x),
 bits(word(record->bytes+8)-(uint32_t)camera.y),
 bits(word(record->bytes+12)-(uint32_t)camera.z)};
 p=taxi_rotate_camera(p,-(int32_t)pitch,-(int32_t)yaw,-(int32_t)roll,trig);
 return taxi_effect_sprite(p,focal,cx,cy,record->bytes[0x34],record->bytes[0x35],
 phase,disabled,random,context,out);
}
int taxi_effect_sprite(TaxiPoint p,int32_t focal,int32_t cx,int32_t cy,
 uint8_t type,uint8_t frame,int32_t phase,int disabled,TaxiEffectRandom random,
 void *context,TaxiEffectSprite *out){
 if(!out||focal<=0)return -1;
 if(!type||p.z<=focal)return 0;
 if((type==2||type==3)&&!disabled&&!random)return -1;
 TaxiEffectSprite s={0};
 s.position=(TaxiPoint){bits((uint32_t)cx+(uint32_t)div32(bits((uint32_t)p.x*(uint32_t)focal),p.z)),
 bits((uint32_t)cy-(uint32_t)div32(bits((uint32_t)p.y*(uint32_t)focal),p.z)),p.z};
 int32_t scale=div32(bits((uint32_t)focal*1536u),p.z);
 uint8_t random_frame=0;
 if(type==2||type==3){
  int32_t extra=shift((uint32_t)div32(phase,1138)*288u,8);
  scale=bits(((uint32_t)scale+(uint32_t)extra)*2u);
  if(!disabled){
   random_frame=(uint8_t)(random(context,0,65535)%3);
   s.position.x=bits((uint32_t)s.position.x+(uint32_t)random(context,-5,5));
   s.position.y=bits((uint32_t)s.position.y-(uint32_t)random(context,4,8));
  }
 }else if(type==4||type==5){
  scale=shift((uint32_t)scale*384u,8);
  if(type==5)s.position.y=bits((uint32_t)s.position.y-(uint32_t)shift((uint32_t)scale,5));
 }
 if(scale==256)scale=257;
 if(scale>512)scale=512;
 uint16_t low=(uint16_t)scale;memcpy(&s.scale,&low,2);
 s.priority=0;s.bucket=2;
 if(type==1){s.group=8;s.frame=frame;s.flags=4;}
 else if(type==2||type==3){s.group=9;s.frame=random_frame;s.palette=type==3;}
 else {s.group=10;s.frame=type==4?frame:(uint8_t)(frame+5);}
 *out=s;return 1;
}
