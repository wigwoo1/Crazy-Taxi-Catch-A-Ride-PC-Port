#include "rank_bitmap.h"
int taxi_rank_bitmap_rect(TaxiRankBitmapRect *out,int32_t x){
 if(!out||x<0)return -1;
 TaxiRankBitmapRect r={0};
 x&=~1;
 if(x<240){
  int32_t width=240-x;if(width>36)width=36;
  r.x=x&~3;r.y=88;r.width=(uint16_t)(width&~3);r.height=r.width?32:0;
 }
 *out=r;return 0;
}
