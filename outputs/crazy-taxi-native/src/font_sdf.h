#ifndef TAXI_FONT_SDF_H
#define TAXI_FONT_SDF_H
#include <stddef.h>
#include <stdint.h>

/* Build an antialiased RGBA glyph from a ROM-derived RGBA cell.  The source
   remains authoritative; output dimensions are scale times the source cell. */
int taxi_font_sdf_expand_rgba(const uint8_t *source,unsigned width,unsigned height,
    unsigned scale,unsigned edge_width_q8,uint8_t *output,size_t output_bytes,
    unsigned *output_width,unsigned *output_height);

#endif
