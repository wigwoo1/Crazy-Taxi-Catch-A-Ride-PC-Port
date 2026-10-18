#include "title_obj_queue.h"
int taxi_title_obj_queue(TaxiStartupObjQueue *q,const uint16_t m[9],int32_t x,int32_t y,int32_t z,uint16_t width,uint16_t height,uint8_t bucket,uint8_t priority,uint8_t flags,uint16_t mask,uint32_t tile_base,int32_t palette_base,uint32_t palette,TaxiStartupObjQueued *out){
 int result=taxi_startup_obj_queue(q,m,x,y,z,width,height,bucket,priority,flags,mask,tile_base,out);
 if(result!=1)return result;
 uint16_t *a=q->attributes[out->bucket][out->index];a[0]&=0xdfffu;
 uint32_t bank=palette_base==-1?palette:(uint32_t)palette_base+palette;
 a[2]=(uint16_t)(((uint32_t)priority<<10)|(bank<<12)|(tile_base+m[8]));return 1;
}
