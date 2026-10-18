#ifndef TAXI_TRAFFIC_DRIVE_H
#define TAXI_TRAFFIC_DRIVE_H
#include "traffic_scene.h"
#include <stdio.h>
void taxi_traffic_heading_trace(FILE *sink);
void taxi_traffic_frame_trace(FILE *sink);
#include "drive_world.h"
#include "passenger_scene.h"
/* Bind position/power to traffic and synchronize duplicate native speed/scale. */
int taxi_traffic_drive_update(TaxiTrafficActor *actors,size_t count,
    TaxiTrafficScene *scene,TaxiDriveWorld *drive);
int taxi_traffic_game_update(TaxiTrafficActor *actors,size_t count,
    TaxiTrafficScene *scene,TaxiDriveWorld *drive,TaxiPassengerScene *passengers);
#endif
