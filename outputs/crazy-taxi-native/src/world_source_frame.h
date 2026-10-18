#ifndef TAXI_WORLD_SOURCE_FRAME_H
#define TAXI_WORLD_SOURCE_FRAME_H
#include "world_frame.h"
#include "source_frame_native.h"
/* Observed normal textured world submissions, explicit mip0 contract.
   Unsupported special, flat or near-clipped families fail. */
int taxi_world_source_frame(uint8_t output[38400],const TaxiSourceFrameConfig *,const TaxiWorldFrame *,TaxiSourceFrameResult *);
#endif
