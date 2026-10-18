#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "../src/passenger_route.h"
int main(void) {
    TaxiRoutePoint p[]={{0,0},{768,1024},{768,1024}};int32_t length=99;
    assert(!taxi_passenger_route_length(p,3,&length)&&length==1280);
    p[1]=(TaxiRoutePoint){255,0};assert(!taxi_passenger_route_length(p,2,&length)&&length==0);
    p[1].x=-1;assert(!taxi_passenger_route_length(p,2,&length)&&length==256);
    assert(!taxi_passenger_route_length(p,1,&length)&&length==0);
    assert(taxi_passenger_route_length(p,0,&length)==-1&&length==0);
    assert(taxi_passenger_route_length(NULL,1,&length)==-1&&length==0);
    for(int ratio=0;ratio<=100;ratio++) {
        TaxiRouteCategories c=taxi_passenger_route_categories(100,100,600,(TaxiRoutePoint){0},(TaxiRoutePoint){ratio,0});
        assert(c.length_category==0&&c.detour_category==(ratio<20?4:ratio<40?3:ratio<60?2:ratio<80?1:0));
    }
    TaxiRouteCategories c=taxi_passenger_route_categories(0,0,0,(TaxiRoutePoint){0},(TaxiRoutePoint){100,0});
    assert(c.length_category==0&&c.detour_category==4);
    c=taxi_passenger_route_categories(600,100,600,(TaxiRoutePoint){0},(TaxiRoutePoint){0});
    assert(c.length_category==4);
    puts("PASS: recovered passenger route shifts, lengths, category thresholds and zero division.");
    return 0;
}
