#ifndef TAXI_TAXI_FRAME_COPY_OWNED_H
#define TAXI_TAXI_FRAME_COPY_OWNED_H
#include "sprite_replacement_owned.h"
/* Original1591C..15950 two source-bank requests for group1 halves0/1.
   Caller owns original cache gate/previous-frame state and palette binding. */
int taxi_frame_copy_owned(TaxiSpriteReplacementOwner *,uint8_t driver,uint16_t frame,uint8_t immediate);
#endif
