#include "source_display_native.h"
#include <string.h>
int taxi_source_display_native(uint8_t out[153600],uint8_t workspace[153600],
 const uint8_t bitmap[38400],const TaxiSourceDisplayState *s,
 const uint16_t oam[128][4],const TaxiTitleAssets *assets,const TaxiPaletteBanks *palette,
 const TaxiFlagHudPlan *indicator,const uint8_t *indicator_indices,size_t indicator_size){
 if(!out||!workspace||out==workspace||!bitmap||!s||!oam||!assets||!palette)return -1;
 if((s->display&~16u)!=0x1444||s->bg2_control!=0x00c2||s->mosaic||s->blend_control!=0x1f4f||s->blend_alpha||s->blend_brightness)return -1;
 if(s->bg2.pa!=256||s->bg2.pb||s->bg2.pc||s->bg2.pd!=256||s->bg2.reference_x||s->bg2.reference_y)return -1;
 for(unsigned i=0;i<6;i++)if(s->windows[i])return -1;
 if(taxi_published_obj_layer(workspace,240,160,oam,assets,palette,0x06010000))return -1;
 for(unsigned i=0;i<38400;i++){
  uint16_t color=palette->background[bitmap[i]];
  for(unsigned c=0;c<3;c++)out[i*4+c]=(uint8_t)(((color>>(c*5))&31)*255/31);
  out[i*4+3]=255;

 }
 if(indicator&&taxi_challenge_flag_indicator_pixels(out,indicator,indicator_indices,indicator_size,palette->background))return -1;
 for(unsigned i=0;i<38400;i++)if(workspace[i*4+3])memcpy(out+i*4,workspace+i*4,4);
 return 0;
}
