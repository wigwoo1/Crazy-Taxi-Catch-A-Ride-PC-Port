#ifndef TAXI_PAIR_COLUMN_SETUP_H
#define TAXI_PAIR_COLUMN_SETUP_H
#include <stdint.h>
typedef struct { uint32_t row,rows,x,y,dx,dy; } TaxiPairColumnSetup;
/* Original1304..13C4: perspective interpolation then vertical clipping.
   Explicit bounds are caller-owned patched values; no framebuffer mutation. */
int taxi_pair_column_setup(const uint32_t descriptor[8],uint32_t top,uint32_t bottom,TaxiPairColumnSetup *out);
#endif
