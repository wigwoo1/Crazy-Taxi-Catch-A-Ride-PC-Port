#include "palette_registration_small.h"
#include <string.h>
static void half(uint8_t *p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void word(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
int taxi_palette_register_small(TaxiPaletteRegistration *s,const uint16_t palette[16],uint32_t identity,uint32_t cap,uint32_t mode,TaxiPaletteTransfer *transfer){
 if(!s||!palette||!transfer)return -2;
 unsigned selected=mode&255u;uint16_t *bytes=selected?&s->bg_bytes:&s->obj_bytes;
 if((unsigned)*bytes+32>512)return -1;
 if(s->count>=128)return -2;
 unsigned minimum=94,maximum=0,darkest=1,brightest=1;
 for(unsigned i=1;i<16;i++){
  unsigned c=palette[i],sum=(c&31)+((c>>5)&31)+((c>>10)&31);
  if(sum<minimum){minimum=sum;darkest=i;}
  if(i==1||sum>maximum){maximum=sum;brightest=i;}
 }
 unsigned index=s->count;uint8_t *record=s->records[index];
 unsigned limit=cap&255u;if(limit>32)limit=32;
 uint32_t source=(s->flags&1)?0x084a50b0u:(s->flags&2)?0x084a52b0u:(s->flags&4)?identity:0x084a50b0u;
 *transfer=(TaxiPaletteTransfer){source,(selected?0x05000000u:0x05000200u)+*bytes,0x84000008u};
 half(record,16);half(record+2,*bytes);half(record+4,(uint16_t)limit);record[6]=1;record[7]=(uint8_t)selected;
 record[14]=(uint8_t)darkest;record[15]=(uint8_t)brightest;word(record+16,identity);
 s->count++;*bytes=(uint16_t)(*bytes+32);return (int)index;
}
int taxi_palette_transfer_small_apply(TaxiPaletteBanks *banks,const TaxiPaletteTransfer *request,uint32_t identity,const uint16_t palette[16]){
 if(!banks||!request||request->control!=0x84000008u)return -1;
 uint32_t a=request->destination_identity;uint16_t *out;
 if(a>=0x05000000u&&a<=0x050001e0u)out=banks->background+(a-0x05000000u)/2;
 else if(a>=0x05000200u&&a<=0x050003e0u)out=banks->objects+(a-0x05000200u)/2;
 else return -1;
 if(a&1)return -1;
 if(request->source_identity==0x084a50b0u||request->source_identity==0x084a52b0u){
  uint16_t color=request->source_identity==0x084a50b0u?0:0x07ff;
  for(unsigned i=0;i<16;i++)out[i]=color;
 }else{if(!palette||request->source_identity!=identity)return -1;memmove(out,palette,32);}
 return 0;
}
