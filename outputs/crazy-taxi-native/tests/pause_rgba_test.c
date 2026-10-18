#ifdef NDEBUG
#undef NDEBUG
#endif
#include "palette_fade.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){unsigned tested=0;
 for(unsigned color=0;color<32768;color++)for(unsigned encoding=0;encoding<2;encoding++){
  uint8_t source[4],result[4];
  for(unsigned c=0;c<3;c++){unsigned v=(color>>(5*c))&31;source[c]=(uint8_t)(encoding?((v<<3)|(v>>2)):v*255/31);}
  source[3]=(uint8_t)(color%3==0?0:color%3==1?127:255);
  assert(!taxi_palette_gameplay_alternate_rgba(source,result,1));
  unsigned expected=(taxi_palette_gameplay_alternate_rgb555((uint16_t)color)&31)*255/31;
  assert(result[0]==expected&&result[1]==expected&&result[2]==expected&&result[3]==source[3]);
  assert(!taxi_palette_gameplay_alternate_rgba(source,source,1)&&!memcmp(source,result,4));tested++;
 }
 printf("PASS decoded RGB555 pause adapter: %u colors/encodings, alpha retention and in-place conversion.\n",tested);return 0;
}
