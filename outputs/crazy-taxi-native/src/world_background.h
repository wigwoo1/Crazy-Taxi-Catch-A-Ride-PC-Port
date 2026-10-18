#ifndef TAXI_WORLD_BACKGROUND_H
#define TAXI_WORLD_BACKGROUND_H
#include <stdint.h>
#include <stddef.h>
#include "indexed_asset.h"
/* Original 2b550 copy, fixed 240-byte framebuffer stride. Bounds in pixels;
   source word copies require horizontal bounds divisible by four. */
int taxi_world_background(uint8_t frame[38400],const uint8_t *texture,
    size_t size,unsigned height,int32_t pitch,int32_t yaw,
    unsigned left,unsigned top,unsigned right,unsigned bottom,uint32_t fill);
/* Scene+20 selects descriptor; 2b4b6 repeats first pixel of its last row. */
int taxi_world_background_scene(uint8_t frame[38400],const TaxiIndexedAsset *asset,
    int32_t descriptor,int32_t pitch,int32_t yaw,unsigned left,unsigned top,
    unsigned right,unsigned bottom);
/* Consumer2b6b2..6ee: shortest camera pitch, signed /2, clamp +/-142. */
int32_t taxi_world_background_pitch(int32_t camera_pitch);
/* GBA bitmap mode4: palette index0 is transparent BG2 -> backdrop5.
   Preserve original indices; RGB color/fade cannot determine coverage.
   Mode3 direct-color BG2 has no transparent color. Unsupported modes fail.
   Output may alias indices; validation precedes writes. No windows/affine
   screen clipping here: caller supplies resolved BG2 enable/covered pixels. */
int taxi_world_bitmap_layers(uint8_t *layers,const uint8_t *indices,size_t count,
    unsigned mode,int bg2_enabled);
/* Original2C9F0 rectangle clear, caller-provided global03006900 fill word.
   Width/height are extents, not end coordinates. Fixed240-byte stride.
   Native safe domain: nonnegative extents<=240/160; offscreen is a no-op.
   Original33F2C repeats only the low byte of the supplied fill word. */
int taxi_world_clear_rect(uint8_t frame[38400],int32_t x,int32_t y,
    int32_t width,int32_t height,uint32_t pattern);
/* Original2B4B6..2B4E0 selected background texture last-row first index.
   Returns its four-byte repetition; indices and palette colors stay separate. */
int taxi_world_background_pattern(const TaxiIndexedAsset *,int32_t descriptor,uint32_t *pattern);
#endif
