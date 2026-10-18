#include "body_points.h"
#include <string.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
int taxi_body_points(TaxiBodyPoints *out,TaxiPoint actor,uint16_t pitch,
 uint16_t roll,int32_t heading,int32_t drift,const TaxiTrig *trig){
 if(!out||!trig)return -1;
 static const TaxiPoint local[6]={{-179,256,-358},{179,256,-358},
 {-179,64,-358},{179,64,-358},{0,64,-358},{0,64,358}};
 static const TaxiPoint vectors[6]={{-768,256,-2048},{768,256,-2048},
 {-256,256,-3072},{256,256,-3072},{0,256,-2048},{0,256,2048}};
 uint32_t sum=(uint32_t)heading+(uint32_t)drift;
 int32_t yaw=bits((sum>>12)|((sum&0x80000000u)?0xfff00000u:0));
 TaxiBodyPoints next;
 for(unsigned i=0;i<6;i++){
  TaxiPoint p=taxi_rotate_point(local[i],-(int32_t)pitch,yaw,roll,trig);
  next.positions[i]=(TaxiPoint){bits((uint32_t)p.x+(uint32_t)actor.x),
   bits((uint32_t)p.y+(uint32_t)actor.y),bits((uint32_t)p.z+(uint32_t)actor.z)};
  next.vectors[i]=taxi_rotate_point(vectors[i],-(int32_t)pitch,yaw,roll,trig);
 }
 *out=next;return 1;
}
