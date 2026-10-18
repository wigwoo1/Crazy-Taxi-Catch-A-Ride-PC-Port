#include "../src/results_flow.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(void) {
    TaxiResultsFlow s;
    assert(!taxi_results_flow_begin(&s,0,0,0,99,8,0,0));
    for(int i=0;i<33;i++) {
        assert(!taxi_results_flow_step(&s,1,0));
        assert(s.animation.phase==0&&s.animation.timer==0);
        assert(s.palette_cap==(i<32?i:31));
    }
    assert(!s.fade.active_177b);
    for(int i=0;i<6;i++)taxi_results_flow_step(&s,1,0);
    assert(s.animation.phase==0);
    taxi_results_flow_step(&s,1,0);assert(s.animation.phase==1);
    unsigned fades=0;int steps=0;
    while(!s.complete&&steps++<1000)fades+=(taxi_results_flow_step(&s,1,0)&TAXI_RESULTS_FADE_OUT)!=0;
    assert(s.complete&&fades==1&&!s.unlock_city&&s.palette_cap==0);
    assert(!taxi_results_flow_step(&s,1,0));
    /* Retained interval2 makes fade progression every second call after
       the first explicit interval1; animation must stay gated throughout. */
    assert(!taxi_results_flow_begin(&s,100,25,2,3,1,1,2));
    for(int i=0;i<64;i++) {
        taxi_results_flow_step(&s,1,0);assert(!s.animation.timer);
    }
    assert(s.fade.active_177b);taxi_results_flow_step(&s,1,0);
    assert(!s.fade.active_177b&&s.animation.phase==0);
    s.animation.phase=6;s.animation.timer=300;
    assert(taxi_results_flow_step(&s,1,0)&TAXI_RESULTS_FADE_OUT);
    assert(!s.complete&&s.palette_cap==31);
    for(int i=0;!s.complete&&i<100;i++)taxi_results_flow_step(&s,1,0);
    assert(s.complete&&s.unlock_city);
    assert(!taxi_results_flow_begin(&s,0,0,0,99,0,0,0));
    s.animation.exiting=1;s.fade.active_177b=0;
    taxi_results_flow_step(&s,1,1);assert(s.complete&&!s.unlock_city);
    puts("Original results flow: fade-in gating, palette-before-step, retained intervals, single exit fade and city unlock passed; post-results screens pending.");
}
