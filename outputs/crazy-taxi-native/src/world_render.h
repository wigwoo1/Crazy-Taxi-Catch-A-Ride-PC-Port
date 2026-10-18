#ifndef TAXI_WORLD_RENDER_H
#define TAXI_WORLD_RENDER_H
#include "world_asset.h"
#include "traffic_presentation.h"
typedef struct {
    TaxiPoint vertices[4];
    unsigned count,clip_flags,bucket;
    int32_t depth,max_depth;
} TaxiWorldRenderPolygon;
typedef struct {int32_t horizontal,vertical;} TaxiWorldRenderFrustum;
/* Source2c7f6..2c818, unaligned viewport span wrapped<<7 divided by192. */
TaxiWorldRenderFrustum taxi_world_render_frustum(int32_t left,int32_t top,
    int32_t right,int32_t bottom);
typedef struct {int32_t min_x,min_z,max_x,max_z;} TaxiWorldRenderBounds;
/* Source2b330 eight near/far corners,02d18 positivepitch thenpositiveyaw,
   wrapped world bounds and signed grid division. Rect uses aligned clip edges. */
int taxi_world_render_bounds(const TaxiWorldAsset *world,TaxiPoint camera,
    int32_t yaw,int32_t pitch,const TaxiTrig *trig,const TaxiTrafficProjection *projection,
    int32_t left,int32_t top,int32_t right,int32_t bottom,TaxiWorldRenderBounds *out);
/* Original2D18 consumes existing engine+50 corners; viewport setters do not
   regenerate them. Fourth words are retained and ignored by this XZ scan.
   This owner-based API does not advance generation or reset polygon caches. */
int taxi_world_render_bounds_corners(const TaxiWorldAsset *,TaxiPoint,int32_t,int32_t,
    const TaxiTrig *,const int32_t [8][4],TaxiWorldRenderBounds *);

typedef int (*TaxiWorldCellVisible)(void *context,unsigned x,unsigned z);
typedef int (*TaxiWorldReferenceVisit)(void *context,unsigned reference);
/* Source02eb0..02fd4 row-major inclusive cell traversal and031ec normal
   polygon first-visit cache. Caller supplies recovered frustum bounds/culling.
   Source02f74 shifts after the final corner, so02f84 reads zero low6bits
   and common-plane cell rejection never fires. NULL visibility preserves this.
   High-bit references are forwarded unchanged for the special producer. */
int taxi_world_render_visit(const TaxiWorldAsset *world,int32_t min_x,int32_t min_z,
    int32_t max_x,int32_t max_z,uint8_t *seen,size_t seen_size,
    TaxiWorldCellVisible visible,TaxiWorldReferenceVisit visit,void *context);
/* Persistent original polygon stamp owner; no cache clearing. Caller owns
   generation increment/snapshot restoration. Special references pass unchanged. */
int taxi_world_render_visit_stamped(const TaxiWorldAsset *,int32_t,int32_t,int32_t,int32_t,uint32_t *,size_t,uint32_t,TaxiWorldCellVisible,TaxiWorldReferenceVisit,void *);
/* Both original persistent stamp owners. Special bit14 references skip before
   cache access; caller owns generation and frame/preview restoration. */
int taxi_world_render_visit_cached(const TaxiWorldAsset *,int32_t,int32_t,int32_t,int32_t,
    uint32_t *,size_t,uint32_t *,size_t,uint32_t,TaxiWorldCellVisible,TaxiWorldReferenceVisit,void *);
/* Original3264..32B0 bit0 two-sided bypass and wrapped signed-normal dot gate.
   Material rejection/cache stamping remain caller-owned. */
int taxi_world_polygon_facing(const TaxiWorldAsset *,unsigned,TaxiPoint);
/* Original32FC..33E0 vertex transform and six inclusive clip-plane flags. */
int taxi_world_render_vertex(TaxiPoint,TaxiPoint,int32_t,int32_t,const TaxiTrig *,
    const TaxiTrafficProjection *,int32_t,int32_t,TaxiPoint *,unsigned *);
/* Original33F0/3548 common-plane reject, union flags, wrapped mean depth.
   Zero retains caller output. Source vertex-body max starts at zero. */
int taxi_world_render_aggregate(const TaxiPoint *,const unsigned *,unsigned,TaxiWorldRenderPolygon *);
/* Source031ec ordinary polygon producer, before software clipping/rasterization.
   Returns1 candidate,0 culled,-1 invalid; output retained on0/-1.
   Frustum horizontal/vertical factors are original engine+38/+3c Q8 values. */
int taxi_world_render_polygon(const TaxiWorldAsset *world,unsigned polygon,
    TaxiPoint camera,int32_t yaw,int32_t pitch,const TaxiTrig *trig,
    const TaxiTrafficProjection *projection,int32_t horizontal,int32_t vertical,
    TaxiWorldRenderPolygon *out);
typedef struct {
    unsigned material,clip_flags,bucket;
    int32_t depth;
    TaxiTrafficBillboard billboard;
    TaxiTrafficScreenBounds screen;
} TaxiWorldRenderSpecial;
/* Source038d0..03aa8 special billboard: normal near/far, raw view-space
   rectangle offsets, queue depth offset0. Caller owns separate firstvisit cache. */
int taxi_world_render_special(const TaxiWorldAsset *world,unsigned index,
    TaxiPoint camera,int32_t yaw,int32_t pitch,const TaxiTrig *trig,
    const TaxiTrafficProjection *projection,TaxiWorldRenderFrustum frustum,
    TaxiWorldRenderSpecial *out);
#endif
