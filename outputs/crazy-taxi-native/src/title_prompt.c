#include "title_prompt.h"
int taxi_title_prompt(int32_t timer,uint32_t clock,uint8_t fade,
 uint16_t width,uint16_t height,TaxiTitlePrompt out[4]){
 if(!out)return -1;
 if(timer<=33||(clock&127)>111||fade)return 0;
 for(unsigned i=0;i<4;i++)out[i]=(TaxiTitlePrompt){
  .x=(int32_t)(width>>1)-64+(int32_t)i*32,.y=(int32_t)height-26,
  .z=0,.frame=(uint8_t)i,.group=10,.priority=0,.bucket=0,.flags=0,.palette=0};
 return 4;
}
