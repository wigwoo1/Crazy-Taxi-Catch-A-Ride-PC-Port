#include "flat_row_raster.h"
#include <string.h>
static int32_t signed_word(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
int taxi_flat_row_batch(uint8_t fb[38400],unsigned first,unsigned count,const uint32_t *d,unsigned left,unsigned right,uint16_t pattern,TaxiFlatRowWork *work){
 if(!fb||!d||first>=160||!count||count>160-first||left>=right||right>240||(left&1)||(right&1))return -1;
 uint8_t staged[38400];memcpy(staged,fb,sizeof staged);TaxiFlatRowWork total={0};
 for(unsigned row=0;row<count;row++){
  int32_t a=signed_word(d[row*8]),b=signed_word(d[row*8+4]);
  if(a<(int32_t)left)a=(int32_t)left;if(b>(int32_t)right)b=(int32_t)right;
  a=signed_word((uint32_t)a&~1u);int32_t width=signed_word((uint32_t)b-(uint32_t)a);if(width<=0)continue;
  unsigned pairs=((unsigned)width+1)/2;if(a<0||(uint64_t)(unsigned)a+pairs*2>240)return -1;
  unsigned dst=(first+row)*240+(unsigned)a;uint8_t low=(uint8_t)pattern,high=(uint8_t)(pattern>>8);
  if((first+row)&1){uint8_t t=low;low=high;high=t;}
  if(dst&2){staged[dst]=low;staged[dst+1]=high;dst+=2;pairs--;total.framebuffer_halfword_writes++;}
  while(pairs>=2){staged[dst]=staged[dst+2]=low;staged[dst+1]=staged[dst+3]=high;dst+=4;pairs-=2;total.framebuffer_word_writes++;}
  if(pairs){staged[dst]=low;staged[dst+1]=high;total.framebuffer_halfword_writes++;}
 }
 memcpy(fb,staged,sizeof staged);if(work)*work=total;return 0;
}
