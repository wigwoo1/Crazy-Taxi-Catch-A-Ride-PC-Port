#ifndef TAXI_AXIS_SHADER_H
#define TAXI_AXIS_SHADER_H
#include "indexed_pixel.h"
#include "indexed_asset.h"
/* Native GPU adaptation: corrected source spans, continuous high resolution
   interpolation between adjacent spans. snapped is diagnostic source sampling.
   Float precision/wrap interpolation and triangle edge coverage require checks. */
int taxi_axis_shader_begin(const TaxiIndexedSpan *spans,unsigned count,int columns,
    int left,unsigned width,unsigned height,int snapped);
int taxi_axis_shader_lookup(const TaxiIndexedTexture *texture,const uint8_t lookup[2560],
    const uint16_t palette[256],int16_t cap);
/* Shaded lookup output alpha: resolved index0=backdrop, nonzero=BG2.
   Begin resets disabled (opaque raster output). Requires active lookup;
   caller must disable alpha-test/blending so written index0 is retained. */
int taxi_axis_shader_bitmap_coverage(int enabled);
int taxi_axis_shader_flat(uint8_t low,uint8_t high);
int taxi_axis_shader_special(void);
void taxi_axis_shader_end(void);
void taxi_axis_shader_free(void);
#endif
