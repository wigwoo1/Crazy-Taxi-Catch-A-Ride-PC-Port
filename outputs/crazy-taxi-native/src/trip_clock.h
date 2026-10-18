#ifndef TAXI_TRIP_CLOCK_H
#define TAXI_TRIP_CLOCK_H
#include "trip_end.h"
typedef struct {int32_t remaining_16a8;} TaxiMainClock;
typedef struct {uint8_t phase_16c2;int32_t offset_16c4,hold_16c8;} TaxiDistanceDisplay;
/* Source c224..c2fa, before trip-clock subtraction. Select is action6's edge. */
void taxi_distance_display_step(TaxiDistanceDisplay *display,TaxiTripStartState *trip,
    int32_t delta,uint8_t select_pressed);
/* Source b4f6..b51e: conditional map bitmap request; bitmap ownership external. */
int taxi_distance_display_position(const TaxiDistanceDisplay *display,
    const TaxiTripStartState *trip,int32_t *x,int32_t *y);
/* Original normal expiry animation0800bd76..bdf8. Completion requests
   caller state2/flag1697 clear; the surrounding transition is not modeled. */
typedef struct {int16_t phase_16bc,hold_16be,scale_16c0;} TaxiExpiryDisplay;
void taxi_expiry_display_reset(TaxiExpiryDisplay *display);
int taxi_expiry_display_step(TaxiExpiryDisplay *display,int32_t delta);
typedef struct {
    int32_t delta_3b00,gate_1700;
    uint8_t settings_0,freeze_5f20,ended_1697;
} TaxiClockInput;
/* Phases preserve original call order. Main expiry emits a game-end request;
   caller must perform it before later phases. Trip expiry emits destination
   clear + trip_end(0); caller MUST dispatch before allowance transfer. */
int taxi_main_clock_step(TaxiMainClock *clock,const TaxiClockInput *input);
int taxi_trip_clock_step(TaxiTripStartState *trip,const TaxiClockInput *input);
/* Later0800c354..0800c40c transfers allowance/bonus, then displaytimer. Returns
   palette-restore request when intermediate remaining/60>9. No rendering. */
int taxi_clock_transfer(TaxiMainClock *clock,TaxiTripStartState *trip,
    TaxiTripEndState *end,const TaxiClockInput *input);
#endif
