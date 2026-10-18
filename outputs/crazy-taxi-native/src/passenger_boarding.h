#ifndef TAXI_PASSENGER_BOARDING_H
#define TAXI_PASSENGER_BOARDING_H
#include "passenger_pickup.h"
/* Selected-customer boarding portion of08022fe8/23070/23110..2318c.
   Freeze is original byte02005f20. Includes selected actor gates/culling;
   idle animation, flag64 fade and sprite drawing remain external.
   ROM animation address in record+28 is an identifier, never dereferenced. */
int taxi_passenger_boarding(TaxiPassengerState *customer,int selected,
    const TaxiPickupState *pickup,TaxiRoutePoint taxi,int32_t delta,uint8_t freeze);
#endif
