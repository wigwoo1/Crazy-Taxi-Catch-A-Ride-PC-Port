#ifndef TAXI_PASSENGER_PICKUP_H
#define TAXI_PASSENGER_PICKUP_H
#include "passenger_init.h"
typedef struct {
    int32_t pending_1704,selected_index,dialogue_index,destination_index;
    int32_t approach_x_7534,approach_z_7538;
    uint8_t animation_25d7,voice_25d6;
} TaxiPickupState;
typedef struct {
    int32_t candidate_index; /* Result of original08034074, -1 for none. */
    TaxiRoutePoint taxi_position;
    int32_t gate_2b78,gate_1700;
    int32_t random_first,random_second; /* Supplied original RNG results. */
} TaxiPickupInput;
typedef struct {
    uint8_t began,completed,random_calls;
    /* Ordered completion requests: destination select,0800cb84,0800c540. */
    int32_t customer_index,destination_index;
} TaxiPickupEffects;
/* 0800b098..0800b20a pickup branches only. Candidate marking/search,
   approach animation, timers, audio and the flag16 dialogue branch are
   external dependencies; this does not implement a complete trip loop.
   Index fields replace original pointers; invalid inputs roll back. */
int taxi_passenger_pickup(TaxiPickupState *state,TaxiPassengerState *customers,
    const uint8_t *destination_indices,size_t count,const TaxiPickupInput *input,
    TaxiPickupEffects *effects);
#endif
