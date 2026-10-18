#include "pair_column_setup.h"
#include "projection_divide.h"
#include <string.h>
static uint32_t asr(uint32_t x,unsigned n){return (x>>n)|((x&0x80000000u)?(~0u<<(32-n)):0u);}
static int64_t signed_word(uint32_t x){return (x&0x80000000u)?(int64_t)x-4294967296LL:x;}
int taxi_pair_column_setup(const uint32_t d[8],uint32_t top,uint32_t bottom,TaxiPairColumnSetup *out){
 if(!d||!out||top>bottom||bottom>160)return -1;
 uint32_t start=d[0],end=d[4]+2u,span=end-start;
 if(signed_word(span)<=0||signed_word(start)>=bottom||signed_word(end)<top)return 0;
 uint32_t a=taxi_projection_divide(0x40000000u,asr(d[3],6));
 uint32_t b=taxi_projection_divide(0x40000000u,asr(d[7],6));
 uint32_t ax=d[1]*a,bx=d[5]*b,ay=d[2]*a,by=d[6]*b;
 uint32_t x=asr(ax,8),y=asr(ay,8);
 uint32_t dx=asr(taxi_projection_divide(bx-ax,span),8);
 uint32_t dy=asr(taxi_projection_divide(by-ay,span),8);
 uint32_t skip=top-start;
 if(signed_word(skip)>0){x+=dx*skip;y+=dy*skip;start=top;}
 if(signed_word(end)>bottom)end=bottom;
 TaxiPairColumnSetup next={start,end-start,x,y,dx,dy};
 memcpy(out,&next,sizeof(next));return 1;
}
