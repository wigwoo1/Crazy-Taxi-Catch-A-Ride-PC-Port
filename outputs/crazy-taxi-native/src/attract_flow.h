#ifndef TAXI_ATTRACT_FLOW_H
#define TAXI_ATTRACT_FLOW_H
#include <stdint.h>
typedef struct {
 int32_t elapsed,blink_elapsed;
 uint8_t blink_enabled,blink_frame,phase,exit_state,exit_gate;
} TaxiAttractFlow;
/* Source db70..dc2a only. Host owns playback and fade completion. */
void taxi_attract_flow_begin(TaxiAttractFlow *state);
int taxi_attract_flow_step(TaxiAttractFlow *state,int32_t delta,
 uint8_t fade,uint8_t confirm,uint8_t start);
#endif
