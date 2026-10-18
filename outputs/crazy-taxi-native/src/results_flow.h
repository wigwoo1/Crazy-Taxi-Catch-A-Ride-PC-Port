#ifndef TAXI_RESULTS_FLOW_H
#define TAXI_RESULTS_FLOW_H
#include "results_state.h"
#include "palette_fade.h"
typedef struct {
    TaxiResultsState animation;
    TaxiPaletteFade fade;
    int16_t palette_cap;
    int8_t customers,rank,license;
    uint8_t complete,unlock_city;
} TaxiResultsFlow;
/* Original2392c entry. Fade interval/reload are retained runtime fields,
   supplied explicitly rather than assuming a new interval from arguments. */
int taxi_results_flow_begin(TaxiResultsFlow *flow,int32_t whole,int32_t fraction,
    int8_t customers,int8_t rank,int8_t license,uint8_t interval,uint8_t reload);
/* Source23d42..2406e: animation first, then26318 palette processing/step,
   then exit/fade-completion and city-unlock check. Does not implement the
   subsequent253e4/24098/24674 name-entry/Game Over/profile-save screens. */
unsigned taxi_results_flow_step(TaxiResultsFlow *flow,int32_t delta,uint8_t city_unlocked);
#endif
