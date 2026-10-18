#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "challenge_flag_projection.h"
int main(void){
 TaxiTrig trig={0};trig.cosine[0]=256;trig.sine[1024]=256;
 TaxiChallengeFlagRenderRequest r={.z=4000,.scale=65536,.rotation_angle=4096};TaxiChallengeFlagProjection p,before;
 assert(taxi_challenge_flag_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p)==1&&p.packet_flags==32);
 assert(p.camera_corners[0].x==-16&&p.camera_corners[1].y==64&&p.screen_corners[1].y==77);
 r.x=2500;assert(taxi_challenge_flag_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p)==1&&p.packet_flags==36);
 before=p;r.x=3000;assert(!taxi_challenge_flag_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p)&&!memcmp(&p,&before,sizeof p));assert(taxi_challenge_flag_project(&r,(TaxiPoint){0},0,0,256,106,&trig,&p)==1);
 r.x=0;r.rotation_angle=1024;assert(taxi_challenge_flag_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p)==1);assert(p.camera_corners[1].x==-64&&p.camera_corners[1].y==-16);
 r.z=1024;before=p;assert(!taxi_challenge_flag_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p)&&!memcmp(&p,&before,sizeof p));r.z=38400;assert(!taxi_challenge_flag_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p));
 FILE *f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");assert(f);uint8_t raw[16388];assert(fread(raw,1,sizeof raw,f)==sizeof raw&&!memcmp(raw,"CTR1",4));fclose(f);
 for(unsigned i=0;i<4096;i++){
  int values[2];for(unsigned t=0;t<2;t++){unsigned u=raw[4+t*8192+i*2]|((unsigned)raw[5+t*8192+i*2]<<8);int signed_value=u>=32768?(int)u-65536:(int)u;values[t]=signed_value>=0?signed_value/16:-((-signed_value+15)/16);}
  trig.sine[i]=values[0];trig.cosine[i]=values[1];
 }
 assert(trig.cosine[0]==256&&trig.sine[0]==0&&trig.sine[1024]==255);
 uint8_t actor[44]={1};actor[16]=0xda;actor[17]=0x56;actor[18]=1;actor[24]=0xfd;actor[25]=actor[26]=actor[27]=255;
 unsigned projected=0;unsigned original_bucket=0;
 for(int scale=16;scale<=256;scale++){
  assert(taxi_challenge_flag_render_request(actor,87770,-3,735,(int16_t)scale,0,&r)==1);
  assert(taxi_challenge_flag_project(&r,(TaxiPoint){87770,735,-4003},0,0,160,106,&trig,&p)==1);
  if(scale==16)original_bucket=p.bucket;else assert(p.bucket==original_bucket);
  assert(p.packet_flags==32);for(unsigned i=0;i<4;i++)assert(p.camera_corners[i].z==4000&&p.screen_corners[i].z==4000);
  assert(taxi_challenge_flag_project(&r,(TaxiPoint){87770,735,-4003},0,0,256,106,&trig,&p)==1);projected++;
 }
 printf("PASS original CTR trig and actual Flag X87770/Y735/Z-3: %u shrinking/rotating states, source-depth bucket invariant, both viewport widths; controlled camera pose.\n",projected);
 puts("PASS Flag native rotated projection: four corners, near/far, side clipping, retained culled output, wider viewport visibility; diagnostic cardinal trig only.");return 0;
}
