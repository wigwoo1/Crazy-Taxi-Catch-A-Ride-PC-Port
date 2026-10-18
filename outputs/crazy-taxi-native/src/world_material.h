#ifndef TAXI_WORLD_MATERIAL_H
#define TAXI_WORLD_MATERIAL_H
#include "world_render.h"
#include "world_clip.h"
typedef struct {unsigned texture,count;TaxiWorldClipVertex vertices[4];} TaxiWorldMaterialPolygon;
/* Source036ec..373c animation selection, wrapped multiply/ASR12/remainder. */
int taxi_world_material_texture(const uint8_t material[28],int32_t phase,
    unsigned texture_count,unsigned *texture);
/* Source037bc/03800 static UV or flat shade packet. Both extracted cities
   have zero UV animation rates. This static adapter rejects nonzero scrolling;
   use polygon_scrolling followed by material_uv for the qualified full writes. */
int taxi_world_material_polygon(const TaxiWorldAsset *world,unsigned polygon,
    const TaxiWorldRenderPolygon *geometry,int32_t phase,unsigned texture_count,
    TaxiWorldMaterialPolygon *out);
/* Source3754..37E4 ARM rotated loads; explicit descriptor log dimensions. */
int taxi_world_material_scroll(TaxiWorldMaterialPolygon *,const uint8_t material[28],int32_t phase,unsigned log_width,unsigned log_height);
int taxi_world_material_polygon_scrolling(const TaxiWorldAsset *,unsigned,const TaxiWorldRenderPolygon *,int32_t,unsigned,TaxiWorldMaterialPolygon *);
/* Original3754..37F4 full UV writes including SL register side effect.
   Geometry count retained; nonzero scrolling writes fourth UV slot on triangles. */
int taxi_world_material_uv(TaxiWorldMaterialPolygon *,const uint8_t polygon[24],
    const uint8_t material[28],int32_t phase,unsigned log_width,unsigned log_height);
#endif
