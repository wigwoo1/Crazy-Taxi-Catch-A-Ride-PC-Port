#ifndef EARNED_CAPTURE_H
#define EARNED_CAPTURE_H
#include "drive_world.h"
#include "input.h"
#include "xbox.h"
/* Local diagnostic ABI only. Same compiler/header versions required for replay.
   Captures the driver/world boundary, before traffic responses. */
typedef struct {
 TaxiDriveWorld before,after;
 TaxiDriveWorldContext context_before,context_after;
 TaxiInput buttons_before,buttons_sampled;
 TaxiXboxSample pad;
 TaxiCommand command;
 TaxiGearInput input;
 uint32_t counter;
 int32_t delta;
 unsigned world_hit;
} EarnedCapture;
#endif
