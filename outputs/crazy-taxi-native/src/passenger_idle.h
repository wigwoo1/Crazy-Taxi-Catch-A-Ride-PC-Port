#ifndef TAXI_PASSENGER_IDLE_H
#define TAXI_PASSENGER_IDLE_H
#include "passenger_boarding.h"
#include <stdio.h>
void taxi_passenger_idle_trace(FILE *sink);
/* Original08022f72 fade counter precedes freeze gate; hide occurs only at
   gesture termination080230ee, not immediately at counter90. RNG outputs
   are explicit: gesture chooser and idle delay requested in range120..600. */
int taxi_passenger_idle(TaxiPassengerState *state,int selected,
    const TaxiPickupState *pickup,TaxiRoutePoint taxi,int32_t delta,uint8_t freeze,
    int32_t gesture_random,int32_t idle_delay_random);
/* Consumes shared RNG only at original expiration/sentinel sites. */
int taxi_passenger_idle_rng(TaxiPassengerState *state,int selected,
    const TaxiPickupState *pickup,TaxiRoutePoint taxi,int32_t delta,uint8_t freeze,
    uint32_t *rng);
/* Legal ordinary kinds0..7/stages0..3; frame byte uses source signedness. */
int taxi_passenger_frame_select(TaxiPassengerState *);
typedef struct {uint8_t clock_5f28,frame_5f29,direction_5f2a;} TaxiPassengerMarkerPhase;
int taxi_passenger_marker_phase_step(TaxiPassengerMarkerPhase *,int32_t delta,uint8_t freeze);
typedef struct {uint32_t descriptor,scale;int32_t x,y,z;uint32_t flags,flip;} TaxiPassengerMarkerRequest;
/* Original normal23304..2338C caller requests only, not31A50 projection. */
int taxi_passenger_marker_requests(const TaxiPassengerState *,int32_t pending,int32_t active,uint8_t phase,TaxiPassengerMarkerRequest out[2],unsigned *count);
#endif
