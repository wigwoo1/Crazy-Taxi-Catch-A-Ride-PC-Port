#ifndef TAXI_GRAPHICS_GENERATION_H
#define TAXI_GRAPHICS_GENERATION_H
#include <stdint.h>
/* Original2D20..2D38: no scene/transform leaves generation unchanged.
   Nonzero source identities are data, never called or dereferenced here. */
int taxi_graphics_generation_advance(uint32_t *,uint32_t,uint32_t);
#endif
