#include "timing.h"
#include <string.h>
#define PERIOD UINT64_C(4389000000000)
#define RATE UINT64_C(262144)
int32_t taxi_frame_delta(uint32_t current,uint32_t previous) {
    uint32_t u=current-previous;int32_t delta;memcpy(&delta,&u,4);
    return delta<1?1:delta>60?60:delta;
}
int taxi_clock_advance(TaxiSimulationClock *clock,uint64_t elapsed_ns,uint32_t *ticks) {
    if(!clock||!ticks||clock->phase>=PERIOD||elapsed_ns>(UINT64_MAX-clock->phase)/RATE) return -1;
    uint64_t total=clock->phase+elapsed_ns*RATE,whole=total/PERIOD;
    if(whole>UINT32_MAX) return -1;
    clock->phase=total%PERIOD;*ticks=(uint32_t)whole;return 0;
}
double taxi_clock_alpha(const TaxiSimulationClock *clock) {return (double)clock->phase/(double)PERIOD;}
