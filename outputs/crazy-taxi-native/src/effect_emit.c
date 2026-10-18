#include "effect_emit.h"
#include <string.h>
static TaxiEffectEmitObserver observer;static void *observer_context;
void taxi_effect_emit_observer(TaxiEffectEmitObserver callback,void *context){observer=callback;observer_context=context;}
static void emitted(const TaxiEffectPool *pool,unsigned slot){if(observer)observer(observer_context,pool,slot);}
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static void put(uint8_t *p,uint32_t u){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(u>>(i*8));}
static int32_t asr(uint32_t u,unsigned n){return bits((u>>n)|((u&0x80000000u)?(~0u<<(32-n)):0));}
int taxi_effect_emit_impact(TaxiEffectPool *pool,unsigned *ring,TaxiImpactEffect *s){
 if(!pool||!ring||!s||*ring>=9)return -1;
 if(s->alternate)return 0;
 s->timer=bits((uint32_t)s->timer-(uint32_t)s->delta);
 if(s->timer>0)return 0;
 s->timer=bits((uint32_t)s->timer+(uint32_t)(s->clock/8));
 unsigned emitted_slot=*ring;uint8_t *b=pool->records[emitted_slot].bytes;*ring=(*ring+1)%9;
 put(b,s->attachment_tag);b[0x34]=4;b[0x35]=0;b[0x36]=0;b[0x37]=64;
 uint16_t life=(uint16_t)((uint32_t)s->clock>>1);
 b[0x3a]=(uint8_t)life;b[0x3b]=(uint8_t)(life>>8);
 put(b+0x3c,0x08021ab1);put(b+0x40,0);
 put(b+0x14,0);put(b+0x18,0);put(b+0x1c,0);
 put(b+0x24,(uint32_t)s->velocity_basis.x*9u);
 put(b+0x28,(uint32_t)s->velocity_basis.y*9u);
 put(b+0x2c,(uint32_t)s->velocity_basis.z*9u);emitted(pool,emitted_slot);
 return 1;
}
int taxi_effect_emit_surface(TaxiEffectPool *pool,unsigned *ring,unsigned type,TaxiSurfaceEffect *s){
 if(!pool||!ring||!s||*ring>=9||(type!=2&&type!=3))return -1;
 int *slots=pool->slots[type-2];
 for(unsigned i=0;i<6;i++)if(slots[i]<-1||slots[i]>8)return -1;
 int32_t magnitude=s->speed<0?bits(0u-(uint32_t)s->speed):s->speed;
 if((s->actor_flags&8)||s->alternate||!(s->material_flags&(type==2?2:4))||magnitude<=54900)return 0;
 s->timer=bits((uint32_t)s->timer+(uint32_t)(s->clock/8));
 int count=0;
 for(unsigned pair=0;pair<3;pair++){
  if(slots[pair*2]!=-1&&slots[pair*2+1]!=-1)continue;
  for(unsigned member=0;member<2;member++){
   unsigned index=*ring;uint8_t *b=pool->records[index].bytes;*ring=(index+1)%9;
   put(b,s->attachment_tags[pair]);b[0x34]=(uint8_t)type;b[0x35]=0;b[0x36]=0;b[0x37]=64;
   b[0x3a]=(uint8_t)s->clock;b[0x3b]=(uint8_t)((uint32_t)s->clock>>8);
   put(b+0x3c,type==3&&pair<2?0x08021ab1:0x08021a05);
   put(b+0x40,0);put(b+0x14,0);put(b+0x18,0);put(b+0x1c,0);
   put(b+0x24,(uint32_t)asr((uint32_t)s->velocity_basis[pair].x*3072u,12));
   put(b+0x28,(uint32_t)asr((uint32_t)s->velocity_basis[pair].y*3072u,12));
   put(b+0x2c,(uint32_t)asr((uint32_t)s->velocity_basis[pair].z*3072u,12));
   slots[pair*2+member]=(int)index;count++;emitted(pool,index);
  }
 }
 return count;
}
int taxi_effect_emit_smoke(TaxiEffectPool *pool,unsigned *ring,TaxiSmokeEffect *s){
 if(!pool||!ring||!s||*ring>=9)return -1;
 TaxiBoostEffect *c=&s->common;
 int special=s->state_2630!=0&&s->phase_e6a==1;
 if(!special&&((c->actor_flags&8)||(c->material_flags&6)||c->alternate))return 0;
 if(!(s->voice_25d4&&c->scale<=4096)&&!special)return 0;
 c->timer=bits((uint32_t)c->timer-(uint32_t)c->delta);
 if(c->timer>0)return 0;
 c->timer=bits((uint32_t)c->timer+(uint32_t)(c->clock/4));
 int32_t q=s->speed/1138;
 int32_t factor=special?asr((uint32_t)q*7u*512u,12):bits((uint32_t)asr((uint32_t)q,2)*9u);
 for(unsigned i=0;i<2;i++){
  unsigned emitted_slot=*ring;uint8_t *b=pool->records[emitted_slot].bytes;*ring=(*ring+1)%9;
  put(b,c->attachment_tags[i]);b[0x34]=1;b[0x35]=0;b[0x36]=0;b[0x37]=64;
  b[0x3a]=(uint8_t)c->clock;b[0x3b]=(uint8_t)((uint32_t)c->clock>>8);
  put(b+0x3c,0x080218ad);put(b+0x40,0);
  put(b+0x14,0);put(b+0x18,0);put(b+0x1c,0);
  put(b+0x24,(uint32_t)c->velocity_basis[i].x*(uint32_t)factor);
  put(b+0x28,(uint32_t)c->velocity_basis[i].y*(uint32_t)factor);
  put(b+0x2c,(uint32_t)c->velocity_basis[i].z*(uint32_t)factor);emitted(pool,emitted_slot);
 }
 return 2;
}
int taxi_effect_emit_boost(TaxiEffectPool *pool,unsigned *ring,TaxiBoostEffect *s){
 if(!pool||!ring||!s||*ring>=9)return -1;
 if((s->actor_flags&8)||(s->material_flags&2)||s->alternate||s->scale<=4096||!s->action2)return 0;
 s->timer=bits((uint32_t)s->timer-(uint32_t)s->delta);
 if(s->timer>0)return 0;
 s->timer=bits((uint32_t)s->timer+(uint32_t)(s->clock/4));
 for(unsigned i=0;i<2;i++){
  unsigned emitted_slot=*ring;uint8_t *b=pool->records[emitted_slot].bytes;
  *ring=(*ring+1)%9;
  put(b,s->attachment_tags[i]);b[0x34]=5;b[0x35]=0;
  b[0x36]=0;b[0x37]=64;b[0x3a]=(uint8_t)s->clock;b[0x3b]=(uint8_t)((uint32_t)s->clock>>8);
  put(b+0x3c,0x08021959);put(b+0x40,0);
  put(b+0x14,0);put(b+0x18,0);put(b+0x1c,0);
  put(b+0x24,(uint32_t)s->velocity_basis[i].x*40u);
  put(b+0x28,(uint32_t)s->velocity_basis[i].y*40u);
  put(b+0x2c,(uint32_t)s->velocity_basis[i].z*40u);emitted(pool,emitted_slot);
 }
 return 2;
}
