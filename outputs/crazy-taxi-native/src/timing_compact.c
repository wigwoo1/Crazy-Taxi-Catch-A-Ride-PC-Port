#include "timing_compact.h"
#include <string.h>
static const unsigned char widths[17]={4,4,8,8,4,4,4,4,4,1,8,4,4,4,4,4,1};
static uint64_t read_le(const unsigned char *p,unsigned n){
 uint64_t v=0;for(unsigned i=0;i<n;i++)v|=(uint64_t)p[i]<<(8*i);return v;
}
int taxi_timing_compact_encode(TaxiTimingCompact *state,const unsigned char raw[74],
 unsigned char *out,size_t capacity,size_t *used){
 if(!state||!raw||!out||!used)return -1;
 TaxiTimingCompact next=*state;unsigned char buffer[TAXI_TIMING_COMPACT_MAX]={0};
 size_t pos=3,offset=0;uint32_t mask=0;
 for(unsigned i=0;i<17;i++){
  uint64_t value=read_le(raw+offset,widths[i]),delta=value^state->prior[i];
  offset+=widths[i];next.prior[i]=value;
  if(delta){
   mask|=UINT32_C(1)<<i;
   do{unsigned char b=(unsigned char)(delta&127);delta>>=7;
    buffer[pos++]=(unsigned char)(b|(delta?128:0));
   }while(delta);
  }
 }
 if(pos>capacity)return -1;
 buffer[0]=(unsigned char)mask;buffer[1]=(unsigned char)(mask>>8);buffer[2]=(unsigned char)(mask>>16);
 memcpy(out,buffer,pos);*state=next;*used=pos;return 0;
}
int taxi_timing_compact_decode(TaxiTimingCompact *state,const unsigned char *data,size_t size,
 unsigned char raw[74],size_t *used){
 if(!state||!data||!raw||!used||size<3)return -1;
 uint32_t mask=(uint32_t)data[0]|((uint32_t)data[1]<<8)|((uint32_t)data[2]<<16);
 if(mask&~UINT32_C(0x1ffff))return -1;
 TaxiTimingCompact next=*state;unsigned char buffer[74];size_t pos=3,offset=0;
 for(unsigned i=0;i<17;i++){
  if(mask&(UINT32_C(1)<<i)){
   uint64_t delta=0;unsigned j;
   for(j=0;j<10;j++){
    if(pos==size)return -1;
    unsigned char b=data[pos++];
    if(j==9&&(b&254))return -1;
    delta|=(uint64_t)(b&127)<<(7*j);
    if(!(b&128)){
     if(!delta||(j&&!(b&127)))return -1;
     break;
    }
   }
   if(j==10)return -1;
   next.prior[i]^=delta;
  }
  unsigned n=widths[i];uint64_t value=next.prior[i];
  if(n<8&&(value>>(8*n)))return -1;
  for(unsigned j=0;j<n;j++)buffer[offset++]=(unsigned char)(value>>(8*j));
 }
 memcpy(raw,buffer,74);*state=next;*used=pos;return 0;
}
