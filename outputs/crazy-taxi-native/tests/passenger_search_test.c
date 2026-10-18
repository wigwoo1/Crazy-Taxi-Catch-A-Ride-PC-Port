#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/passenger_search.h"
#include "../src/passenger_pickup.h"
int main(void) {
    TaxiPassengerState c[3]={0};TaxiRoutePoint spawn[3]={{0,0},{0,0},{100,0}};
    c[0].bytes[0]=c[1].bytes[0]=c[2].bytes[0]=1;c[2].bytes[1]=8;
    int32_t index,distance;
    assert(!taxi_passenger_search(c,spawn,3,(TaxiRoutePoint){1024,0},&index,&distance));
    assert(index==0&&distance==1048576&&c[0].bytes[0]==1); /* strict thresholds, first tie */
    assert(!taxi_passenger_search(c,spawn,3,(TaxiRoutePoint){1023,0},&index,&distance));
    assert(c[0].bytes[0]==(1|4|8));
    assert(!taxi_passenger_search(c,spawn,3,(TaxiRoutePoint){0,0},&index,&distance));
    assert(c[0].bytes[0]==(1|4|8)); /* bit8 persists after moving inward */
    TaxiPickupState state={.destination_index=-1,.selected_index=-1,.dialogue_index=-1};
    TaxiPickupInput input={.candidate_index=index};TaxiPickupEffects effects;uint8_t dest[3]={2,3,4};
    assert(!taxi_passenger_pickup(&state,c,dest,3,&input,&effects)&&effects.began&&effects.customer_index==0);
    /* Do not fake boarding completion; pending waits for original animation. */
    assert(!taxi_passenger_pickup(&state,c,dest,3,&input,&effects)&&!effects.completed);
    assert(!taxi_passenger_search(c,spawn,3,(TaxiRoutePoint){1025,0},&index,&distance));
    assert(c[0].bytes[0]==(1|8|16));
    assert(!taxi_passenger_search(c,spawn,3,(TaxiRoutePoint){2048,0},&index,&distance));
    assert(c[0].bytes[0]==(1|8));
    assert(!taxi_passenger_search(c,spawn,3,(TaxiRoutePoint){12800,0},&index,&distance));
    assert(index==-1&&distance==INT32_MAX);
    c[0].bytes[0]=65;c[1].bytes[0]=0;
    assert(!taxi_passenger_search(c,spawn,3,(TaxiRoutePoint){0,0},&index,&distance)&&index==-1);
    c[0].bytes[0]=1;c[0].bytes[3]=5;TaxiPassengerState old[3];memcpy(old,c,sizeof c);
    assert(taxi_passenger_search(c,spawn,3,(TaxiRoutePoint){0,0},&index,&distance)==-1&&!memcmp(old,c,sizeof c));
    puts("PASS: original customer search boundaries, filters, sticky flags, ties and pickup integration.");return 0;
}
