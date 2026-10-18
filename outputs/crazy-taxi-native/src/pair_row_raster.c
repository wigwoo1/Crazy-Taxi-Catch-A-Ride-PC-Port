#include "pair_row_raster.h"
static uint32_t asr(uint32_t v,unsigned s){return !s?v:(v>>s)|((v&0x80000000u)?(~0u<<(32-s)):0u);}
int taxi_pair_row_raster(uint8_t fb[38400],size_t dst,const uint8_t *tex,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,uint32_t *x,uint32_t *y,uint32_t dx,uint32_t dy,unsigned pairs,TaxiPairColumnWork *work){
 if(!fb||!tex||!x||!y||!pairs||pairs>120||sx>31||sy>31||dst>=38400||(dst&1)||dst%240+pairs*2>240)return -1;
 uint32_t ax=*x,ay=*y;for(unsigned i=0;i<pairs;i++){uint32_t index=(mx&asr(ax,sx))+(my&asr(ay,sy));if(index>=size)return -1;ax+=dx;ay+=dy;}
 ax=*x;ay=*y;TaxiPairColumnWork measured={pairs,0};
 for(unsigned i=0;i<pairs;i++){uint8_t pixel=tex[(mx&asr(ax,sx))+(my&asr(ay,sy))];if(pixel){fb[dst]=fb[dst+1]=pixel;measured.framebuffer_halfword_writes++;}dst+=2;ax+=dx;ay+=dy;}
 *x=ax;*y=ay;if(work)*work=measured;return 0;
}

#include "projection_divide.h"
#include <string.h>
int taxi_pair_row_batch(uint8_t fb[38400],unsigned first,unsigned count,const uint32_t *d,const uint8_t *tex,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,TaxiPairColumnWork *work){
 if(!fb||!d||!tex||first>=160||!count||count>160-first||sx>31||sy>31)return -1;
 uint8_t staged[38400];memcpy(staged,fb,sizeof staged);TaxiPairColumnWork total={0};
 for(unsigned row=0;row<count;row++){const uint32_t *v=d+row*8;uint32_t left=v[0]&~1u,width_bits=v[4]-left;int32_t width;memcpy(&width,&width_bits,4);if(width<=0)continue;
  if(left>=240||(uint32_t)width>240-left)return -1;
  uint32_t x=v[1],y=v[2],dx=taxi_projection_divide(v[5]-x,(uint32_t)width)*2,dy=taxi_projection_divide(v[6]-y,(uint32_t)width)*2;unsigned pairs=((unsigned)width+1)/2;TaxiPairColumnWork part;
  if(taxi_pair_row_raster(staged,(first+row)*240+left,tex,size,mx,my,sx,sy,&x,&y,dx,dy,pairs,&part))return -1;
  total.texture_byte_reads+=part.texture_byte_reads;total.framebuffer_halfword_writes+=part.framebuffer_halfword_writes;
 }memcpy(fb,staged,sizeof staged);if(work)*work=total;return 0;
}

static int32_t signed_word(uint32_t x){int32_t v;memcpy(&v,&x,4);return v;}
int taxi_pair_row_prepare_bounds(TaxiPairRowPrepared *out,const uint32_t d[8],int32_t left,int32_t right){
 if(!out||!d||left>=right||left < -1024||right>1024||(left&1)||(right&1))return -1;
 int32_t a=signed_word(d[0]),b=signed_word(d[4]),width=signed_word(d[4]-d[0]);
 if(width<=0||a>=(int32_t)right||b<(int32_t)left)return 0;
 uint32_t x=d[1],y=d[2],dx=taxi_projection_divide(d[5]-x,(uint32_t)width),dy=taxi_projection_divide(d[6]-y,(uint32_t)width);
 if(a<(int32_t)left){uint32_t advance=(uint32_t)left-(uint32_t)a;x+=dx*advance;y+=dy*advance;a=(int32_t)left;}
 if(b>(int32_t)right)b=(int32_t)right;a=(int32_t)((uint32_t)a&~1u);unsigned pairs=((unsigned)(b-a)+1)/2;if(!pairs)pairs=1;
 if(a<left||(int64_t)a+pairs*2>right)return -1;
 *out=(TaxiPairRowPrepared){x,y,dx*2,dy*2,(unsigned)a,pairs};return 1;
}
int taxi_pair_row_prepare_clipped(TaxiPairRowPrepared *out,const uint32_t d[8],unsigned left,unsigned right){if(right>240)return -1;return taxi_pair_row_prepare_bounds(out,d,(int32_t)left,(int32_t)right);}
int taxi_pair_row_batch_clipped(uint8_t fb[38400],unsigned first,unsigned count,const uint32_t *d,unsigned left,unsigned right,const uint8_t *tex,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,TaxiPairColumnWork *work){
 if(!fb||!d||!tex||first>=160||!count||count>160-first||sx>31||sy>31||left>=right||right>240||(left&1)||(right&1))return -1;
 uint8_t staged[38400];memcpy(staged,fb,sizeof staged);TaxiPairColumnWork total={0};
 for(unsigned row=0;row<count;row++){TaxiPairRowPrepared p;int status=taxi_pair_row_prepare_clipped(&p,d+row*8,left,right);if(status<0)return -1;if(!status)continue;TaxiPairColumnWork part;
  if(taxi_pair_row_raster(staged,(first+row)*240+p.column,tex,size,mx,my,sx,sy,&p.x,&p.y,p.dx,p.dy,p.pairs,&part))return -1;
  total.texture_byte_reads+=part.texture_byte_reads;total.framebuffer_halfword_writes+=part.framebuffer_halfword_writes;
 }memcpy(fb,staged,sizeof staged);if(work)*work=total;return 0;
}

/* Original EFC..1018: clipped count precedes even-left address rounding. */
int taxi_pair_row_prepare_perspective(TaxiPairRowPrepared *out,const uint32_t d[8],int32_t left,int32_t right){
 if(!out||!d||left>=right||left < -1024||right>1024||(left&1)||(right&1))return -1;
 int32_t a=signed_word(d[0]),b=signed_word(d[4]),width=signed_word(d[4]-d[0]);
 if(width<=0||a>=right||b<left)return 0;
 uint32_t za=taxi_projection_divide(0x40000000,asr(d[3],6)),zb=taxi_projection_divide(0x40000000,asr(d[7],6));
 uint32_t ua=d[1]*za,ub=d[5]*zb,va=d[2]*za,vb=d[6]*zb;
 uint32_t x=asr(ua,8),y=asr(va,8),dx=asr(taxi_projection_divide(ub-ua,(uint32_t)width),8),dy=asr(taxi_projection_divide(vb-va,(uint32_t)width),8);
 if(a<left){uint32_t advance=(uint32_t)left-(uint32_t)a;x+=dx*advance;y+=dy*advance;a=left;}
 if(b>right)b=right;
 unsigned pairs=((unsigned)(b-a)+1)/2;if(!pairs)pairs=1;
 a=signed_word((uint32_t)a&~1u);
 if(a<left||(int64_t)a+pairs*2>right)return -1;
 *out=(TaxiPairRowPrepared){x,y,dx*2,dy*2,(unsigned)a,pairs};return 1;
}
int taxi_pair_row_batch_perspective(uint8_t fb[38400],unsigned first,unsigned count,const uint32_t *d,unsigned left,unsigned right,const uint8_t *tex,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,TaxiPairColumnWork *work){
 if(!fb||!d||!tex||first>=160||!count||count>160-first||sx>31||sy>31||left>=right||right>240||(left&1)||(right&1))return -1;
 uint8_t staged[38400];memcpy(staged,fb,sizeof staged);TaxiPairColumnWork total={0};
 for(unsigned row=0;row<count;row++){TaxiPairRowPrepared p;int status=taxi_pair_row_prepare_perspective(&p,d+row*8,(int32_t)left,(int32_t)right);if(status<0)return -1;if(!status)continue;TaxiPairColumnWork part;
  if(taxi_pair_row_raster(staged,(first+row)*240+p.column,tex,size,mx,my,sx,sy,&p.x,&p.y,p.dx,p.dy,p.pairs,&part))return -1;
  total.texture_byte_reads+=part.texture_byte_reads;total.framebuffer_halfword_writes+=part.framebuffer_halfword_writes;
 }memcpy(fb,staged,sizeof staged);if(work)*work=total;return 0;
}
