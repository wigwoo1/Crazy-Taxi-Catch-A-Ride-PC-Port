#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "effect_update.h"
static void put(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
typedef struct {TaxiPoint point;int result,calls;} Query;
static int height(void *ctx,TaxiPoint p,int32_t *y){Query *q=ctx;q->point=p;q->calls++;*y=962;return q->result;}
int main(void){
 TaxiEffectRecord r={{0}};Query q={{0},1,0};TaxiPoint attachment={INT32_MAX,999,30};
 put(r.bytes,0x08021959);put(r.bytes+8,(uint32_t)-256);
 put(r.bytes+0x14,4095);put(r.bytes+0x24,1);
 put(r.bytes+0x18,UINT32_MAX);put(r.bytes+0x28,2);
 put(r.bytes+0x1c,0);put(r.bytes+0x2c,UINT32_MAX);
 assert(taxi_effect_update(&r,attachment,1,60,height,&q)==1);
 assert(q.point.x==INT32_MIN&&q.point.y==-256&&q.point.z==29);
 assert(word(r.bytes+8)==962&&word(r.bytes+0x18)==1);
 assert(word(r.bytes)==0x08021959&&word(r.bytes+0x40)==15&&r.bytes[0x35]==1);
 assert(taxi_effect_update(&r,attachment,16,60,height,&q)==1);
 assert(word(r.bytes+0x40)==0&&r.bytes[0x35]==1);
 q.result=0;r.bytes[0x35]=3;
 assert(taxi_effect_update(&r,attachment,1,60,height,&q)==0);
 assert(word(r.bytes+8)==0x80000000u&&r.bytes[0x35]==3);
 r.bytes[0x35]=255;put(r.bytes+0x40,0);
 assert(taxi_effect_update(&r,attachment,0,0x00fffff9,height,&q)==0);
 assert(r.bytes[0x35]==0&&word(r.bytes+0x40)==UINT32_MAX);
 for(unsigned old=0;old<65536;old++){
  r.bytes[0x36]=(uint8_t)old;r.bytes[0x37]=(uint8_t)(old>>8);
  assert(taxi_effect_update(&r,attachment,1,60,height,&q)==0);
  unsigned expected=(old+640)&65535;
  if(expected>24576&&expected<32768)expected=24576;
  assert(((unsigned)r.bytes[0x36]|(unsigned)r.bytes[0x37]<<8)==expected);
 }
 put(r.bytes+0x40,0x7fffffffu);
 assert(taxi_effect_update(&r,attachment,-1,60,height,&q)==0);
 assert(word(r.bytes+0x40)==0);
 TaxiEffectRecord saved=r;q.result=-1;
 assert(taxi_effect_update(&r,attachment,1,60,height,&q)==-1);
 assert(memcmp(&saved,&r,sizeof r)==0);
 assert(taxi_effect_update(NULL,attachment,1,60,height,&q)==-1);
 const uint32_t tags[]={0x080218ad,0x08021a05,0x08021ab1,0x08021b5d};
 for(unsigned i=0;i<4;i++){
  memset(&r,0,sizeof r);put(r.bytes+0x28,UINT32_MAX);r.bytes[0x36]=48;r.bytes[0x37]=117; /* 30000 */
  int calls=q.calls;
  assert(taxi_effect_update_tag(&r,tags[i],attachment,1,63,NULL,NULL)==1);
  assert(q.calls==calls&&word(r.bytes+8)==998);
  assert(word(r.bytes+0x40)==(i==3?7u:15u));
  unsigned size=(unsigned)r.bytes[0x36]|(unsigned)r.bytes[0x37]<<8;
  assert(size==(i==3?30640u:24576u));
 }
 saved=r;
 assert(taxi_effect_update_tag(&r,0xdeadbeef,attachment,1,60,height,&q)==-1);
 assert(memcmp(&r,&saved,sizeof r)==0);
 puts("Effect update: position, height, animation, all size patterns and rollback PASS");
 return 0;
}

