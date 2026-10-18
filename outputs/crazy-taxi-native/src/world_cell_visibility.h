#ifndef TAXI_WORLD_CELL_VISIBILITY_H
#define TAXI_WORLD_CELL_VISIBILITY_H
#include <stdint.h>
typedef struct {int32_t spacing_x,spacing_z,origin_x,origin_z,camera_x,camera_z,sin_yaw,cos_yaw,sin_pitch,cos_pitch,horizontal,near_plane,far_plane;} TaxiWorldCellVisibility;
/* Source2EC0..2FAC: retained intermediate coordinates and final flag shift. */
int taxi_world_cell_visibility(const TaxiWorldCellVisibility *s,uint32_t cell_x,uint32_t cell_z,uint32_t *packed_flags,uint32_t *shared_flags);
#endif
