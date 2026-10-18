#include "actor_effects.h"
#include <string.h>
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int valid(const TaxiActorEffects *s){
 if(!s||s->ring>=9)return 0;
 for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)if(s->pool.slots[g][j]<-1||s->pool.slots[g][j]>8)return 0;
 for(unsigned i=0;i<9;i++){
  const uint8_t *b=s->pool.records[i].bytes;if(!b[0x34])continue;
  if(b[0x34]<1||b[0x34]>5)return 0;
  uint32_t tag=word(b+0x3c);
  if(word(b)>5||(tag!=0x080218ad&&tag!=0x08021959&&tag!=0x08021a05&&tag!=0x08021ab1))return 0;
 }
 return 1;
}
int taxi_actor_effects_cold_init(TaxiActorEffects *s){
 if(!s)return -1;
 memset(s,0,sizeof *s);
 return taxi_effect_pool_cold_init(&s->pool,&s->ring,&s->timers[3]);
}
int taxi_actor_effects_session_reset(TaxiActorEffects *s){return s?taxi_effect_pool_reset(&s->pool,&s->ring):-1;}
int taxi_actor_effects_impact(TaxiActorEffects *s,unsigned slot,uint8_t alternate,
 int32_t delta,int32_t clock){
 if(!valid(s)||slot>=6)return -1;
 TaxiImpactEffect input={.alternate=alternate,.timer=s->impact_timer,.delta=delta,
 .clock=clock,.attachment_tag=slot,.velocity_basis=s->body.vectors[slot]};
 int result=taxi_effect_emit_impact(&s->pool,&s->ring,&input);
 if(result>=0)s->impact_timer=input.timer;
 return result;
}
typedef struct {TaxiActorEffects *state;TaxiActorEffectInput input;TaxiEffectHeight height;void *context;int error;} Frame;
static int surface(Frame *f,unsigned type){
 TaxiActorEffects *s=f->state;TaxiActorEffectInput *i=&f->input;
 TaxiSurfaceEffect request={.actor_flags=i->actor_flags,.material_flags=i->material_flags,
 .alternate=i->alternate,.speed=i->speed,.timer=s->timers[type-1],.clock=i->clock};
 for(unsigned p=0;p<3;p++){request.attachment_tags[p]=p+2;request.velocity_basis[p]=s->body.vectors[p+2];}
 int result=taxi_effect_emit_surface(&s->pool,&s->ring,type,&request);
 if(result>=0)s->timers[type-1]=request.timer;
 return result;
}
static void expired(void *ctx,TaxiEffectPool *pool,unsigned type){
 Frame *f=ctx;(void)pool;if(surface(f,type)<0)f->error=1;
}
static void update(void *ctx,TaxiEffectPool *pool,unsigned index,uint32_t tag){
 Frame *f=ctx;unsigned point=word(pool->records[index].bytes);
 if(point>5||taxi_effect_update_tag(&pool->records[index],tag,f->state->body.positions[point],
 f->input.delta,(uint32_t)f->input.clock,f->height,f->context)<0)f->error=1;
}
int taxi_actor_effects_step(TaxiActorEffects *state,const TaxiBodyPoints *body,
 TaxiActorEffectInput i,TaxiEffectHeight height,void *context){
 if(!valid(state)||!body||!height)return -1;
 TaxiActorEffects s=*state;s.body=*body;
 TaxiBoostEffect common={.actor_flags=i.actor_flags,.material_flags=i.material_flags,
 .alternate=i.alternate,.action2=i.action2,.scale=i.scale,.delta=i.delta,.clock=i.clock,
 .attachment_tags={2,3},.velocity_basis={body->vectors[2],body->vectors[3]}};
 TaxiSmokeEffect smoke={.common=common,.speed=i.speed,.state_2630=i.state_2630,
 .phase_e6a=i.phase_e6a,.voice_25d4=i.voice_25d4};
 smoke.common.timer=s.timers[0];
 int a=taxi_effect_emit_smoke(&s.pool,&s.ring,&smoke);if(a<0)return -1;s.timers[0]=smoke.common.timer;
 common.timer=s.timers[3];
 int b=taxi_effect_emit_boost(&s.pool,&s.ring,&common);if(b<0)return -1;s.timers[3]=common.timer;
 Frame f={&s,i,height,context,0};
 int c=surface(&f,2),d=surface(&f,3);if(c<0||d<0)return -1;
 if(taxi_effect_pool_step(&s.pool,i.delta,update,expired,&f)<0||f.error)return -1;
 *state=s;return a+b+c+d;
}
int taxi_actor_effects_advance(TaxiActorEffects *state,TaxiActorEffectInput input,TaxiEffectHeight height,void *context){
 if(!valid(state)||!height)return -1;
 TaxiActorEffects s=*state;Frame f={&s,input,height,context,0};
 if(taxi_effect_pool_step(&s.pool,input.delta,update,expired,&f)<0||f.error)return -1;
 *state=s;return 0;
}
int taxi_actor_effects_sprites(const TaxiActorEffects *s,TaxiPoint camera,
 int16_t pitch,int16_t yaw,int16_t roll,const TaxiTrig *trig,int32_t focal,
 int32_t cx,int32_t cy,int32_t phase,int disabled,TaxiEffectRandom random,
 void *context,TaxiEffectSprite out[9]){
 if(!valid(s)||!trig||!out||focal<=0)return -1;
 TaxiEffectSprite requests[9];int count=0;
 for(unsigned j=0;j<9;j++){
  int result=taxi_effect_world_sprite(&s->pool.records[j],camera,pitch,yaw,roll,trig,
   focal,cx,cy,phase,disabled,random,context,&requests[count]);
  if(result<0)return -1;count+=result;
 }
 memcpy(out,requests,(size_t)count*sizeof *out);return count;
}
