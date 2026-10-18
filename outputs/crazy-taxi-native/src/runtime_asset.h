#ifndef TAXI_RUNTIME_ASSET_H
#define TAXI_RUNTIME_ASSET_H
#include "simulation_start.h"
typedef struct {
    TaxiTrig trig;TaxiAngleTable traction;TaxiNormalAngles normals;
    uint8_t templates[2][584];TaxiSpawnMarker starts[3];
} TaxiRuntimeAsset;
/* Strict CTR1 loader; no ROM addresses/execution. Atomic output on failure. */
int taxi_runtime_asset_load(TaxiRuntimeAsset *out,const char *path);
/* Original14BEC retains02001708 through14C96: zero ->084A554C,
   any nonzero ->084A5794. Not indexed by selected cabbie1539. */
unsigned taxi_runtime_template_bank(uint8_t override_1708);
#endif

