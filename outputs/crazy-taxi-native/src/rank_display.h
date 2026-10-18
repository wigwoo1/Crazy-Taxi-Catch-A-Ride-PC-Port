#ifndef TAXI_RANK_DISPLAY_H
#define TAXI_RANK_DISPLAY_H
#include <stdint.h>
typedef struct {
    uint8_t active_16d5, phase_16d6;
    int32_t position_16d8, hold_16dc;
    uint16_t current_5388[2], target_538e[2];
} TaxiRankDisplay;
/* Recovered CA0C. Trigger byte 1 starts AND advances the panel.
   Caller owns initialization, rank, delta and invocation order. No rendering. */
void taxi_rank_display_step(TaxiRankDisplay *, uint8_t settings,
                            uint8_t rank, uint8_t trigger, int32_t delta);
#endif
