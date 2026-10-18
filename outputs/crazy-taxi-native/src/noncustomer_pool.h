#ifndef TAXI_NONCUSTOMER_POOL_H
#define TAXI_NONCUSTOMER_POOL_H
#include "world_asset.h"
typedef struct {uint8_t bytes[64];} TaxiNoncustomerState;
typedef struct {
    TaxiNoncustomerState *actors;size_t capacity;
    uint16_t *count_430c; /* Same canonical owner reset by native B6EC. */
    TaxiWorldAsset *world;
    const uint32_t *bitmap_identities; /* Four original087EC068 words. */
} TaxiNoncustomerPool;
/* Decode the exact16 immutable table bytes extracted from the user's ROM.
   These are source identities, never executable host addresses. */
int taxi_noncustomer_bitmaps_decode(uint32_t output[4],const uint8_t *,size_t);
/* Complete1E9D0 owned pool/real29B44 ground path for original city kinds10..13.
   Full signed count64..32767 is a successful no-op, without source/ground reads.
   Negative counts reject instead of accessing before native pool storage.
   Retains slotY as the height query hint; count increments only on completion.
   Ground failure retains source prefix writes, matching caller-visible order.
   Returns0 on source completion/fullpool, -1 on invalid native data/domain. */
int taxi_noncustomer_pool_spawn(TaxiNoncustomerPool *,const uint8_t source[32]);
/* TaxiPassengerInitWorld.spawn adapter; source index does not select pool slot. */
int taxi_noncustomer_pool_passenger(void *,size_t index,const uint8_t source[32]);
#endif
