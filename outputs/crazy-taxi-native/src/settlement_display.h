#ifndef TAXI_SETTLEMENT_DISPLAY_H
#define TAXI_SETTLEMENT_DISPLAY_H
#include "trip_end.h"
#include "fare.h"
#include "rank_display_owned.h"
typedef struct {
    uint8_t fade_177b,outer_1ea8,start_1696,entry_ended_1697,freeze_5f20,settings_0;
    int32_t delta_3b00;
} TaxiSettlementDisplayInput;
/* BA04 entry gates. Newly expired main time does not change entry_ended. */
int taxi_settlement_display_allowed(const TaxiSettlementDisplayInput *);
/* OriginalC316..C354: result timer, C540(-1,-1), then CA0C(0).
   Cash/rank views are optional for scalar-only users; no initialization here. */
int taxi_settlement_display_step(TaxiTripEndState *,TaxiCashGlyphs *,
    TaxiRankDisplayOwner *,uint8_t rank,const TaxiSettlementDisplayInput *);
#endif
