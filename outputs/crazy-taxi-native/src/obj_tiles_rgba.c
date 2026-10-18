#include "obj_tiles_rgba.h"
int taxi_obj_tiles_rgba(uint8_t *out,size_t capacity,const TaxiTitleAssets *a,const TaxiPaletteBanks *p,uint32_t destination,uint16_t attr2,unsigned width,unsigned height){
 if(!out||!a||!p||width<8||width>64||height<8||height>64||(width&(width-1))||(height&(height-1))||capacity<(size_t)width*height*4)return -1;
 uint32_t address=0x06010000u+(attr2&1023u)*32u;if(address<destination)return -1;size_t offset=address-destination,bytes=(size_t)width*height/2;if(offset>sizeof a->objects||bytes>sizeof a->objects-offset)return -1;
 unsigned bank=attr2>>12;
 for(unsigned y=0;y<height;y++)for(unsigned x=0;x<width;x++){
  size_t source=offset+((y/8)*(width/8)+x/8)*32+(y%8)*4+x%8/2;unsigned index=(a->objects[source]>>((x&1)*4))&15;uint16_t color=p->objects[bank*16+index];size_t pixel=((size_t)y*width+x)*4;for(unsigned c=0;c<3;c++)out[pixel+c]=(uint8_t)(((color>>(c*5))&31)*255/31);out[pixel+3]=index?255:0;
 }
 return 0;
}
