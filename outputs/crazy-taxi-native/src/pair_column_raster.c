#include "pair_column_raster.h"
static uint32_t asr(uint32_t value,unsigned shift){
 if(!shift)return value;
 return (value>>shift)|((value&0x80000000u)?(~0u<<(32-shift)):0u);
}
int taxi_pair_column_raster_work(uint8_t fb[38400],size_t dst,const uint8_t *tex,size_t size,
 uint32_t mx,uint32_t my,unsigned sx,unsigned sy,uint32_t *x,uint32_t *y,
 uint32_t dx,uint32_t dy,unsigned rows,TaxiPairColumnWork *work){
 if(!fb||!tex||!x||!y||!rows||rows>160||sx>31||sy>31||dst>=38400||(dst&1u)||dst%240>=239||
   dst+240u*(rows-1)+1>=38400)return -1;
 uint32_t ax=*x,ay=*y;
 /* Validate every source access first; invalid native inputs retain output. */
 for(unsigned i=0;i<rows;i++){uint32_t index=(mx&asr(ax,sx))+(my&asr(ay,sy));if(index>=size)return -1;ax+=dx;ay+=dy;}
 ax=*x;ay=*y;TaxiPairColumnWork measured={rows,0};
 for(unsigned i=0;i<rows;i++){uint32_t index=(mx&asr(ax,sx))+(my&asr(ay,sy));uint8_t pixel=tex[index];
  if(pixel){fb[dst]=pixel;fb[dst+1]=pixel;measured.framebuffer_halfword_writes++;}dst+=240;ax+=dx;ay+=dy;
 }
 *x=ax;*y=ay;if(work)*work=measured;return 0;
}

int taxi_pair_column_raster(uint8_t fb[38400],size_t dst,const uint8_t *tex,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,uint32_t *x,uint32_t *y,uint32_t dx,uint32_t dy,unsigned rows){return taxi_pair_column_raster_work(fb,dst,tex,size,mx,my,sx,sy,x,y,dx,dy,rows,0);}
