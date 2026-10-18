#include "challenge_flag.h"
#include <string.h>
static int16_t half(uint16_t u){int16_t s;memcpy(&s,&u,2);return s;}
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t absolute_difference(int32_t a,int32_t b){int32_t d=bits((uint32_t)a-(uint32_t)b);return d<0?bits(0u-(uint32_t)d):d;}
void taxi_challenge_flag_begin(TaxiChallengeFlag *s){*s=(TaxiChallengeFlag){0,1200,256};}
/* OriginalA9DC fields after sharedABC4, and7FAE..80E2 scalar projection.
   Caller owns10B54/BA04/pools, rank9AB0, palette DMA and flag actor. */
void taxi_challenge_flag_step(TaxiChallengeFlag *s,const TaxiChallengeFlagInput *in,TaxiChallengeFlagStatus *status,TaxiChallengeFlagEffects *e){
 *e=(TaxiChallengeFlagEffects){.sound_index=-1};
 if(in->phase153a||in->outer1ea8)return;
 if(!s->caught_1550&&!in->intro1696&&!status->ended1697&&!in->pause5f20){
  int before=s->remaining_1552/60;
  s->remaining_1552=half((uint16_t)((uint32_t)(uint16_t)s->remaining_1552-(uint32_t)in->delta));
  if(s->remaining_1552<=0){s->remaining_1552=0;status->success153c=0;status->ended1697=1;}
  if(before>9&&s->remaining_1552/60<=9){e->warning_palette=1;if(in->effects_enabled)e->sound_index=39;}
 }
 if(s->remaining_1552&&!status->success153c&&!in->pause5f20&&absolute_difference(in->flag_x,in->car_x)<=767&&absolute_difference(in->flag_z,in->car_z)<=767){
  if(in->effects_enabled)e->sound_index=42;
  s->caught_1550=1;status->success153c=status->ended1697=1;e->request_rank=1;
 }
 if(s->caught_1550){s->scale_1554=half((uint16_t)((uint32_t)(uint16_t)s->scale_1554-(uint32_t)in->delta));if(s->scale_1554<=16)e->hide_flag=1;}
}

static int32_t read32(const uint8_t *p){return bits(p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24));}
int taxi_challenge_flag_render_request(uint8_t p[44],int32_t px,int32_t pz,int32_t y,int16_t scale,uint32_t factor,TaxiChallengeFlagRenderRequest *out){
 if(!p||!out)return -1;
 int8_t kind;memcpy(&kind,p+1,1);
 int32_t x=read32(p+16),z=read32(p+24);
 if(absolute_difference(px,x)>25599||absolute_difference(pz,z)>25599||kind>7||(p[0]&3)!=1)return 0;
 if(!factor)factor=0xb1400u;
 uint32_t raw=((uint32_t)(int32_t)scale<<1)*factor;
 int32_t scaled=bits((raw>>8)|((raw&0x80000000u)?0xff000000u:0));
 TaxiChallengeFlagRenderRequest request={x,y,z,scaled,bits((uint32_t)(int32_t)scale<<4),0x0849a6c1};
 p[32]=32;p[33]=64;p[34]=5;p[35]=6;
 for(unsigned i=0;i<4;i++)p[36+i]=(uint8_t)(request.bitmap_identity>>(8*i));
 *out=request;return 1;
}

static int32_t shift(uint32_t u,unsigned n){return bits((u>>n)|((u&0x80000000u)?(UINT32_MAX<<(32-n)):0));}
int taxi_challenge_flag_corners(const TaxiChallengeFlagRenderRequest *r,TaxiPoint center,const TaxiTrig *trig,TaxiPoint out[4]){
 if(!r||!trig||!out)return -1;
 int32_t half_width=shift((uint32_t)r->scale*32u,17),height=shift((uint32_t)r->scale*64u,16);
 int32_t nx=bits(0u-(uint32_t)half_width);
 TaxiPoint points[4]={{nx,0,0},{nx,height,0},{half_width,height,0},{half_width,0,0}};
 unsigned angle=(uint32_t)r->rotation_angle&4095u;
 uint32_t c=(uint32_t)trig->cosine[angle],s=(uint32_t)trig->sine[angle];
 for(unsigned i=0;i<4;i++){
  uint32_t x=(uint32_t)points[i].x,y=(uint32_t)points[i].y;
  points[i].x=bits((uint32_t)center.x+(uint32_t)shift(x*c-y*s,8));
  points[i].y=bits((uint32_t)center.y+(uint32_t)shift(x*s+y*c,8));
  points[i].z=center.z;
 }
 memcpy(out,points,sizeof points);return 0;
}
