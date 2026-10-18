#ifndef TAXI_SHADE_TABLE_H
#define TAXI_SHADE_TABLE_H
#include <stdint.h>
#include <stddef.h>
/* Original 0802b0b0 palette builder, excluding engine state writes.
   Zero depth or absent palette preserves the previous lookup. */
int taxi_shade_table_build(uint8_t lookup[2560], const uint16_t palette[256],
    uint32_t depth, uint32_t packed_color);
/* Original 0802ca94 fills every bank from material byte15 (20-byte stride).
   Reject invalid signed-byte indices rather than reading outside the table. */
int taxi_shade_table_material(uint8_t lookup[2560], const uint8_t *materials,
    size_t count, uint32_t index);
/* Originale2b4..e33e registration: first minimum/maximum RGB555 sum
   among indices1..255; palette index0 is excluded. */
int taxi_palette_extrema(const uint16_t palette[256],uint8_t *darkest,uint8_t *brightest);
/* Original2C688: nearest two distinct indices1..255 by RGB Manhattan
   distance, first index wins ties. Result is first/second alternating bytes.
   Original store targets engine byte offset0x64 (100), not decimal64. */
int taxi_palette_nearest_pair(const uint16_t palette[256],unsigned red,unsigned green,unsigned blue,uint32_t *packed);
/* Original2C558 palette search: includes index0, strict first-index ties.
   Used by ordinary2AC6C initialization with RGB(0,0,0). This pure helper
   excludes mode4 gate, global fill owner and two-buffer writes. */
int taxi_palette_nearest_index(const uint16_t palette[256],unsigned red,
    unsigned green,unsigned blue,uint8_t *index);
#endif
