#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/loading_flow.h"
#include "../src/passenger_init.h"
typedef struct {TaxiLoadingFlow flow;unsigned records,presentations;TaxiPassengerState *states;} Work;
static void record(void *context,unsigned pass,size_t index) {
    Work *w=context;
    assert(pass==w->records/128&&index==w->records%128);
    /* The source helper precedes processing this record on both passes. */
    assert(w->states[index].bytes[1]==0xa4);
    w->records++;
    if(taxi_loading_flow_record(&w->flow,(unsigned)index)) {
        w->presentations++;taxi_loading_flow_present(&w->flow,1);
    }
}
int main(void) {
    TaxiPassengerResolved resolved[128]={0};TaxiPassengerState states[128],reference[128];
    TaxiPassengerInitEffect effects[128],reference_effects[128];
    memset(states,0xa4,sizeof states);memcpy(reference,states,sizeof states);
    Work w={.states=states};taxi_loading_flow_begin(&w.flow,0);
    for(unsigned n=0;n<33;n++)taxi_loading_flow_present(&w.flow,1);
    assert(!taxi_passengers_init_observed(states,resolved,128,effects,record,&w));
    assert(w.records==256&&w.presentations==52&&w.flow.animation_bank==2);
    assert(!taxi_passengers_init_normal(reference,resolved,128,reference_effects));
    assert(!memcmp(states,reference,sizeof states)&&!memcmp(effects,reference_effects,sizeof effects));
    assert(taxi_loading_flow_finish(&w.flow));
    puts("Loading passenger work: two128-record passes,52 pre-record presentations, retained bank2 and identical native initialization PASS");
}
