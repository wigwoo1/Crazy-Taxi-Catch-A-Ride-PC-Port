#ifndef TAXI_PASSENGER_ASSET_H
#define TAXI_PASSENGER_ASSET_H
#include "passenger_route.h"
typedef struct {
    uint32_t source_index,count;
    TaxiRoutePoint *points;
} TaxiPassengerRouteAsset;
typedef struct {
    uint32_t count,route_count,destination_count;
    uint8_t (*source)[32],(*destinations)[48];
    TaxiPassengerRouteAsset *routes;
} TaxiPassengerAsset;
/* CTP1 preserves source records and ordered route identities. Native points
   project only X/Z; other raw words remain in the asset file. No pointers
   from the ROM are stored or dereferenced. Zero-initialize before loading. */
int taxi_passenger_asset_load(TaxiPassengerAsset *out,const char *path);
void taxi_passenger_asset_free(TaxiPassengerAsset *asset);
#endif
