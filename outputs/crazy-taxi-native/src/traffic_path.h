#ifndef TAXI_TRAFFIC_PATH_H
#define TAXI_TRAFFIC_PATH_H
#include "ai_path.h"
typedef struct {
    int32_t distance;
    size_t segment;
    int32_t x,z;
    uint16_t heading;
} TaxiTrafficPathState;
/* Loads CTP1 original traffic points, preserving prior asset on failure.
   On success replaces and frees prior owned points. */
int taxi_traffic_path_load(TaxiAiPathAsset *asset,const char *filename);
/* Source 0802e190, called with spawn phase (cell halfword << 2).
   One-point paths write only x/z. Invalid paths preserve state. */
int taxi_traffic_path_initialize(const TaxiAiPathPoint *points,size_t count,
                                int32_t phase,TaxiTrafficPathState *state);
/* Source 0802e264: advance initialized state by signed distance increment. */
int taxi_traffic_path_advance(const TaxiAiPathPoint *points,size_t count,
                             int32_t increment,TaxiTrafficPathState *state);
#endif
