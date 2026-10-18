#include "start_art_replace.h"
int taxi_start_art_replace(int16_t phase,uint8_t language,const TaxiHudStartBackend *b){
 if(!b||!b->resource_word||!b->write||!b->replace||phase<0||phase>2||language>4)return -1;
 uint32_t index=(uint32_t)phase*5+language;
 for(unsigned j=0;j<3;j++){uint32_t source=b->resource_word(b->context,0x087ec210+4*index);if(b->replace(b->context,source,j,j,7,0))return -1;}
 uint32_t palette=b->resource_word(b->context,0x087ec24c+4*index);
 b->write(b->context,0x040000d4,palette,4);
 b->write(b->context,0x040000d8,0x05000360,4);
 b->write(b->context,0x040000dc,0x80000010,4);
 palette=b->resource_word(b->context,0x087ec24c+4*index);
 b->write(b->context,0x0200187c,palette,4);
 return 0;
}
