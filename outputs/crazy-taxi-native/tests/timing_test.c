#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "../src/timing.h"
int main(void) {
    assert(taxi_frame_delta(5,5)==1&&taxi_frame_delta(5,6)==1);
    assert(taxi_frame_delta(100,0)==60&&taxi_frame_delta(60,0)==60);
    assert(taxi_frame_delta(1,UINT32_MAX)==2&&taxi_frame_delta(0x80000000,0)==1);
    TaxiSimulationClock whole={0},split={0};uint32_t ticks,count=0;
    assert(!taxi_clock_advance(&whole,UINT64_C(1000000000),&ticks)&&ticks==59);
    for(unsigned i=0;i<1000;i++) {assert(!taxi_clock_advance(&split,1000000,&ticks));count+=ticks;}
    assert(count==59&&split.phase==whole.phase&&taxi_clock_alpha(&whole)>0&&taxi_clock_alpha(&whole)<1);
    whole=(TaxiSimulationClock){0};assert(!taxi_clock_advance(&whole,UINT64_C(4389000000000),&ticks));
    assert(ticks==262144&&!whole.phase);
    uint64_t old=whole.phase;ticks=123;
    assert(taxi_clock_advance(&whole,UINT64_MAX,&ticks)==-1&&whole.phase==old&&ticks==123);
    puts("PASS: original frame delta and exact-rate native clock, partition invariance and overflow rejection.");return 0;
}
