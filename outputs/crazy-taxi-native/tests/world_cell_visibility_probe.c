#include "world_cell_visibility.h"
#include <stdio.h>
int main(void){TaxiWorldCellVisibility s;unsigned x,z;while(scanf("%d%d%d%d%d%d%d%d%d%d%d%d%d%u%u",&s.spacing_x,&s.spacing_z,&s.origin_x,&s.origin_z,&s.camera_x,&s.camera_z,&s.sin_yaw,&s.cos_yaw,&s.sin_pitch,&s.cos_pitch,&s.horizontal,&s.near_plane,&s.far_plane,&x,&z)==15){uint32_t packed,shared;int rc=taxi_world_cell_visibility(&s,x,z,&packed,&shared);printf("%d %u %u\n",rc,packed,shared);}return 0;}
