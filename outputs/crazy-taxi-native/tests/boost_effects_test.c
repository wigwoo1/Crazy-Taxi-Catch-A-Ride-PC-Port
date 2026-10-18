#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "boost_effects.h"
typedef struct {int calls,result;TaxiPoint points[2];} Query;
static int height(void *ctx,TaxiPoint p,int32_t *out){Query *q=ctx;
 if(q->calls<2)q->points[q->calls]=p;q->calls++;*out=-256;return q->result;
}
int main(void){
 static TaxiTrig trig;trig.cosine[0]=256;
 TaxiBoostEffects s;memset(&s,0xa5,sizeof s);
 assert(taxi_boost_effects_cold_init(&s)==0);
 TaxiBodyPoints body;
 assert(taxi_body_points(&body,(TaxiPoint){0,0,1000},0,0,0,0,&trig)==1);
 TaxiBoostEffect in={.scale=4710,.action2=1,.delta=1,.clock=60};
 Query q={.result=1};
 assert(taxi_boost_effects_step(&s,&body,in,height,&q)==2);
 assert(s.ring==2&&s.timer==14&&q.calls==2);
 assert(q.points[0].x==-182&&q.points[0].y==0&&q.points[0].z==612);
 assert(q.points[1].x==181&&q.points[1].z==612);
 assert(s.pool.records[0].bytes[0x3a]==59&&s.pool.records[0].bytes[0x35]==1);
 TaxiEffectSprite requests[9];
 assert(taxi_boost_effects_sprites(&s,(TaxiPoint){0},0,0,0,&trig,100,120,80,requests)==2);
 assert(requests[0].group==10&&requests[0].frame==6);
 assert(requests[0].position.x==91&&requests[0].position.z==612);
 TaxiBoostEffects saved=s;q.result=-1;
 assert(taxi_boost_effects_step(&s,&body,in,height,&q)==-1);
 assert(memcmp(&saved,&s,sizeof s)==0);
 q.result=1;in.action2=0;
 for(unsigned i=0;i<59;i++)assert(taxi_boost_effects_step(&s,&body,in,height,&q)==0);
 assert(taxi_boost_effects_sprites(&s,(TaxiPoint){0},0,0,0,&trig,100,120,80,requests)==0);
 assert(s.timer==14); /* Disabled emitter doesn't consume its timer. */
 s.pool.slots[0][0]=4;
 assert(taxi_boost_effects_session_reset(&s)==0&&s.timer==14&&s.pool.slots[0][0]==4);
 s.pool.records[0].bytes[0x34]=2;saved=s;
 assert(taxi_boost_effects_step(&s,&body,in,height,&q)==-1);
 assert(memcmp(&saved,&s,sizeof s)==0); /* Unsupported source owner is explicit. */
 puts("Boost composition: body attachments, same-frame lifetime/update, projection, expiry and rollback PASS");
 return 0;
}
