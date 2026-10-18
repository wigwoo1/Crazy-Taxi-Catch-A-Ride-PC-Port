#ifndef TAXI_FARE_SESSION_H
#define TAXI_FARE_SESSION_H
#include "game_settings.h"
#include "fare.h"
#include "reward_update.h"
#include "trip_clock.h"
#include "passenger_pickup.h"
typedef struct {
    TaxiGameSettings settings;TaxiMainClock clock;TaxiTripStartState trip;
    TaxiTripEndState end;TaxiFareState fare;
    int32_t gate_1700;uint8_t ended_1697,outer_state_1ea8;
    uint8_t audio_muted_1534; /* SourceB782 reset0; challenge-failure writers remain separate. */
    TaxiExpiryDisplay expiry;TaxiDistanceDisplay distance_display;
    uint32_t ranking_scores[100];
    uint8_t ranking_loaded,rank_16d4;
    uint8_t tips[10][56];
    uint8_t reward_gate_9aa4;TaxiRewardClock reward_clock;
    int32_t combo_whole_9aa8,combo_fraction_9aac;
} TaxiFareSession;
/* Keep settled pending glyph targets: original C540(0) changes total targets
   only; C540(-1) animates the retained rows. Pickup and tip updates own new
   pending values. The native active-trip refresh covers its pickup handoff. */
void taxi_fare_session_cash_targets(const TaxiFareSession *,TaxiCashGlyphs *);
void taxi_fare_settlement_trace(FILE *sink);
void taxi_fare_session_init(TaxiFareSession *session);
/* Normal expiry branch. Returns request for original fade(0,32,1,1).
   Fade completion, results and outer-state cleanup belong to the caller. */
int taxi_fare_session_expiry_step(TaxiFareSession *session,int32_t delta);
/* Called for native pickup completion: original timer then fare-start order. */
int taxi_fare_session_pickup(TaxiFareSession *session,const TaxiPassengerState *customer,
    int32_t customer_index,int32_t random_result);
/* After caller's delivery acceptance: clear destination, collect tips,
   trip-end, normalize/settle. Exit XYZ must come from original resolver.
   Timeout uses success0: leaves pending money un-settled like original. */
int taxi_fare_session_end(TaxiFareSession *session,TaxiPickupState *pickup,
    TaxiPassengerState *customer,const TaxiTripEndInput *exit,TaxiTripEndEffects *effects);
/* Original late-frame1700 decrement; entry clock gates must be snapshotted.
   Call after clock transfer, never before main clock update. */
void taxi_fare_session_gate_step(TaxiFareSession *session,const TaxiClockInput *input);
/* Accepted Turn delivery07970..079A8 only. Source order: destination
   removal, numeric2939C tip collection, CD38(1), retained16A8 rank gate.
   Caller owns destination panel clear, tip visual snapping,9AB0(rank).
   Returns request_rank separately; no ordinary money settlement. */
int taxi_fare_session_end_turn(TaxiFareSession *,TaxiPickupState *,TaxiPassengerState *,const TaxiTripEndInput *,TaxiTripEndEffects *,uint8_t *request_rank);
/* Caller-owned glyph state is committed only after successful end processing. */
int taxi_fare_session_end_display(TaxiFareSession *,TaxiPickupState *,TaxiPassengerState *,const TaxiTripEndInput *,TaxiTripEndEffects *,TaxiCashGlyphs *);
int taxi_fare_session_end_turn_display(TaxiFareSession *,TaxiPickupState *,TaxiPassengerState *,const TaxiTripEndInput *,TaxiTripEndEffects *,uint8_t *,TaxiCashGlyphs *);
#endif
