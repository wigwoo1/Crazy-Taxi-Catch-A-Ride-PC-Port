#include "effect_update.h"
#include <limits.h>
#include <string.h>
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void put(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static int32_t signed32(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
static uint32_t asr(uint32_t v,unsigned n){return (v>>n)|((v&0x80000000u)?(~0u<<(32-n)):0);}
static int update(TaxiEffectRecord *record,TaxiPoint attachment,int32_t delta,
 uint32_t clock_base,TaxiEffectHeight height,void *context,int ground,unsigned clock_shift,unsigned cap){
 if(!record||(ground&&!height))return -1;
 TaxiEffectRecord next=*record;uint8_t *b=next.bytes;
 for(unsigned off=0x14;off<=0x1c;off+=4)put(b+off,word(b+off)+word(b+off+0x10));
 put(b+4,(uint32_t)attachment.x+asr(word(b+0x14),12));
 put(b+12,(uint32_t)attachment.z+asr(word(b+0x1c),12));
 TaxiPoint point={signed32(word(b+4)),signed32(word(b+8)),signed32(word(b+12))};
 int result=1;
 if(ground){
  int32_t y=INT32_MIN;result=height(context,point,&y);
  if(result!=0&&result!=1)return -1;
  put(b+8,(uint32_t)(result?y:INT32_MIN));
 }else put(b+8,(uint32_t)attachment.y+asr(word(b+0x18),12));
 uint32_t timer=word(b+0x40);
 if(timer==0){
  b[0x35]=(uint8_t)(b[0x35]+1);
  if(b[0x35]==4)b[0x35]=3;
  int32_t clock=signed32(asr(clock_base<<clock_shift,8));
  put(b+0x40,(uint32_t)(clock/4));
 }else{
  timer-=(uint32_t)delta;
  put(b+0x40,signed32(timer)<0?0:timer);
 }
 uint16_t size=(uint16_t)((uint32_t)b[0x36]|(uint32_t)b[0x37]<<8);
 size=(uint16_t)(size+(uint32_t)delta*640u);
 if(size>cap&&size<32768)size=(uint16_t)cap;
 b[0x36]=(uint8_t)size;b[0x37]=(uint8_t)(size>>8);
 *record=next;return result;
}
int taxi_effect_update(TaxiEffectRecord *r,TaxiPoint a,int32_t d,uint32_t c,
 TaxiEffectHeight h,void *ctx){return update(r,a,d,c,h,ctx,1,8,24576);}
int taxi_effect_update_tag(TaxiEffectRecord *r,uint32_t tag,TaxiPoint a,
 int32_t d,uint32_t c,TaxiEffectHeight h,void *ctx){
 if(tag==0x08021959)return update(r,a,d,c,h,ctx,1,8,24576);
 if(tag==0x080218ad||tag==0x08021a05||tag==0x08021ab1)return update(r,a,d,c,h,ctx,0,8,24576);
 if(tag==0x08021b5d)return update(r,a,d,c,h,ctx,0,7,30720);
 return -1;
}

