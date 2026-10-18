#include "destination_preview_border.h"
#include <string.h>
int taxi_destination_preview_border(uint8_t *pixels,size_t bytes,unsigned width,unsigned height,uint8_t color){
 if(!pixels||bytes<4096||width<2||width>64||!height||height>64)return -1;
 memset(pixels,color,width);memset(pixels+width*(height-1),color,width);
 unsigned stride=width&~1u;
 for(unsigned row=1;row+1<height;row++){unsigned start=row*stride;pixels[start]=color;pixels[start+stride-1]=color;}
 return 0;
}
