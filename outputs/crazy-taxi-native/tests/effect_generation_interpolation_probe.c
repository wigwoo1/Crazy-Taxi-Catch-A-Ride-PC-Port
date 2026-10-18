#include "../src/effect_emit.h"
#include "../src/effect_presentation_interpolation.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
static unsigned observed,slots[9];
static void observe(void *ctx,const TaxiEffectPool *p,unsigned slot){(void)ctx;assert(slot<9&&p->records[slot].bytes[52]);observed++;slots[slot]++;}
static int emit(unsigned kind,TaxiEffectPool *pool,unsigned *ring){
 TaxiBoostEffect boost={.scale=8192,.action2=1,.delta=1,.clock=60,.attachment_tags={2,3},.velocity_basis={{1,2,3},{4,5,6}}};
 if(kind==0)return taxi_effect_emit_boost(pool,ring,&boost);
 TaxiSmokeEffect smoke={.common=boost,.voice_25d4=1,.speed=60000};smoke.common.scale=4096;if(kind==1)return taxi_effect_emit_smoke(pool,ring,&smoke);
 TaxiSurfaceEffect surface={.speed=60000,.clock=60,.material_flags=6,.attachment_tags={2,3,4},.velocity_basis={{1,2,3},{4,5,6},{7,8,9}}};
 if(kind==2||kind==3)return taxi_effect_emit_surface(pool,ring,kind,pool?&surface:0);
 TaxiImpactEffect impact={.delta=1,.clock=60,.attachment_tag=5,.velocity_basis={1,2,3}};return taxi_effect_emit_impact(pool,ring,&impact);
}
int main(void){unsigned emissions=0,cases=0;
 for(unsigned kind=0;kind<5;kind++)for(unsigned start=0;start<9;start++){
  TaxiEffectPool a={0},b;unsigned ra=start,rb=start;int32_t timer;unsigned dummy;assert(!taxi_effect_pool_cold_init(&a,&dummy,&timer));b=a;
  taxi_effect_emit_observer(0,0);int expected=emit(kind,&a,&ra);assert(expected>0);
  observed=0;memset(slots,0,sizeof slots);taxi_effect_emit_observer(observe,0);assert(emit(kind,&b,&rb)==expected);taxi_effect_emit_observer(0,0);
  assert(observed==(unsigned)expected&&ra==rb&&!memcmp(&a,&b,sizeof a));for(unsigned n=0;n<9;n++)assert(slots[n]==((n+9-start)%9<(unsigned)expected));emissions+=observed;cases++;
 }
 TaxiEffectPresentationSlot a={0},b={0};a.visible=b.visible=1;a.record.bytes[52]=b.record.bytes[52]=2;
 a.unjittered.position=(TaxiPoint){100,80,1000};b.unjittered.position=(TaxiPoint){140,100,2000};a.sprite=a.unjittered;b.sprite=b.unjittered;
 a.sprite.scale=100;b.sprite.scale=200;b.sprite.position.x+=3;b.sprite.position.y-=6;b.sprite.frame=2;b.sprite.group=9;b.sprite.palette=1;
 TaxiEffectPresentationSlot olda=a,oldb=b;unsigned blended=0;
 for(unsigned i=0;i<=100;i++){int flag;double alpha=i/100.;TaxiEffectSprite out=taxi_effect_presentation_interpolate(&a,&b,1,1,alpha,&flag);assert(out.frame==2&&out.group==9&&out.palette==1);
  if(i==100){assert(!flag&&!memcmp(&out,&b.sprite,sizeof out));}else{assert(flag);assert(out.position.x==(int)(100+40*alpha)+3&&out.position.y==(int)(80+20*alpha)-6&&out.position.z==(int)(1000+1000*alpha));assert(out.scale==(int)(100+100*alpha));blended++;}
  TaxiEffectSprite snapped=taxi_effect_presentation_interpolate(&a,&b,1,2,alpha,&flag);assert(!flag&&!memcmp(&snapped,&b.sprite,sizeof snapped));
 }
 for(unsigned i=0;i<4;i++){int flag;double invalid[]={-1,1,2,NAN};TaxiEffectSprite out=taxi_effect_presentation_interpolate(&a,&b,1,1,invalid[i],&flag);assert(!flag&&!memcmp(&out,&b.sprite,sizeof out));}
 int flag;TaxiEffectSprite out=taxi_effect_presentation_interpolate(&a,&b,0,0,.5,&flag);assert(!flag&&!memcmp(&out,&b.sprite,sizeof out));a.visible=0;out=taxi_effect_presentation_interpolate(&a,&b,1,1,.5,&flag);assert(!flag&&!memcmp(&out,&b.sprite,sizeof out));a=olda;assert(!memcmp(&a,&olda,sizeof a)&&!memcmp(&b,&oldb,sizeof b));
 printf("PASS emission_cases=%u observed_overwrites=%u interpolation_samples=%u reused_slot_snaps=101\n",cases,emissions,blended);return 0;
}
