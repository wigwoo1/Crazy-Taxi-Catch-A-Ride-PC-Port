#ifndef TAXI_TRIP_START_H
#define TAXI_TRIP_START_H
#include <stdint.h>
typedef struct {
    int32_t active_customer, direction_753c;
    int32_t time_16ac, initial_time_16b0, allowance_16e0;
    uint16_t scale_5310, scale_5312, phase_5314;
    uint8_t active_1694, active_1695, voice_25d6;
} TaxiTripStartState;
typedef struct {
    int32_t customer_index, route_length, remaining_16a8, random_result;
    uint8_t category, settings_0, settings_3;
} TaxiTripStartInput;
/* Scalar writes of 0800cb84. Does not run countdown, delivery, audio or palette
   DMA. Normal path requests palette refresh via output flag. Settings low nibble
   1..8 covers the eight recovered table entries; other normal values rejected
   atomically, not emulated as out-of-range ROM reads. No default mode invented. */
int taxi_trip_start(TaxiTripStartState *state,const TaxiTripStartInput *input,
                    uint8_t *palette_refresh);
#endif
