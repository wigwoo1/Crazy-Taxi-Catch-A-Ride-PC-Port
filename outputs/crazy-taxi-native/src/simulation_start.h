#ifndef TAXI_SIMULATION_START_H
#define TAXI_SIMULATION_START_H
#include "drive_world.h"
#include "spawn_placement.h"
/* Fresh native simulation-field projection of placement -> configuration ->
   reset -> initializer tail. Does not initialize render actors, camera,
   audio, random visual RPM or traffic. State/context must represent a fresh
   actor; successful call replaces them. Original current gear is zero, and
   RPM is copied from the template, never forced to a diagnostic active value.
   Returns1 placed,0 ground absent,-1 invalid with output rollback. */
int taxi_simulation_start(TaxiDriveWorld *state,TaxiDriveWorldContext *context,
    const uint8_t *vehicle_template,size_t size,const TaxiSpawnMarker *marker,
    uint8_t settings,TaxiGroundQuery ground,void *query_context);
/* Challenge vehicle-field initialization only. Caller supplies the original
   selected template and GRID_01 marker; settings high bits must equal128 as set
   by1AAF8. Retains lower option bits and installs caller-owned submode1537.
   Transactional when ground is absent/invalid. Render/audio/RNG excluded. */
int taxi_simulation_start_challenge(TaxiDriveWorld *state,TaxiDriveWorldContext *context,
    const uint8_t *vehicle_template,size_t size,const TaxiSpawnMarker *marker,
    uint8_t settings,unsigned submode,TaxiGroundQuery ground,void *query_context);
#endif

