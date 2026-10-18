/* Diagnostic city orchestration. Endpoint Y and spawn Y are explicit inputs;
   neither is claimed as the original initializer's stack-height policy. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "../src/world_asset.h"
#include "../src/destination.h"
#include "../src/passenger_init.h"
#include "../src/passenger_search.h"
#include "../src/passenger_boarding.h"
typedef struct {TaxiRoutePoint *points;size_t count;} Route;
static Route routes[1024];
static TaxiPassengerResolved resolved[128];
static TaxiPassengerState initial[128],current[128];
static TaxiPassengerInitEffect init_effects[128];
static TaxiRoutePoint spawns[128],targets[256];
static uint8_t destinations[256][48],indices[128];
int main(int argc,char **argv) {
    TaxiWorldAsset world={0};unsigned count,nroutes,ndest;int32_t endpoint_y;
    if(argc!=2||taxi_world_asset_load(&world,argv[1])) return 1;
    if(scanf("%u %u %u %" SCNd32,&count,&nroutes,&ndest,&endpoint_y)!=4||count>128||nroutes>1024||ndest>256) return 2;
    for(unsigned i=0;i<ndest;i++) {
        unsigned slots[4];
        if(scanf("%" SCNd32 " %" SCNd32 " %u %u %u %u",&targets[i].x,&targets[i].z,&slots[0],&slots[1],&slots[2],&slots[3])!=6) return 3;
        for(unsigned j=0;j<4;j++) {if(slots[j]>65535) return 3;destinations[i][40+j*2]=(uint8_t)slots[j];destinations[i][41+j*2]=(uint8_t)(slots[j]>>8);}
    }
    for(unsigned i=0;i<count;i++) {
        unsigned kind,dest;int32_t y;
        if(scanf("%u %u %" SCNd32 " %" SCNd32 " %" SCNd32,&kind,&dest,&spawns[i].x,&y,&spawns[i].z)!=5||kind>255||dest>255||(kind<=9&&dest>=ndest)) return 4;
        indices[i]=(uint8_t)dest;resolved[i].kind=(uint8_t)kind;resolved[i].origin=spawns[i];resolved[i].spawn_height=y;
        if(kind<=9) resolved[i].destination=targets[dest];
    }
    for(unsigned i=0;i<nroutes;i++) {
        unsigned n;if(scanf("%u",&n)!=1||!n||n>4096) return 5;
        routes[i].points=calloc(n,sizeof(TaxiRoutePoint));routes[i].count=n;
        if(!routes[i].points) return 5;
        for(unsigned j=0;j<n;j++) if(scanf("%" SCNd32 " %" SCNd32,&routes[i].points[j].x,&routes[i].points[j].z)!=2) return 5;
    }
    unsigned eligible=0,trials=0,completed=0;
    for(unsigned i=0;i<count;i++) {
        if(resolved[i].kind>9) continue;
        for(unsigned j=0;j<nroutes;j++) {
            TaxiRoutePoint *p=routes[j].points;
            if(p[0].x!=spawns[i].x||p[0].z!=spawns[i].z) continue;
            TaxiRoutePoint end=p[routes[j].count-1];TaxiGroundHit hit={0};
            int status=taxi_world_asset_ground(&world,(TaxiPoint){end.x,endpoint_y,end.z},&hit);
            if(status<0) return 6;
            if(status&&taxi_destination_accepts(indices[i],destinations[indices[i]],hit.material)) {
                resolved[i].route=p;resolved[i].point_count=routes[j].count;eligible++;break;
            }
        }
    }
    if(taxi_passengers_init_normal(initial,resolved,count,init_effects)) return 7;
    for(unsigned i=0;i<count;i++) {
        if(!resolved[i].route||resolved[i].kind>=8) continue;
        /* Fixed diagnostic taxi position near this original customer. */
        if(spawns[i].x>INT32_MAX-700) return 8;
        TaxiRoutePoint taxi={spawns[i].x+700,spawns[i].z};
        memcpy(current,initial,sizeof initial);
        int32_t candidate,distance;
        if(taxi_passenger_search(current,spawns,count,taxi,&candidate,&distance)||candidate<0) return 8;
        TaxiPickupState pickup={.selected_index=-1,.dialogue_index=-1,.destination_index=-1};
        TaxiPickupInput input={.candidate_index=candidate,.taxi_position=taxi};TaxiPickupEffects effect;
        if(taxi_passenger_pickup(&pickup,current,indices,count,&input,&effect)||!effect.began) return 9;
        unsigned ticks=0;
        for(;ticks<200;ticks++) {
            if(taxi_passenger_boarding(current+candidate,1,&pickup,taxi,1,0)) return 10;
            if(taxi_passenger_pickup(&pickup,current,indices,count,&input,&effect)) return 11;
            if(effect.completed) break;
        }
        trials++;
        if(effect.completed&&pickup.destination_index==indices[candidate]) completed++;
        else return 12;
        printf("TRIP %u %" PRId32 " %u %" PRId32 "\n",i,candidate,ticks+1,pickup.destination_index);
    }
    printf("SUMMARY %u %u %u\n",eligible,trials,completed);
    for(unsigned i=0;i<nroutes;i++) free(routes[i].points);
    taxi_world_asset_free(&world);return 0;
}
