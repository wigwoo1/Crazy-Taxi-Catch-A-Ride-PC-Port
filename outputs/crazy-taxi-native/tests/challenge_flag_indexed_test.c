#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "challenge_flag_indexed.h"
static void verify_rows(const TaxiChallengeFlagProjection *p,const uint8_t pixels[2048],const uint8_t oracle[38400]){
 TaxiIndexedSpan rows[160],wide[160];uint8_t rendered[38400];memset(rendered,253,sizeof rendered);
 assert(taxi_challenge_flag_rows(rows,p,0,240)==1);
 assert(taxi_challenge_flag_rows(wide,p,-24,264)==1);
 for(unsigned y=0;y<160;y++){
  const TaxiIndexedSpan *s=rows+y;uint32_t u=s->u,v=s->v;
  assert(s->x>=0&&(int64_t)s->x+(int64_t)s->pairs*2<=240);
  assert(wide[y].x>=-24&&(int64_t)wide[y].x+(int64_t)wide[y].pairs*2<=264);
  for(size_t i=0;i<s->pairs;i++){
   /* Independent consumer of doubled source-pixel sampling data. */
   unsigned texel=((u>>14)&31)+(((v>>14)&63)*32);
   if(pixels[texel])rendered[y*240+s->x+i*2]=rendered[y*240+s->x+i*2+1]=pixels[texel];
   u+=s->du;v+=s->dv;
  }
 }
 assert(!memcmp(rendered,oracle,sizeof rendered));
 TaxiIndexedSpan retained[160];memcpy(retained,rows,sizeof rows);
 assert(!taxi_challenge_flag_rows(rows,p,-23,264)&&!memcmp(rows,retained,sizeof rows));
}
int main(void){
 uint8_t pixels[2048],raw[16388],fb[38400],before[38400];
 FILE *f=fopen("outputs/crazy-taxi-native/assets/challenge-flag/flag.indices","rb");assert(f&&fread(pixels,1,sizeof pixels,f)==sizeof pixels);fclose(f);
 f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");assert(f&&fread(raw,1,sizeof raw,f)==sizeof raw&&!memcmp(raw,"CTR1",4));fclose(f);
 TaxiTrig trig={0};for(unsigned i=0;i<4096;i++)for(unsigned t=0;t<2;t++){unsigned u=raw[4+t*8192+i*2]|((unsigned)raw[5+t*8192+i*2]<<8);int n=u>=32768?(int)u-65536:(int)u;int v=n>=0?n/16:-((-n+15)/16);if(t)trig.cosine[i]=v;else trig.sine[i]=v;}
 uint8_t actor[44]={1};actor[16]=0xda;actor[17]=0x56;actor[18]=1;actor[24]=0xfd;actor[25]=actor[26]=actor[27]=255;
 unsigned changed=0;
 for(int scale=16;scale<=256;scale++){
  TaxiChallengeFlagRenderRequest r;TaxiChallengeFlagProjection p;TaxiIndexedVertex v[4];
  assert(taxi_challenge_flag_render_request(actor,87770,-3,735,(int16_t)scale,0,&r)==1);
  assert(taxi_challenge_flag_project(&r,(TaxiPoint){87770,735,-4003},0,0,160,106,&trig,&p)==1);
  assert(!taxi_challenge_flag_vertices(&p,v));
  assert(v[0].u==0&&v[0].v==(64u<<14)&&v[1].v==0&&v[2].u==(32u<<14)&&v[2].v==0&&v[3].v==(64u<<14));
  memset(fb,253,sizeof fb);assert(taxi_challenge_flag_indexed(fb,&p,pixels)==1);verify_rows(&p,pixels,fb);for(unsigned i=0;i<sizeof fb;i++)if(fb[i]!=253)changed++;
 }
 assert(changed);
 TaxiChallengeFlagRenderRequest clipped_request;TaxiChallengeFlagProjection clipped;
 assert(taxi_challenge_flag_render_request(actor,87770,-3,735,256,0,&clipped_request)==1);
 assert(taxi_challenge_flag_project(&clipped_request,(TaxiPoint){85320,735,-4003},0,0,160,106,&trig,&clipped)==1&&clipped.packet_flags==36);
 memset(fb,253,sizeof fb);assert(taxi_challenge_flag_indexed(fb,&clipped,pixels)==1);
 verify_rows(&clipped,pixels,fb);unsigned clipped_pixels=0;for(unsigned i=0;i<sizeof fb;i++)if(fb[i]!=253)clipped_pixels++;assert(clipped_pixels);
 TaxiChallengeFlagProjection p={.packet_flags=8};memcpy(before,fb,sizeof fb);assert(taxi_challenge_flag_indexed(fb,&p,pixels)==-1&&!memcmp(fb,before,sizeof fb));
 puts("PASS original Flag indices and UVs through native affine polygon: 241 rotated/shrinking states with byte-exact native-span consumer against polygon raster, widened bounds, partial-side clipped polygon and unsupported rectangle packet rejected; controlled camera, source-resolution diagnostic, no live palette/GPU claim.");return 0;
}
