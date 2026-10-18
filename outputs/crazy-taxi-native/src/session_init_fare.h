#ifndef TAXI_SESSION_INIT_FARE_H
#define TAXI_SESSION_INIT_FARE_H
#include "session_init.h"
#include "fare_session.h"
/* Bridge B6EC represented fare/trip/expiry fields to existing gameplay owners.
   Full512settings already belong to init. Settings/clock imports happen before
   reset; ranking/tips/reward/voice/distance state retained unless B6EC writes. */
int taxi_session_init_fare(TaxiSessionInit *,TaxiFareSession *,uint16_t);
#endif
