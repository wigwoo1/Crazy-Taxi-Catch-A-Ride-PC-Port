#include "text_layout.h"
#include <string.h>
int32_t taxi_text_correction_q8(const uint16_t *metrics,uint16_t count,
    uint16_t current,uint16_t next,const uint8_t *pairs,int32_t spacing,int16_t scale) {
    if(current>=count)return 0;
    const uint16_t *m=metrics+(unsigned)current*9;
    uint32_t correction;
    if(next&&pairs) {
        if(next>=count)return 0;
        correction=(uint32_t)pairs[(unsigned)current*count+next]-(uint32_t)spacing;
    } else {
        correction=(uint32_t)m[0]*8-((uint32_t)m[4]+m[6])-(uint32_t)spacing;
    }
    uint32_t wrapped=correction*(uint32_t)(int32_t)scale;
    int32_t result;memcpy(&result,&wrapped,sizeof result);return result;
}

uint32_t taxi_text_measure(const uint16_t *metrics,uint16_t count,const char *text,const uint8_t *pairs,int32_t spacing){
 uint32_t width=0,height=0;
 if(!metrics||!text)return 0;
 for(unsigned i=0;text[i];i++){
  uint16_t glyph=(uint16_t)((unsigned char)text[i]-32),next=text[i+1]?(uint16_t)((unsigned char)text[i+1]-32):0;
  if(glyph>=count)continue;const uint16_t *m=metrics+(unsigned)glyph*9;
  width+=(uint32_t)m[0]*8*256-(uint32_t)taxi_text_correction_q8(metrics,count,glyph,next,pairs,spacing,256);
  if((uint32_t)m[1]*8>height)height=(uint32_t)m[1]*8;
 }
 int32_t signed_width;memcpy(&signed_width,&width,4);return ((uint32_t)(signed_width/256)<<16)|height;
}

int32_t taxi_text_glyph_coordinate(int32_t q8,int16_t offset){return q8/256-offset;}
