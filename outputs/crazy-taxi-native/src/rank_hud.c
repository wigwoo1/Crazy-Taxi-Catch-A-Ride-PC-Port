#include "rank_hud.h"
#include "obj_tiles_rgba.h"
#include <string.h>
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|(unsigned)p[1]<<8);}
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int32_t signed_bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
static int32_t asr8(int32_t v){uint32_t u=(uint32_t)v;return signed_bits((u>>8)|(u&0x80000000u?0xff000000u:0));}
int taxi_rank_hud_snapshot(TaxiRankHudSnapshot *out,const TaxiRankDisplayOwner *o){
 if(!out||!o||!o->scalar_16c0||!o->cache_5300)return -1;
 TaxiRankHudSnapshot s={0};s.active=o->scalar_16c0[0x15];
 s.position=signed_bits(word(o->scalar_16c0+0x18));
 s.current[0]=half(o->cache_5300+0x88);s.current[1]=half(o->cache_5300+0x8a);
 *out=s;return 0;
}
int taxi_rank_hud_plan(TaxiRankHudPlan *out,const TaxiRankHudSnapshot *s,uint8_t language){
 if(!out||!s||(s->active&&language>=5))return -1;
 TaxiRankHudPlan p={0};
 if(s->active){
  p.active=1;p.descriptor=0x087ec33cu+8u*language;p.x=asr8(s->position);p.y=88;
  for(unsigned i=0;i<2;i++){
   TaxiRankHudSprite *r=p.sprites+i;
   r->source=0x080c7d8c;r->index=s->current[i];r->frame=9+i;r->group=6;
   r->x=p.x+(i?19:10);r->y=100;
  }
 }
 *out=p;return 0;
}
int taxi_rank_hud_enqueue(TaxiSpriteReplacementOwner *owner,const TaxiRankHudPlan *p){
 if(!owner||!owner->copies||!owner->immediate||!p)return -1;
 if(!p->active)return 0;
 TaxiSpriteCopies copies=*owner->copies;TaxiSpriteImmediateCopy dma=*owner->immediate;
 TaxiSpriteReplacementOwner next=*owner;next.copies=&copies;next.immediate=&dma;
 for(unsigned i=0;i<2;i++){
  const TaxiRankHudSprite *r=p->sprites+i;
  if(r->source!=0x080c7d8cu||r->frame!=9+i||r->group!=6||r->immediate)return -1;
  if(taxi_sprite_replace_owned(&next,r->source,r->index,r->frame,r->group,r->immediate)<0)return -1;
 }
 *owner->copies=copies;*owner->immediate=dma;return 0;
}
int taxi_rank_hud_decode(uint8_t out[2][512],const TaxiOrdinaryGraphicsOwners *g){
 if(!out||!g||!g->assets||!g->banks)return -1;
 /* Original group6 frames9/10 are 1x2 4bpp tiles at indices18/20. */
 uint32_t base=g->assets->bases[6];
 if(base<0x06010000u||base>0x06017d40u||(base&31))return -1;
 uint8_t next[2][512];
 for(unsigned i=0;i<2;i++){
  uint32_t tile=(base-0x06010000u)/32+18+2*i;
  if(taxi_obj_tiles_rgba(next[i],512,g->assets,g->banks,g->destination,(uint16_t)(tile|(10u<<12)),8,16))return -1;
 }
 memcpy(out,next,sizeof next);return 0;
}
