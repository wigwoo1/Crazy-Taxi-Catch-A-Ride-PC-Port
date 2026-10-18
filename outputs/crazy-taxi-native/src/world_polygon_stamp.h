#ifndef TAXI_WORLD_POLYGON_STAMP_H
#define TAXI_WORLD_POLYGON_STAMP_H
#include <stdint.h>
#include <stddef.h>
/* Original31EC normal-reference first-visit gate3228..3230. Caller2D18
   supplies graphics generation after increment. Cache survives preview restore.
   Special-reference dispatch and material rejection remain caller-owned. */
int taxi_world_polygon_stamp(uint32_t *,size_t,uint32_t,unsigned);
/* Original38A8..38CC special dispatch: bit14 references skip without touching
   cache; other bit15 references use masked14-bit index and same generation gate. */
int taxi_world_special_stamp(uint32_t *,size_t,uint32_t,unsigned);
#endif
