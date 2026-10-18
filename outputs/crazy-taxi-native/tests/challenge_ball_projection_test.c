#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "challenge_ball_projection.h"
int main(void){
 TaxiTrig trig={0};trig.cosine[0]=256;TaxiBallRenderRequest r={0,0,4096,2178048,0x0849b0c9};TaxiBallProjection p;
 assert(taxi_challenge_ball_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p)==1&&p.packet_flags==8);
 assert(p.bounds.left==96&&p.bounds.right==144&&p.bounds.top==31&&p.bounds.bottom==80);
 TaxiBallProjection before=p;r.z=1024;assert(!taxi_challenge_ball_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p)&&!memcmp(&p,&before,sizeof p));r.z=38400;assert(!taxi_challenge_ball_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p));
 r.z=4096;r.x=2400;assert(taxi_challenge_ball_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p)==1&&p.packet_flags==12);
 r.x=4000;assert(!taxi_challenge_ball_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p));r.x=0;
 FILE *f=fopen("outputs/crazy-taxi-native/assets/challenge-ball/balloon.indices","rb");assert(f);uint8_t atlas[8192];assert(fread(atlas,1,8192,f)==8192&&fgetc(f)==EOF);fclose(f);
 for(unsigned frame=0;frame<8;frame++){
  r.bitmap_identity=0x0849b0c9u+1024*frame;assert(taxi_challenge_ball_project(&r,(TaxiPoint){0},0,0,160,106,&trig,&p)==1);
  uint8_t pixels[38400];memset(pixels,255,sizeof pixels);assert(taxi_challenge_ball_indexed(pixels,&p,atlas)==1);
  unsigned visible=0,untouched=0;for(unsigned i=0;i<38400;i++){visible+=pixels[i]!=255;untouched+=pixels[i]==255;assert(pixels[i]!=0);}
  assert(visible&&untouched);printf("Frame%u original indexed visible%u/zero-index background preserved\n",frame,visible);
 }
 puts("PASS Ball original32-square projection/depth/side gates and eight source bitmap frames through native keyed raster; controlled trig, no GPU/original execution oracle.");return 0;
}
