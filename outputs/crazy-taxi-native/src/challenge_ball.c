#include "challenge_ball.h"
#include <string.h>
static int32_t signed_bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t read32(const uint8_t *p){return signed_bits((uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24);}
static void write32(uint8_t *p,int32_t v){uint32_t u=(uint32_t)v;for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(u>>(i*8));}
void taxi_challenge_ball_begin(TaxiChallengeBall *s){*s=(TaxiChallengeBall){.remaining_1558=2700};}
static int32_t decay(int32_t v,int32_t delta){
 uint32_t amount=(uint32_t)delta<<5;
 if(v>0){int32_t next=signed_bits((uint32_t)v-amount);return next>0?next:0;}
 if(v<0){int32_t next=signed_bits((uint32_t)v+amount);return next<0?next:0;}
 return 0;
}
int taxi_challenge_ball_motion_step(TaxiChallengeBall *s,uint8_t actor[44],unsigned index,int pause,int32_t delta){
 if(!s||!actor||index>=20)return -1;
 int32_t *v=s->velocity_155c[index];
 if(!pause){write32(actor+16,signed_bits((uint32_t)read32(actor+16)+(uint32_t)(v[0]/60)));write32(actor+24,signed_bits((uint32_t)read32(actor+24)+(uint32_t)(v[2]/60)));}
 v[0]=decay(v[0],delta);v[2]=decay(v[2],delta);return 0;
}

static int32_t asr(uint32_t u,unsigned n){return signed_bits((u>>n)|((u&0x80000000u)?(UINT32_MAX<<(32-n)):0));}
int32_t taxi_challenge_ball_impulse_angle(int32_t z,int32_t x){
 unsigned sign=0;int32_t a=z,b=x;
 if(a<0){sign|=1;a=signed_bits(0u-(uint32_t)a);}else if(!a)a=1;
 if(b<0){sign|=2;b=signed_bits(0u-(uint32_t)b);}else if(!b)b=1;
 int32_t numerator=signed_bits((uint32_t)(a>b?b:a)<<16),denominator=a>b?a:b;
 int32_t ratio=signed_bits((uint32_t)((int64_t)numerator/denominator));
 int32_t angle=a>b?asr((uint32_t)ratio,7):signed_bits(1024u-(uint32_t)asr((uint32_t)ratio,7));
 if(sign==2)angle=signed_bits(0u-(uint32_t)angle);
 else if(sign==1)angle=signed_bits(2048u-(uint32_t)angle);
 else if(sign==3)angle=signed_bits((uint32_t)angle+2048u);
 return angle;
}
int taxi_challenge_ball_impulse(TaxiChallengeBall *s,const uint8_t actors[20][44],unsigned popped,const int32_t xt[4096],const int32_t zt[4096],TaxiBallRoot root){
 if(!s||!actors||popped>=20||!xt||!zt||!root)return -1;
 for(unsigned i=0;i<20;i++){
  if(i==popped||!(actors[i][0]&1)||actors[i][10])continue;
  int32_t x=signed_bits((uint32_t)read32(actors[i]+16)-(uint32_t)read32(actors[popped]+16));
  int32_t z=signed_bits((uint32_t)read32(actors[i]+24)-(uint32_t)read32(actors[popped]+24));
  uint32_t squared=(uint32_t)x*(uint32_t)x+(uint32_t)z*(uint32_t)z;
  if(signed_bits(squared)>0x270fffff)continue;
  int32_t strength=(25600-(int32_t)(root(squared)&65535u))/10;
  unsigned angle=(uint32_t)taxi_challenge_ball_impulse_angle(z,x)&4095u;
  s->velocity_155c[i][0]=asr((uint32_t)xt[angle]*(uint32_t)strength,8);
  s->velocity_155c[i][2]=asr((uint32_t)zt[angle]*(uint32_t)strength,8);
 }
 return 0;
}

static int16_t half_bits(uint16_t u){int16_t s;memcpy(&s,&u,2);return s;}
static int32_t absdiff(int32_t a,int32_t b){int32_t d=signed_bits((uint32_t)a-(uint32_t)b);return d<0?signed_bits(0u-(uint32_t)d):d;}
int taxi_challenge_ball_step(TaxiChallengeBall *s,uint8_t actors[20][44],const TaxiBallInput *in,TaxiBallStatus *status,TaxiBallEffects *e,const int32_t xt[4096],const int32_t zt[4096],TaxiBallRoot root){
 if(!s||!actors||!in||!status||!e||!xt||!zt||!root)return -1;
 *e=(TaxiBallEffects){0};
 if(in->phase){e->render_world=in->phase==1;return 0;}if(in->outer)return 0;
 if(!status->success&&!in->intro&&!status->ended&&!in->pause){
  int before=s->remaining_1558/60;
  s->remaining_1558=half_bits((uint16_t)((uint32_t)(uint16_t)s->remaining_1558-(uint32_t)in->delta));
  if(s->remaining_1558<=0){s->remaining_1558=0;status->success=0;status->ended=1;}
  if(before>9&&s->remaining_1558/60<=9){e->warning_palette=1;e->warning_sound=!!in->effects_enabled;}
 }
 if(!s->remaining_1558||status->success||in->pause)return 0;
 for(unsigned i=0;i<20;i++){
  uint8_t *a=actors[i];if(!(a[0]&1)||a[10])continue;
  if(absdiff(read32(a+16),in->car_x)<=767&&absdiff(read32(a+24),in->car_z)<=767){
   a[10]=1;a[12]=15;a[13]=0;a[9]=15;s->popped_1556=half_bits((uint16_t)((uint16_t)s->popped_1556+1));
   e->popped_mask|=1u<<i;if(in->effects_enabled)e->pop_sound_count++;
   if(s->popped_1556==20){status->success=status->ended=1;e->request_rank=1;}
   else taxi_challenge_ball_impulse(s,(const uint8_t (*)[44])actors,i,xt,zt,root);
  }
  taxi_challenge_ball_motion_step(s,a,i,in->pause,in->delta);
 }
 return 0;
}

int taxi_challenge_ball_render_request(uint8_t a[44],int32_t px,int32_t pz,int32_t y,int pause,int32_t delta,uint32_t factor,TaxiBallRenderRequest *out){
 if(!a||!out)return -1;
 int8_t kind;memcpy(&kind,a+1,1);int32_t x=read32(a+16),z=read32(a+24);
 if(absdiff(px,x)>25599||absdiff(pz,z)>25599||kind>7||(a[0]&3)!=1)return 0;
 uint32_t bitmap=0x0849b0c9u+((uint32_t)(int32_t)kind<<10);
 a[32]=a[33]=32;a[34]=a[35]=5;
 int8_t popped;memcpy(&popped,a+10,1);
 if(popped==1){
  int8_t frame;memcpy(&frame,a+8,1);bitmap=0x0849b0c9u+((uint32_t)((int32_t)frame+4)<<10);
  if(!pause){
   int16_t hold=half_bits((uint16_t)(a[12]|(uint16_t)a[13]<<8));hold=half_bits((uint16_t)((uint32_t)(uint16_t)hold-(uint32_t)delta));
   a[12]=(uint8_t)hold;a[13]=(uint8_t)((uint16_t)hold>>8);
   if(hold<=0){int8_t interval;memcpy(&interval,a+9,1);a[12]=(uint8_t)interval;a[13]=(uint8_t)((uint16_t)(int16_t)interval>>8);a[8]++;memcpy(&frame,a+8,1);if(frame>3)a[0]=(uint8_t)((a[0]&~1u)|2u);}
  }
 }
 write32(a+36,signed_bits(bitmap));if(!factor)factor=0xb1400u;
 *out=(TaxiBallRenderRequest){x,y,z,asr(768u*factor,8),bitmap};return 1;
}
