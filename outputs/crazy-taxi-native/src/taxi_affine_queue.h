#ifndef TAXI_AFFINE_QUEUE_H
#define TAXI_AFFINE_QUEUE_H
#include "startup_obj_transport.h"
/* Original28658 square16/square64 and tall16x32 4bpp. Source-qualified with normal
   resource groups8/9/10 effect frames and group1 taxi frames0/1. Resolved signed Q8
   trig values belong to angle low12. Other group shapes are unsupported. */
int taxi_affine_queue(TaxiStartupObjQueue *,TaxiStartupObjTransport *,const uint16_t m[9],int32_t x,int32_t y,int32_t z,uint16_t width,uint16_t height,uint8_t bucket,uint8_t priority,uint8_t flags,uint16_t mask,uint32_t tile,int32_t palette_base,uint8_t palette,int16_t scale,int16_t angle,int32_t sine,int32_t cosine,TaxiStartupObjQueued *);
#endif
