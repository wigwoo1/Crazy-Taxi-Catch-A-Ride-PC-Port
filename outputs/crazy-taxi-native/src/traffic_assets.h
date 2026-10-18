#ifndef TAXI_TRAFFIC_ASSETS_H
#define TAXI_TRAFFIC_ASSETS_H
#include "traffic_grid.h"
typedef struct {TaxiTrafficGrid grid;TaxiAiPathAsset roads[256];unsigned road_count;} TaxiTrafficAssets;
/* Zero-initialize before use. Loads every route referenced by selectable grid
   slots, retaining old city assets on any failure. No ROM code is executed. */
int taxi_traffic_assets_load(TaxiTrafficAssets *assets,const char *directory,unsigned city);
void taxi_traffic_assets_free(TaxiTrafficAssets *assets);
#endif
