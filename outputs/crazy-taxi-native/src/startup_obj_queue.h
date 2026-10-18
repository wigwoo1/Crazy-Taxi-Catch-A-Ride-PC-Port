#ifndef TAXI_STARTUP_OBJ_QUEUE_H
#define TAXI_STARTUP_OBJ_QUEUE_H
#include "startup_obj.h"
typedef struct {
 uint8_t total,count[3],last_width,last_height;
 uint16_t attributes[3][128][4],depth[128][4];
} TaxiStartupObjQueue;
typedef struct {TaxiObjField field;uint8_t bucket,index;} TaxiStartupObjQueued;
/* Whole28288 for original startup8bpp resource shapes, originmode0.
   Native bounds reject bucket>2/count>=128; valid source-owned requests retain
   byte counters, clip behavior and attribute ordering. Returns1 queued,0
   disabled/clipped,-1 invalid native ownership. No final rendering here. */
int taxi_startup_obj_queue(TaxiStartupObjQueue *,const uint16_t metrics[9],int32_t x,int32_t y,int32_t z,uint16_t width,uint16_t height,uint8_t bucket,uint8_t priority,uint8_t flags,uint16_t obj_mask,uint32_t tile_base,TaxiStartupObjQueued *);
#endif
