#ifndef TAXI_TRAFFIC_GRID_H
#define TAXI_TRAFFIC_GRID_H
#include <stdint.h>
#include <stddef.h>
#include "traffic_path.h"
typedef struct {uint32_t source_pointer;uint8_t routes[8];uint16_t phases[8];} TaxiTrafficCell;
typedef struct {
    uint16_t width,height;
    int32_t cell_x,cell_z,origin_x,origin_z;
    TaxiTrafficCell *cells;
} TaxiTrafficGrid;
typedef struct {size_t cell;uint8_t route;int32_t phase;} TaxiTrafficSpawn;
int taxi_traffic_grid_load(TaxiTrafficGrid *grid,const char *filename);
void taxi_traffic_grid_free(TaxiTrafficGrid *grid);
/* Source 6734..67b8 and6bc0..6c1c; no RNG or counter mutation here.
   Returns 1 for selection, 0 for absent cell/slots/outside grid, -1 invalid. */
int taxi_traffic_grid_select(const TaxiTrafficGrid *grid,int32_t x,int32_t z,
                            uint32_t counter,TaxiTrafficSpawn *spawn);
int taxi_traffic_grid_path_spawn(const TaxiTrafficGrid *grid,int32_t x,int32_t z,
    uint32_t counter,const TaxiAiPathAsset roads[256],TaxiTrafficSpawn *spawn,
    TaxiTrafficPathState *path);
#endif
