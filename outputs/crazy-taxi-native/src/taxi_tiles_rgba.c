#include "taxi_tiles_rgba.h"
int taxi_tiles_rgba(uint8_t out[32768],const TaxiTitleAssets *a,const TaxiPaletteBanks *p,uint32_t destination,unsigned bank){
 if(!out||!a||!p||bank>15||a->bases[1]<destination)return -1;
 size_t offset=a->bases[1]-destination;if(offset>sizeof a->objects||4096>sizeof a->objects-offset)return -1;
 for(unsigned half=0;half<2;half++)for(unsigned y=0;y<64;y++)for(unsigned x=0;x<64;x++){
  size_t source=offset+half*2048+((y/8)*8+x/8)*32+(y%8)*4+x%8/2;
  unsigned index=(a->objects[source]>>((x&1)*4))&15;uint16_t color=p->objects[bank*16+index];size_t pixel=(y*128+half*64+x)*4;
  for(unsigned c=0;c<3;c++)out[pixel+c]=(uint8_t)(((color>>(c*5))&31)*255/31);
  out[pixel+3]=index?255:0;
 }
 return 0;
}
