#include "pair_column_draw.h"
#include "pair_column_setup.h"
#include "pair_column_raster.h"
static int draw_work(uint8_t fb[38400],unsigned column,const uint32_t d[8],unsigned top,unsigned bottom,const uint8_t *tex,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,TaxiPairColumnWork *work){
 if(!fb||!tex||column>238||(column&1u)||sx>31||sy>31)return -1;
 TaxiPairColumnSetup setup;int status=taxi_pair_column_setup(d,top,bottom,&setup);
 if(status<=0)return status;
 /* Original loop draws once even when clipped remaining count is zero. */
 unsigned rows=setup.rows?setup.rows:1;
 if(taxi_pair_column_raster_work(fb,column+240u*setup.row,tex,size,mx,my,sx,sy,&setup.x,&setup.y,setup.dx,setup.dy,rows,work))return -1;
 return 1;
}

#include <string.h>
int taxi_pair_column_batch_work(uint8_t fb[38400],unsigned first,unsigned count,const uint32_t *d,unsigned top,unsigned bottom,const uint8_t *tex,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,TaxiPairColumnWork *work){
 if(!fb||!d||!tex||first>=120||!count||count>120-first||top>bottom||bottom>160||sx>31||sy>31)return -1;
 uint8_t staged[38400];memcpy(staged,fb,sizeof staged);
 TaxiPairColumnWork total={0};
 for(unsigned i=0;i<count;i++){TaxiPairColumnWork part={0};if(draw_work(staged,(first+i)*2,d+i*8,top,bottom,tex,size,mx,my,sx,sy,&part)<0)return -1;total.texture_byte_reads+=part.texture_byte_reads;total.framebuffer_halfword_writes+=part.framebuffer_halfword_writes;}

 memcpy(fb,staged,sizeof staged);if(work)*work=total;return 0;
}

int taxi_pair_column_draw(uint8_t fb[38400],unsigned column,const uint32_t d[8],unsigned top,unsigned bottom,const uint8_t *tex,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy){return draw_work(fb,column,d,top,bottom,tex,size,mx,my,sx,sy,0);}
int taxi_pair_column_batch(uint8_t fb[38400],unsigned first,unsigned count,const uint32_t *d,unsigned top,unsigned bottom,const uint8_t *tex,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy){return taxi_pair_column_batch_work(fb,first,count,d,top,bottom,tex,size,mx,my,sx,sy,0);}
