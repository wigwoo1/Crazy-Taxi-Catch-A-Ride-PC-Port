#ifndef TAXI_RESULTS_STATE_H
#define TAXI_RESULTS_STATE_H
#include <stdint.h>
typedef struct {
    uint8_t phase,customers,rank,exiting;
    int16_t timer,digit,class_scale,rank_scale,class_hold;
    char target[32],cash[32];
} TaxiResultsState;
enum { TAXI_RESULTS_TICK=1,TAXI_RESULTS_CLASS=2,TAXI_RESULTS_RANK=4,TAXI_RESULTS_FADE_OUT=8 };
/* Caller supplies original formatted cash and signed source customer/rank
   bytes. Fade gate and exit flag stop animation as at08023d42..54. */
int taxi_results_state_begin(TaxiResultsState *state,const char *cash);
/* Original08010830 unpadded results call (width byte255/-1). No carry or
   rounding here; settlement must normalize fractional earnings first. */
int taxi_results_cash_format(char output[32],int32_t whole,int32_t fraction);
int taxi_results_state_begin_money(TaxiResultsState *state,int32_t whole,int32_t fraction);
unsigned taxi_results_state_step(TaxiResultsState *state,int32_t delta,
    int8_t customers,int8_t rank,int fade_gate);
#endif
