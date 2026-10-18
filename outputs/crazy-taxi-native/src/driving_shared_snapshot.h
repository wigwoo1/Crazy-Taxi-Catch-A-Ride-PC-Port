#ifndef TAXI_DRIVING_SHARED_SNAPSHOT_H
#define TAXI_DRIVING_SHARED_SNAPSHOT_H
#include "driver.h"
#include <stddef.h>
typedef struct {TaxiDriverContext context;TaxiCommand command;int32_t delta,frame_delta;uint8_t settings,flag_1697,flag_153c,freeze_177b,sound_enabled;} TaxiDrivingSharedSnapshot;
/* Diagnostic original-memory import. Action resolver and input call ordering
   remain separate; this does not execute source instructions. */
int taxi_driving_shared_snapshot_read(TaxiDrivingSharedSnapshot *,const uint8_t *,size_t,const uint8_t *,size_t);
#endif
