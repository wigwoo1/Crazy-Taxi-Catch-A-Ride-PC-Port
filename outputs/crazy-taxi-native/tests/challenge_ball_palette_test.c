#include "../src/challenge_ball_palette.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void require(int ok,const char *m){if(!ok){fprintf(stderr,"FAIL %s\n",m);exit(1);}}
static void read_exact(const char *path,void *out,size_t n){FILE *f=fopen(path,"rb");require(f!=NULL,"asset open");require(fread(out,1,n,f)==n&&fgetc(f)==EOF,"exact asset size");require(!fclose(f),"asset close");}
int main(void){
 uint8_t indices[8192],raw[512],rgba[32768],before[32768];uint16_t palette[256];
 read_exact("outputs/crazy-taxi-native/assets/challenge-ball/balloon.indices",indices,sizeof indices);
 read_exact("outputs/crazy-taxi-native/assets/challenge-worlds/challenge2-palette.rgb555",raw,sizeof raw);
 for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(raw[2*i]|((uint16_t)raw[2*i+1]<<8));
 const int caps[]={-32768,-1,0,1,15,30,31,32,32767};
 for(unsigned c=0;c<sizeof caps/sizeof caps[0];c++){
  int cap=caps[c]<0?0:caps[c];require(!taxi_challenge_ball_palette_rgba(rgba,indices,palette,(int16_t)caps[c]),"atlas cap");
  for(unsigned i=0;i<8192;i++){
   for(unsigned channel=0;channel<3;channel++){
    unsigned value=(palette[indices[i]]>>(5*channel))&31;if(value>(unsigned)cap)value=(unsigned)cap;
    require(rgba[4*i+channel]==value*255/31,"registered RGB555 channel cap");
   }
   require(rgba[4*i+3]==(indices[i]?255:0),"original transparent index0");
  }
 }
 memset(rgba,0xA5,sizeof rgba);memcpy(before,rgba,sizeof rgba);
 require(taxi_challenge_ball_palette_rgba(rgba,NULL,palette,31)==-1&&!memcmp(rgba,before,sizeof rgba),"missing atlas atomic");
 require(taxi_challenge_ball_palette_rgba(rgba,indices,NULL,31)==-1&&!memcmp(rgba,before,sizeof rgba),"missing registered palette atomic");
 require(taxi_challenge_ball_palette_rgba(NULL,indices,palette,31)==-1,"missing output");
 puts("PASS original Ball registered scene-palette RGBA: 9 signed caps/73728 pixels, all8 frames and zero-index transparency; initial DMA/live registry/hardware/GBA execution not verified.");
 return 0;
}
