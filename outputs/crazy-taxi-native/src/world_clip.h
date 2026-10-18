#ifndef TAXI_WORLD_CLIP_H
#define TAXI_WORLD_CLIP_H
#include "traffic_presentation.h"
/* Original queue vertex32-byte layout; reserved is not a drawable attribute. */
typedef struct {int32_t screen_x,u,v,screen_y,x,y,z,attribute;} TaxiWorldClipVertex;
typedef struct {unsigned count;TaxiWorldClipVertex vertices[8];} TaxiWorldClippedPolygon;
/* Source04424 near(z>plane) /04704 far(z<plane). Retains output on cull/error.
   Geometry multiplies before divide; UV/attribute divide before multiply. */
int taxi_world_clip_depth(const TaxiWorldClipVertex *vertices,unsigned count,
    int32_t plane,int far_clip,TaxiWorldClippedPolygon *out);
/* Source04620..46d4/04900..49b4: project and cull screen bounding box.
   Clip rectangle is separate from the unaligned projection centers. */
int taxi_world_clip_project(TaxiWorldClippedPolygon *polygon,
    const TaxiTrafficProjection *projection,int32_t left,int32_t top,
    int32_t right,int32_t bottom);
/* Original35B0..3644 screen projection and min/max without clip rejection.
   Same wrapped arithmetic as the bounded clip adapter; outputs retain on error. */
int taxi_world_project_vertices(TaxiWorldClippedPolygon *,const TaxiTrafficProjection *,TaxiTrafficScreenBounds *);
#endif
