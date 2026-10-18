#ifndef TAXI_VIEW_TRANSFORM_H
#define TAXI_VIEW_TRANSFORM_H
#include <stdint.h>
#include <stddef.h>
#include "impact.h"
/* Original295a8/2965c full18-word transform. Tables are original runtime
   signed ROM halfwords ASR4, matching TaxiTrig (initializer0802db54). */
typedef struct {int32_t words[18];} TaxiViewTransform;
void taxi_view_transform_position(TaxiViewTransform *out,int32_t x,int32_t y,int32_t z);
void taxi_view_transform_angles(TaxiViewTransform *out,int32_t x,int32_t y,int32_t z,
    const int32_t sine[4096],const int32_t cosine[4096]);
/* df9c ->31860 reads first24 bytes of selected48-byte destination:
   position0..8, angles12..20; target at24..32 is a different field. */
int taxi_view_transform_destination(TaxiViewTransform *out,const uint8_t *record,
    size_t size,const TaxiTrig *trig);
#endif
