#ifndef TAXI_LOADING_PRESENT_BYPASS_H
#define TAXI_LOADING_PRESENT_BYPASS_H
#include <stdint.h>
typedef struct { uint16_t width_7542,height_7544; uint8_t settings_1eb0,override_1708; } TaxiLoadingPresentGate;
typedef int (*TaxiLoadingClearCall)(void *,TaxiLoadingPresentGate *,uint32_t,const uint32_t[4]);
/* D764..D796 and D89C return: clear first, then re-read the shared gates.
   Returns1 for the complete attract/special-mode bypass,0 when the ordinary
   text/image/presentation body remains required,-1 on service failure.
   Does not advance a fade or insert an absent 26318 call. */
int taxi_loading_present_bypass(TaxiLoadingPresentGate *,TaxiLoadingClearCall,void *);
#endif
