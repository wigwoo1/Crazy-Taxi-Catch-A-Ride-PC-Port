#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "../src/loading_flow.h"
int main(void) {
    TaxiLoadingFlow s;taxi_loading_flow_begin(&s,4);
    assert(!taxi_loading_flow_finish(&s)&&!taxi_loading_flow_record(&s,0));
    for(unsigned n=0;n<33;n++) {
        assert(s.phase==TAXI_LOADING_IN&&s.animation_bank==4);
        taxi_loading_flow_present(&s,1);
        assert(s.palette_cap==(n<32?(int)n:31));
    }
    assert(s.phase==TAXI_LOADING_WORK&&!s.fade.active_177b);
    for(unsigned n=0;n<1000;n++)taxi_loading_flow_present(&s,1);
    assert(s.phase==TAXI_LOADING_WORK&&s.animation_bank==0);
    for(unsigned n=0;n<100;n++) {
        assert(taxi_loading_flow_record(&s,n)==(n%5==0));
        assert(s.animation_bank==((n/5+1)%5));
        taxi_loading_flow_present(&s,1);
    }
    assert(taxi_loading_flow_finish(&s)&&!taxi_loading_flow_finish(&s));
    assert(!taxi_loading_flow_record(&s,100));
    for(unsigned n=0;n<33;n++) {
        assert(s.phase==TAXI_LOADING_OUT);taxi_loading_flow_present(&s,1);
        assert(s.palette_cap==(n<32?31-(int)n:0));
    }
    assert(s.phase==TAXI_LOADING_READY);
    TaxiLoadingFlow saved=s;
    for(unsigned n=0;n<100;n++)taxi_loading_flow_present(&s,1);
    assert(!memcmp(&s,&saved,sizeof s));
    taxi_loading_flow_begin(&s,0);assert(s.phase==TAXI_LOADING_IN&&s.animation_bank==0);
    puts("Loading flow: original fade order, explicit work cadence, modulo5 banks, no timed work completion and terminal freeze PASS");
}
