#include <stdio.h>
#include <inttypes.h>
#include "../src/passenger_route.h"
int main(void) {
    unsigned count;TaxiRoutePoint points[4096];int32_t minimum,maximum;TaxiRoutePoint destination;
    while(scanf("%u %" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNd32,
                &count,&minimum,&maximum,&destination.x,&destination.z)==5) {
        if(!count||count>4096) return 1;
        for(unsigned i=0;i<count;i++) if(scanf("%" SCNd32 " %" SCNd32,&points[i].x,&points[i].z)!=2) return 2;
        int32_t length;if(taxi_passenger_route_length(points,count,&length)) return 3;
        TaxiRouteCategories c=taxi_passenger_route_categories(length,minimum,maximum,points[0],destination);
        printf("%" PRId32 " %u %u\n",length,c.length_category,c.detour_category);
    }
    return ferror(stdin)?4:0;
}
