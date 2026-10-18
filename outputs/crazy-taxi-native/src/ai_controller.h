#ifndef TAXI_AI_CONTROLLER_H
#define TAXI_AI_CONTROLLER_H
#include "ai_path.h"
#include "drive_world.h"
typedef struct {
    int32_t position[3],heading_1c,heading_20,speed_544;
    uint32_t phase_378;
    uint8_t command_4;
    uint16_t command_c;
} TaxiAiVehicle;
/* Native5554c with source PATH_AI. Counter initialization/traffic ownership
   belongs to caller; absent path diagnostic returns-1 preserving state. */
int taxi_ai_controller_step(TaxiAiVehicle *vehicle,uint32_t player_phase,
    uint32_t *counter,const TaxiAiPathAsset *path);
/* Bridge to existing native world state; ground contact378 owns path phase.
   This prepares command/state before ordinary drive_world_step. */
int taxi_ai_world_prepare(TaxiDriveWorld *vehicle,const TaxiDriveWorld *player,
    TaxiCommand *command,uint32_t *counter,const TaxiAiPathAsset *path);
#endif
