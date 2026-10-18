#ifndef TAXI_CHALLENGE_ASSETS_H
#define TAXI_CHALLENGE_ASSETS_H
#include "world_asset.h"
#include "indexed_asset.h"
#include "spawn_placement.h"
typedef struct {TaxiWorldAsset world;TaxiIndexedAsset textures;TaxiSpawnMarker spawn;
 uint8_t material_prefix[28];uint16_t palette[256];unsigned submode;} TaxiChallengeAssets;
/* Native host loader; zero-initialize. Complete replacement commits only on
   success; source runtime palette mutation and GPU ownership are separate. */
int taxi_challenge_assets_load(TaxiChallengeAssets *out,const char *directory,unsigned submode);
void taxi_challenge_assets_free(TaxiChallengeAssets *out);
#endif
