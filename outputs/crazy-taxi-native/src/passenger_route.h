#ifndef TAXI_PASSENGER_ROUTE_H
#define TAXI_PASSENGER_ROUTE_H
#include <stdint.h>
#include <stddef.h>
typedef struct {int32_t x,z;} TaxiRoutePoint;
typedef struct {uint8_t length_category,detour_category;} TaxiRouteCategories;
/* 0802289e..28dc: shift each segment delta before squaring, floor sqrt,
   narrow each root to 16 bits, accumulate with wrapping then shift by 8.
   Returns -1 for invalid native arguments without modifying output. */
int taxi_passenger_route_length(const TaxiRoutePoint *points,size_t count,int32_t *length);
/* 08022b38..2c06 arithmetic only. min/max must come from eligible routes.
   Categories' gameplay meaning is not yet established. */
TaxiRouteCategories taxi_passenger_route_categories(int32_t length,int32_t minimum,
    int32_t maximum,TaxiRoutePoint origin,TaxiRoutePoint destination);
#endif
