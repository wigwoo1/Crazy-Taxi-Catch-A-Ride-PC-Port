#ifndef TAXI_LOADING_FLOW_H
#define TAXI_LOADING_FLOW_H
#include "palette_fade.h"
enum { TAXI_LOADING_IN, TAXI_LOADING_WORK, TAXI_LOADING_OUT, TAXI_LOADING_READY };
typedef struct {
    TaxiPaletteFade fade;
    int16_t palette_cap;
    uint8_t phase,animation_bank;
} TaxiLoadingFlow;
/* Normal startup ad86/ae34 loading fade, then b6ec bank reset. */
void taxi_loading_flow_begin(TaxiLoadingFlow *state,uint8_t retained_bank);
/* One d764/26318 presentation call; caller supplies original frame delta.
   Palette cap is captured before fade progression. No invented work timer. */
void taxi_loading_flow_present(TaxiLoadingFlow *state,int32_t delta);
/* 227ac/229de: call BEFORE processing an original initialization record.
   Returns1 exactly when index%5==0 advances bank before d764. */
int taxi_loading_flow_record(TaxiLoadingFlow *state,unsigned index);
/* ae86: caller signals actual completion of all original loading work.
   READY transfers to caller-owned clear/city fade, not frozen gameplay. */
int taxi_loading_flow_finish(TaxiLoadingFlow *state);
#endif
