#include "palette_fade_setup.h"
static uint16_t normalize(uint32_t input){
 uint16_t mask=(uint16_t)input;
 if(mask&1)mask&=0xfffd;else if(mask&2)mask&=0xfffe;
 static const uint16_t bits[7]={4,8,16,32,64,128,256};
 static const uint16_t keep[7]={0xfc07,0xfc0b,0xfc13,0xfc23,0xfc63,0xfc83,0xfd03};
 for(unsigned i=0;i<7;i++)if(mask&bits[i])return (uint16_t)(mask&keep[i]);
 return mask;
}
static void channels(uint16_t mask,uint8_t out[3]){
 out[0]=out[1]=out[2]=1;
 if(mask&4)out[1]=out[2]=0;
 else if(mask&8)out[0]=out[2]=0;
 else if(mask&16)out[0]=out[1]=0;
 else if(mask&32)out[2]=0;
 else if(mask&64)out[1]=0;
 else if(mask&128)out[0]=0;
}
int taxi_palette_fade_setup(TaxiPaletteFadeSetup *out,uint32_t mode,uint32_t cap,uint32_t background,uint32_t objects){
 if(!out)return -1;
 out->mode=(uint8_t)mode;unsigned value=cap&255;out->cap=(uint8_t)(value>32?32:value);out->endpoint_count=0;
 out->background_mask=normalize(background);out->object_mask=normalize(objects);
 channels(out->background_mask,out->background_channels);channels(out->object_mask,out->object_channels);return 0;
}

int taxi_palette_fade_start_normal(TaxiPaletteFadeStart *out,uint32_t mode,uint32_t cap,uint32_t bg,uint32_t obj,uint16_t width,uint16_t height){
 if(!out||(mode&255)>1)return -1;
 TaxiPaletteFadeStart next=*out;
 if(taxi_palette_fade_setup(&next.masks,mode,cap,bg,obj))return -1;
 taxi_palette_fade_begin(&next.fade,next.masks.mode);
 if(!taxi_palette_motion_begin(&next.motion,next.masks.mode,width,height))return -1;
 if(next.masks.mode==1)next.scale_x_177c=next.scale_y_177e=256;
 if((next.masks.background_mask|next.masks.object_mask)&0x2000){next.bldcnt=0x00ff;next.bldy=next.masks.mode?16:0;}
 *out=next;return 0;
}
