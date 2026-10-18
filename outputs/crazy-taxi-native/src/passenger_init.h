#ifndef TAXI_PASSENGER_INIT_H
#define TAXI_PASSENGER_INIT_H
#include "passenger_route.h"
/* Original runtime record is 44 bytes. Unwritten fields are preserved. */
typedef struct {uint8_t bytes[44];} TaxiPassengerState;
typedef struct {
    uint8_t kind;
    TaxiRoutePoint origin,destination;
    /* Caller resolves first eligible PED_ route and spawn ground height.
       Null route means no eligible route. This is not a guessed Y policy. */
    const TaxiRoutePoint *route;
    size_t point_count;
    int32_t spawn_height;
} TaxiPassengerResolved;
typedef struct {uint8_t spawn_other_actor;} TaxiPassengerInitEffect;
/* Ordinary mode scalar projection of080227ac..08022cc6. Route queries,
   loading progress, destination selection side effects and actor spawning
   are external. Special settings must not call this ordinary-mode routine.
   Up to128 records; error leaves states/effects unchanged. */
int taxi_passengers_init_normal(TaxiPassengerState *states,
    const TaxiPassengerResolved *resolved,size_t count,TaxiPassengerInitEffect *effects);
/* Called before each record in both original passes, including noncustomers.
   Observer can present loading; callbacks already delivered are not rolled
   back on malformed input, though states/effects retain transactional behavior. */
typedef void (*TaxiPassengerInitObserver)(void *context,unsigned pass,size_t index);
int taxi_passengers_init_observed(TaxiPassengerState *states,
    const TaxiPassengerResolved *resolved,size_t count,TaxiPassengerInitEffect *effects,
    TaxiPassengerInitObserver observer,void *context);
/* Original challenge submode1 customer-kind branch22C2A..22C94.
   Ground height is caller-resolved; routes/destinations are not consumed.
   Descriptor087EC47C stored as opaque original identity, never dereferenced. */
int taxi_passenger_init_flag(TaxiPassengerState *state,const TaxiPassengerResolved *resolved);
/* Original227AC challenge branch22986/22C2A..22C94. Retains fields
   untouched by source; submodes3/4/5/7 set signed animation counter600. */
int taxi_passenger_init_challenge(TaxiPassengerState *,const TaxiPassengerResolved *,unsigned submode);
#endif
