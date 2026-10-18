#include "published_obj_layer.h"
#include "obj_tiles_rgba.h"
#include "obj_order.h"
#include <string.h>
int taxi_published_obj_layer(uint8_t *rgba,unsigned width,unsigned height,
 const uint16_t oam[128][4],const TaxiTitleAssets *assets,
 const TaxiPaletteBanks *palette,uint32_t destination){
 if(!rgba||!oam||!assets||!palette||!width||!height||width>4096||height>4096)return -1;
 memset(rgba,0,(size_t)width*height*4);
 for(int i=127;i>=0;i--){
  uint16_t a=oam[i][0],b=oam[i][1],c=oam[i][2];
  int affine=(a&0x100)!=0;
  if(!affine&&(a&0x200))continue;
  if((a&0xc000)||(a&0x2000)||(a&0x1c00)||(c&0xc00))return -1;
  unsigned side=8u<<(b>>14);
  unsigned slot=(b>>9)&31;int16_t matrix[4];
  for(unsigned k=0;k<4;k++)matrix[k]=(int16_t)oam[slot*4+k][3];
  int doubled=affine&&(a&0x200)!=0;unsigned field=side*(unsigned)(doubled+1);
  int left=b&511,top=a&255;if(left>=256)left-=512;if(top>=160)top-=256;
  uint8_t pixels[64*64*4];if(taxi_obj_tiles_rgba(pixels,sizeof pixels,assets,palette,destination,c,side,side))return -1;
  for(unsigned y=0;y<field;y++)for(unsigned x=0;x<field;x++){
   int dx=left+(int)x,dy=top+(int)y,sx,sy;
   if(dx<0||dy<0||(unsigned)dx>=width||(unsigned)dy>=height)continue;
   if(affine){int valid=taxi_obj_affine_point(matrix,side,side,doubled,(int)x,(int)y,&sx,&sy);if(valid<0)return -1;if(!valid)continue;}
   else{sx=(b&0x1000)?(int)side-1-(int)x:(int)x;sy=(b&0x2000)?(int)side-1-(int)y:(int)y;}
   const uint8_t *p=pixels+((size_t)sy*side+(unsigned)sx)*4;if(p[3])memcpy(rgba+((size_t)dy*width+(unsigned)dx)*4,p,4);
  }
 }
 return 0;
}
