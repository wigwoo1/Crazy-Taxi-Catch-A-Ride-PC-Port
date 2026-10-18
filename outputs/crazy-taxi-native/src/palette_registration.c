#include "palette_registration.h"
#include "shade_table.h"
static void half(uint8_t *p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void word(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
int taxi_palette_register(TaxiPaletteRegistration *s,const uint16_t palette[256],uint32_t identity,uint32_t limit,uint32_t mode,TaxiPaletteTransfer *transfer){
 if(!s||!palette||!transfer)return -2;
 unsigned selected=mode&255u;uint16_t *bytes=selected?&s->bg_bytes:&s->obj_bytes;
 if(*bytes)return -1;
 if(s->count>=128)return -2;
 uint8_t darkest,brightest;if(!taxi_palette_extrema(palette,&darkest,&brightest))return -2;
 unsigned index=s->count;uint8_t *record=s->records[index];
 unsigned cap=limit&255u;if(cap>32)cap=32;
 uint32_t source=(s->flags&1)?0x084a50b0u:(s->flags&2)?0x084a52b0u:(s->flags&4)?identity:0x084a50b0u;
 *transfer=(TaxiPaletteTransfer){source,selected?0x05000000u:0x05000200u,0x84000080u};
 half(record,256);half(record+2,*bytes);half(record+4,(uint16_t)cap);
 record[6]=1;record[7]=(uint8_t)selected;record[14]=darkest;record[15]=brightest;word(record+16,identity);
 s->count++;*bytes=(uint16_t)(*bytes+512);return (int)index;
}

int taxi_palette_registration_reset(TaxiPaletteRegistration *s){
 if(!s||s->count>128)return -2;
 s->bg_bytes=s->obj_bytes=0;
 for(unsigned i=0;i<s->count;i++){
  uint8_t *record=s->records[i];
  for(unsigned j=0;j<7;j++)record[j]=0;
  for(unsigned j=8;j<14;j++)record[j]=0;
  for(unsigned j=16;j<20;j++)record[j]=0;
 }
 s->count=0;return 0;
}
