#ifndef TAXI_WORLD_ASSET_H
#define TAXI_WORLD_ASSET_H
#include "ground.h"
typedef struct {
    uint8_t *storage;size_t size;
    const uint8_t *vertices,*polygons,*materials;
    const uint8_t **cells;
    uint32_t nv,np,nm,nx,nz;
    int32_t sx,sz,ox,oz;
    uint8_t *special_storage;
    uint32_t special_count;
} TaxiWorldAsset;
/* Call on a zero-initialized/freed object. Strict standalone CTW1 loader.
   Never executes ROM or dereferences original ROM addresses. */
int taxi_world_asset_load(TaxiWorldAsset *out,const char *path);
/* Attach CTS1 to an already loaded world; failed replacement preserves it. */
int taxi_world_asset_load_special(TaxiWorldAsset *world,const char *path);
void taxi_world_asset_free(TaxiWorldAsset *asset);
int taxi_world_asset_ground(void *asset,TaxiPoint point,TaxiGroundHit *hit);
/* Complete0802ABA0 contact query on a loaded world. Outside/NULL cells retain
   every contact byte; a valid cell writes material65535 even on no hit. The
   interpolation word retains its previous value unless the query writes it.
   Return original nonzero-normal boolean, or -1 for invalid native input/data.
   The point is read-only; caller owns its retained Y and contact scratch. */
int taxi_world_asset_contact(void *asset,TaxiPoint point,TaxiWorldContact *contact);
int taxi_world_asset_height(void *asset,TaxiPoint point,int32_t *height);
int taxi_world_asset_obstacle(void *asset,TaxiPoint point,TaxiObstacleHit *hit);
#endif
