#ifndef TAXI_WORLD_FRAME_H
#define TAXI_WORLD_FRAME_H
#include "world_material.h"
#include "render_queue.h"
#include "indexed_asset.h"
enum {TAXI_RENDER_WORLD=0,TAXI_RENDER_TRAFFIC=1,TAXI_RENDER_SPECIAL=2};
typedef struct {
    unsigned source_index,material,texture,bucket,kind;
    uint16_t raster_flags;
    uint8_t raster_flags_valid;
    /* Retain original submission when draw preparation finds no screen coverage. */
    uint8_t draw_culled;
    /* Native producer coordinates before consumer depth clipping.
       For depth-clipped submissions screen coordinates are not defined. */
    TaxiWorldClippedPolygon raw_polygon;
    TaxiWorldClippedPolygon polygon;
    TaxiWorldRenderSpecial special;
} TaxiWorldFramePacket;
typedef struct {
    const TaxiWorldAsset *world;
    TaxiPoint camera;int32_t yaw,pitch,phase;
    const TaxiTrig *trig;TaxiTrafficProjection projection;
    int32_t left,top,right,bottom;
    /* Optional original engine+50 owner for2D18 bounds; viewport resets retain it. */
    const int32_t (*bounds_corners)[4];
    unsigned texture_count;
    const TaxiIndexedAsset *textures;
    /* Optional explicit original engine/material state. NULL leaves flags
       unavailable; caller must not infer original cache or shade ownership. */
    const uint8_t *material_cache;size_t material_cache_size;
    int32_t shade_depth;
    TaxiRenderQueue *queue;
    TaxiWorldFramePacket *packets;size_t capacity,count;
    uint8_t *normal_seen,*special_seen;size_t normal_seen_size,special_seen_size;
    /* Optional persistent source cache owners. Both must be supplied together.
       Caller owns generation advancement and preview snapshot restoration. */
    uint32_t *normal_stamps,*special_stamps;
    size_t normal_stamp_count,special_stamp_count;
    uint32_t generation;
} TaxiWorldFrame;
/* Original b378 traffic precedes b474 scenery callback. Caller may retain
   actor packets in the queue before this build; scenery appends afterward.
   Bounded storage; errors preserve already appended packets.
   Rasterizer-specific screen clipping/lighting remains a consumer dependency. */
int taxi_world_frame_build(TaxiWorldFrame *frame);
#endif
