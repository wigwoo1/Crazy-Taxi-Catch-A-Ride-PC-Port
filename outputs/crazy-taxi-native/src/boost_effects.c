#include "boost_effects.h"
#include <string.h>
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int valid(const TaxiBoostEffects *s){
 if(!s||s->ring>=9)return 0;
 for(unsigned i=0;i<9;i++){
  const uint8_t *b=s->pool.records[i].bytes;
  if(b[0x34]&&(b[0x34]!=5||word(b+0x3c)!=0x08021959||word(b)>1))return 0;
 }
 return 1;
}
int taxi_boost_effects_cold_init(TaxiBoostEffects *s){
 if(!s)return -1;
 memset(&s->body,0,sizeof s->body);
 return taxi_effect_pool_cold_init(&s->pool,&s->ring,&s->timer);
}
int taxi_boost_effects_session_reset(TaxiBoostEffects *s){
 return s?taxi_effect_pool_reset(&s->pool,&s->ring):-1;
}
typedef struct {const TaxiBodyPoints *body;int32_t delta,clock;TaxiEffectHeight height;void *context;int error;} Update;
static void update(void *opaque,TaxiEffectPool *pool,unsigned index,uint32_t tag){
 Update *u=opaque;(void)tag;
 unsigned attachment=word(pool->records[index].bytes);
 if(taxi_effect_update(&pool->records[index],u->body->positions[attachment+2],
  u->delta,(uint32_t)u->clock,u->height,u->context)<0)u->error=1;
}
static void emit(void *opaque,TaxiEffectPool *pool,unsigned type){(void)opaque;(void)pool;(void)type;}
int taxi_boost_effects_step(TaxiBoostEffects *s,const TaxiBodyPoints *body,
 TaxiBoostEffect input,TaxiEffectHeight height,void *context){
 if(!valid(s)||!body||!height)return -1;
 TaxiBoostEffects next=*s;next.body=*body;input.timer=next.timer;
 input.attachment_tags[0]=0;input.attachment_tags[1]=1;
 input.velocity_basis[0]=body->vectors[2];input.velocity_basis[1]=body->vectors[3];
 int spawned=taxi_effect_emit_boost(&next.pool,&next.ring,&input);
 if(spawned<0)return -1;
 next.timer=input.timer;
 Update u={body,input.delta,input.clock,height,context,0};
 if(taxi_effect_pool_step(&next.pool,input.delta,update,emit,&u)<0||u.error)return -1;
 *s=next;return spawned;
}
int taxi_boost_effects_sprites(const TaxiBoostEffects *s,TaxiPoint camera,
 int16_t pitch,int16_t yaw,int16_t roll,const TaxiTrig *trig,int32_t focal,
 int32_t cx,int32_t cy,TaxiEffectSprite out[9]){
 if(!valid(s)||!trig||!out||focal<=0)return -1;
 TaxiEffectSprite requests[9];int count=0;
 for(unsigned i=0;i<9;i++){
  int result=taxi_effect_world_sprite(&s->pool.records[i],camera,pitch,yaw,roll,
   trig,focal,cx,cy,0,1,NULL,NULL,&requests[count]);
  if(result<0)return -1;
  count+=result;
 }
 memcpy(out,requests,(size_t)count*sizeof *out);return count;
}
