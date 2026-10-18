#ifndef TAXI_ATTRACT_SESSION_H
#define TAXI_ATTRACT_SESSION_H
#include "attract_flow.h"
typedef struct {
 TaxiAttractFlow flow;
 uint8_t active,saved_city,driver;
} TaxiAttractSession;
/* Resolved original random_range(0,65535) result supplied by shared RNG owner.
   These state transitions do not load assets, drive, present or play audio. */
int taxi_attract_session_begin(TaxiAttractSession *state,uint8_t settings[8],
 uint8_t driver,uint16_t city_random);
/* Source B580/B590 gate, B674 city restore and DEA8 cleanup. Return1 requests
   music stop and title parent; caller owns those effects and fade service. */
int taxi_attract_session_finish(TaxiAttractSession *state,uint8_t settings[8],
 uint8_t fade_active);
#endif
