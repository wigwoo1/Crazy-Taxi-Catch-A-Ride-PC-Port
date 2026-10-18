#include "loading_pixels.h"
#include <string.h>
int taxi_loading_pixels(const uint8_t frame[38400],const TaxiPaletteBanks *banks,const TaxiStartupObjTransport *transport,const uint8_t *objects,size_t object_size,uint32_t object_identity,uint16_t mosaic,uint16_t out[38400]){
 if(mosaic)return -2;
 if(!frame||!banks||!transport||!objects||!out)return -1;
 for(unsigned i=0;i<38400;i++)out[i]=banks->background[frame[i]];
 uint8_t occupied[38400];memset(occupied,0,sizeof occupied);
 static const unsigned dims[3][4][2]={{{8,8},{16,16},{32,32},{64,64}},{{16,8},{32,8},{32,16},{64,32}},{{8,16},{8,32},{16,32},{32,64}}};
 for(unsigned i=0;i<128;i++){
  const uint16_t *a=transport->output[i];if(!(a[0]&256)&&(a[0]&512))continue;
  unsigned shape=a[0]>>14;if(shape>=3)return -2;unsigned w=dims[shape][a[1]>>14][0],h=dims[shape][a[1]>>14][1];
  int left=a[1]&511,top=a[0]&255;if(left>=256)left-=512;if(top>=160)top-=256;
  if(left>=240||left+(int)w<=0||top>=160||top+(int)h<=0)continue;
  if((a[0]&0x2900)||((a[2]>>10)&3))return -2;
  for(unsigned y=0;y<h;y++)for(unsigned x=0;x<w;x++){
   int dx=left+(int)x,dy=top+(int)y;if(dx<0||dx>=240||dy<0||dy>=160)continue;
   unsigned sx=(a[1]&4096)?w-1-x:x,sy=(a[1]&8192)?h-1-y:y;
   uint32_t identity=0x06010000u+((a[2]&1023)+(sy/8)*(w/8)+sx/8)*32+(sy%8)*4+(sx%8)/2;
   if(identity<object_identity||(size_t)(identity-object_identity)>=object_size)return -3;
   unsigned index=(objects[identity-object_identity]>>(4*(sx&1)))&15;if(!index)continue;
   unsigned pixel=(unsigned)dy*240+(unsigned)dx;if(occupied[pixel])continue;
   occupied[pixel]=1;out[pixel]=banks->objects[(a[2]>>12)*16+index];
  }
 }
 return 0;
}
