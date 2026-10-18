#ifndef TAXI_INDEXED_PIXEL_H
#define TAXI_INDEXED_PIXEL_H
#include <stdint.h>
#include <stddef.h>
#include "pair_column_raster.h"
#include "flat_row_raster.h"
typedef struct {
    int32_t x;
    uint32_t u, v, shade;
} TaxiIndexedEdge;
typedef struct {
    int32_t x;
    size_t pairs;
    uint32_t u, v, shade, du, dv, dshade;
} TaxiIndexedSpan;
typedef struct { uint32_t umask, vmask; unsigned ushift, vshift; } TaxiIndexedSampling;
typedef struct { int32_t x, y; uint32_t u, v, depth; } TaxiIndexedVertex;
/* Recovered 198c family at original 240x160 resolution only.
   This is not the other material raster families or the widescreen renderer. */
int taxi_indexed_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices, size_t count,
    const uint8_t *texture, size_t texture_size, TaxiIndexedSampling sampling,
    const uint8_t lookup[2560], int32_t left, int32_t top,
    int32_t right, int32_t bottom, uint32_t far_depth, uint32_t shade_depth);
/* Original1730 unshaded family: all vertices already within viewport.
   Its even alignment precedes gradient division, unlike198c. */
int taxi_indexed_unshaded_clipped_rows_bounds(TaxiIndexedSpan spans[160],const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right);
int taxi_indexed_unshaded_clipped_polygon_work(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling,TaxiPairColumnWork *work);
int taxi_indexed_unshaded_polygon_work(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling,TaxiPairColumnWork *work);
int taxi_indexed_unshaded_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling);
/* Original 198c..1ba8 edge workspace; caller retains rows not written. */
int taxi_indexed_edges(TaxiIndexedEdge (*rows)[2], size_t row_count,
    const TaxiIndexedVertex *vertices, size_t count, int32_t top, int32_t bottom,
    uint32_t far_depth, uint32_t shade_depth, int32_t *first, int32_t *last);
/* Original 1bc0..1bf8 for a validated power-of-two texture descriptor. */
int taxi_indexed_sampling(TaxiIndexedSampling *out, unsigned width,
    unsigned height, unsigned log_width, unsigned level);
/* Original 1c44..1d20. Clip limits are caller-provided patched engine values. */
int taxi_indexed_span_prepare(TaxiIndexedSpan *out, TaxiIndexedEdge left,
    TaxiIndexedEdge right, int32_t clip_left, int32_t clip_right);
/* Original ARM1d30..1d60 keyed doubled-pixel path only.
   Shade input is signed12-fractional-bit; lookup has10 banks of256 indices. */
int taxi_indexed_keyed_pair(uint8_t output[2],uint8_t texel,int32_t shade,
    const uint8_t lookup[2560]);
/* Original 1d24..1d84 inner loop after edge interpolation and clipping.
   Steps are already doubled by 1cf4..1d14; masks/shifts come from texture setup. */
int taxi_indexed_keyed_span(uint8_t *output, size_t pairs,
    const uint8_t *texture, size_t texture_size, const uint8_t lookup[2560],
    uint32_t u, uint32_t v, uint32_t shade, uint32_t du, uint32_t dv,
    uint32_t dshade, uint32_t umask, uint32_t vmask,
    unsigned ushift, unsigned vshift);
/* Original1dd4 clipped /1f94 in-bounds flat halfword pattern.
   The two color bytes alternate horizontally and swap on odd rows. */
int taxi_indexed_flat_rows_bounds(TaxiIndexedSpan spans[160],const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right);
int taxi_indexed_clipped_flat_polygon_work(uint8_t framebuffer[38400],const TaxiIndexedVertex *vertices,size_t count,uint16_t pattern,TaxiFlatRowWork *work);
int taxi_indexed_flat_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,uint16_t pattern,int clipped);
/* Original20fc aligns X before shade division, then clips/advances shade.
   Native widened bounds; committed source row data for GPU consumer. */
int taxi_indexed_shaded_flat_rows_bounds(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right,
    uint32_t far_depth,uint32_t shade_depth);
/* Original20fc shaded flat pattern; alignment precedes shade division. */
int taxi_indexed_shaded_flat_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,uint16_t pattern,
    const uint8_t lookup[2560],uint32_t far_depth,uint32_t shade_depth);
/* Original23d0/2668 selects one of8 material halfwords using signed
   vertex attribute ASR24; optional lookup enables2668 depth shading. */
int taxi_indexed_attribute_flat_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,const uint32_t *attributes,size_t count,
    const uint8_t patterns[16],const uint8_t *lookup,
    uint32_t far_depth,uint32_t shade_depth);
/* Original198c shaded affine row spans, source depth quotient/ASR12 bank.
   Signed wrapped shade and doubled dshade retained. GPU integration pending. */
int taxi_indexed_shaded_rows_bounds(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right,
    uint32_t far_depth,uint32_t shade_depth);
/* Original unshaded1448/1730 row sampling, with native widened even bounds.
   clipped!=0 advances UV before even-left alignment;1730 aligns before division.
   SourceY160 rows; invalid input retains caller workspace. */
int taxi_indexed_affine_rows_bounds(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right,int clipped);
/* Native widescreen adaptation: even horizontal bounds in [-1024,1024].
   Rows retain160-source-Y sampling; columns are indexed from left in pairs.
   Column capacity >= (right-left)/2; only that many entries are written.
   Same recovered endpoint math; extended bounds are host behavior. */
int taxi_indexed_perspective_rows_bounds(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right);
int taxi_indexed_perspective_columns_bounds(TaxiIndexedSpan *spans,size_t capacity,
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right);
/* Original c5c row endpoint reciprocal correction and signed wrapped gradients.
   Source 240x160 coordinates; pairs/steps retain even-address semantics.
   Empty rows have pairs0. Invalid inputs preserve caller workspace.
   Exposes sampling data for native GPU integration, not a 4K rasterizer. */
int taxi_indexed_perspective_rows_spans(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count);
/* Originalc5c/1044 perspective edges, recovered UV endpoints per row. */
int taxi_indexed_perspective_rows_polygon_work(uint8_t framebuffer[38400],const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,size_t texture_size,TaxiIndexedSampling sampling,TaxiPairColumnWork *work);
int taxi_indexed_perspective_rows_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling);
/* Original1048 column endpoint correction. Each entry covers source x=2*i,
   x+1; x field is first Y, pairs is number of rows, du/dv step one row.
   Empty columns have pairs0; invalid input retains workspace. */
int taxi_indexed_perspective_columns_spans(TaxiIndexedSpan spans[120],
    const TaxiIndexedVertex *vertices,size_t count);
/* Original1048/1440: reciprocal edges traversed in two-pixel columns. */
int taxi_indexed_perspective_columns_polygon_work(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling,TaxiPairColumnWork *work);
int taxi_indexed_perspective_columns_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling);
/* Original special rectangle UV clipping/even address semantics, widened X.
   bank is constant source shading bank; unshaded caller passes0. */
int taxi_indexed_billboard_rows_bounds(TaxiIndexedSpan spans[160],int32_t left,int32_t top,
    int32_t right,int32_t bottom,unsigned width,unsigned height,int32_t clip_left,int32_t clip_right,
    int clipped,unsigned bank);
/* Original4bac has no bank clamp; bounds rejection avoids missing source memory. */
int taxi_indexed_billboard_bank(unsigned *out,uint32_t depth,uint32_t far_depth,uint32_t shade_depth);
/* Original49f8/4a94 special rectangle, level0, original240x160.
   Clipped variant advances UV before rounding both X bounds down. */
int taxi_indexed_billboard_work(uint8_t framebuffer[38400],int32_t left,int32_t top,int32_t right,int32_t bottom,const uint8_t *texture,size_t texture_size,unsigned width,unsigned height,unsigned log_width,int clipped,TaxiPairColumnWork *work);
int taxi_indexed_billboard(uint8_t framebuffer[38400],int32_t left,
    int32_t top,int32_t right,int32_t bottom,const uint8_t *texture,
    size_t texture_size,unsigned width,unsigned height,unsigned log_width,int clipped);
/* Original4bac with bank2+depth quotient and rotated-word shade lookup. */
int taxi_indexed_shaded_billboard(uint8_t framebuffer[38400],int32_t left,
    int32_t top,int32_t right,int32_t bottom,const uint8_t *texture,
    size_t texture_size,unsigned width,unsigned height,unsigned log_width,
    const uint8_t lookup[2560],uint32_t depth,uint32_t far_depth,uint32_t shade_depth);
/* Source2b720..2b960, textured special packet flag8 only. Returns0 outside this owner. */
uint32_t taxi_indexed_textured_billboard_callback(uint16_t flags);
/* Source31b00/31c86/31ca2/31e6e: corner rejection is caller-owned. */
uint16_t taxi_indexed_billboard_packet_flags(int32_t angle,uint32_t caller_flags,int side_clipped);
#endif
