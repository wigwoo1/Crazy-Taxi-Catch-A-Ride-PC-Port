#ifndef TAXI_TRIP_END_H
#define TAXI_TRIP_END_H
#include "trip_start.h"
#include "passenger_init.h"
typedef struct {
    uint8_t result_16d0, phase_16d1, completed_1692, flag_9aa6;
    uint16_t result_timer_16d2;
    int32_t bonus_16e8, bonus_display_timer_16e4, displayed_bonus_16ec;
} TaxiTripEndState;
typedef struct {
    uint8_t settings_0, success;
    int32_t random_result;
    /* Explicit outputs of original rotated taxi-side offset + 08029b44.
       Caller must resolve these, not substitute a guessed constant height. */
    int32_t exit_x,exit_y,exit_z;
} TaxiTripEndInput;
typedef struct {
    int32_t remaining_percent;
    int32_t handler_2901c_argument;
    int32_t handler_2901c_fraction; /* Request 0802901c(1, whole, fraction).
       Active1694 is already zero: original handler clears combo state only. */
    uint8_t palette_refresh, random_calls;
} TaxiTripEndEffects;
/* Ordinary mode scalar/customer writes of 0800cd38..0800cf62.
   Special mode80 rejected atomically. Does not compute exit position, execute
   palette/audio/2901c effects, clear destination or settle fare money. */
int taxi_trip_end_normal(TaxiTripStartState *trip,TaxiTripEndState *end,
    TaxiPassengerState *selected,const TaxiTripEndInput *input,TaxiTripEndEffects *effects);
/* Crazy Turn submode4, originalCD38 mode80 branch. Retains ordinary
   result/bonus/time/voice fields; no RNG/palette/fare handler request. */
int taxi_trip_end_turn(TaxiTripStartState *,TaxiTripEndState *,TaxiPassengerState *,const TaxiTripEndInput *,TaxiTripEndEffects *);
#endif
