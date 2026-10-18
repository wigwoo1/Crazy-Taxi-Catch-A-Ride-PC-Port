#include "startup_obj_queue.h"
int taxi_startup_obj_queue(TaxiStartupObjQueue *q,const uint16_t m[9],int32_t x,int32_t y,int32_t z,uint16_t width,uint16_t height,uint8_t bucket,uint8_t priority,uint8_t flags,uint16_t mask,uint32_t tile_base,TaxiStartupObjQueued *out){
 if(!q||!m||!out)return -1;
 if((int8_t)q->total<0)return 0;
 TaxiStartupObjPlacement p;if(taxi_startup_obj_placement(m,x,y,256,&p))return -1;
 q->last_width=(uint8_t)p.field.width;q->last_height=(uint8_t)p.field.height;
 if(p.field.left>=(int32_t)width||p.field.left<=-(int32_t)p.field.width||p.field.top>=(int32_t)height||p.field.top<=-(int32_t)p.field.height)return 0;
 if(bucket>2||q->count[bucket]>=128)return -1;
 uint16_t shape=0,size=0;
 if(m[0]==m[1]){if(m[0]==2)size=0x4000;else if(m[0]==4)size=0x8000;else if(m[0]==8)size=0xc000;else if(m[0]!=1)return -1;}
 else if(m[0]==2&&m[1]==4){shape=0x8000;size=0x8000;}else if(m[0]==4&&m[1]==2){shape=0x4000;size=0x8000;}else if(m[0]==8&&m[1]==4){shape=0x4000;size=0xc000;}else return -1;
 uint8_t index=q->count[bucket];uint16_t *a=q->attributes[bucket][index];
 a[0]=(uint16_t)(shape|((uint32_t)p.field.top&255u)|0x2000u|((mask&0x400)?0x1000u:0u)|((flags&4)?0x400u:0u));
 a[1]=(uint16_t)(size|((uint32_t)p.field.left&511u)|((flags&1)?0x1000u:0u)|((flags&2)?0x2000u:0u));
 a[2]=(uint16_t)(((uint32_t)priority<<10)|(tile_base+((uint32_t)m[8]<<1)));a[3]=0;
 if(bucket==2){q->depth[index][0]=(uint16_t)p.field.left;q->depth[index][1]=(uint16_t)p.field.top;q->depth[index][2]=(uint16_t)z;}
 q->total=(uint8_t)(q->total+1u);q->count[bucket]=(uint8_t)(q->count[bucket]+1u);
 *out=(TaxiStartupObjQueued){p.field,bucket,index};return 1;
}
