#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/passenger_init.h"
static int32_t read_word(const uint8_t *p) {
    uint32_t u=(uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
    int32_t s;memcpy(&s,&u,4);return s;
}
int main(void) {
    TaxiRoutePoint short_route[]={{0,0},{2560,0}},long_route[]={{0,0},{5120,0}},ignored[]={{0,0},{512000,0}};
    TaxiPassengerResolved r[4]={
        {.kind=2,.route=short_route,.point_count=2,.spawn_height=-4096,.destination={2560,0}},
        {.kind=7,.route=long_route,.point_count=2,.spawn_height=1024,.destination={5120,0}},
        {.kind=4}, {.kind=12,.route=ignored,.point_count=2}};
    TaxiPassengerState state[4],before[4];TaxiPassengerInitEffect effects[4];
    memset(state,0xa4,sizeof state);memset(effects,0,sizeof effects);
    assert(!taxi_passengers_init_normal(state,r,4,effects));
    assert(state[0].bytes[0]==1&&state[0].bytes[1]==2&&state[0].bytes[2]==0&&state[0].bytes[3]==0);
    assert(state[1].bytes[2]==4&&state[1].bytes[1]==7);
    assert(read_word(state[0].bytes+4)==2560&&read_word(state[0].bytes+20)==-4096);
    assert(state[0].bytes[11]==0xa4&&state[0].bytes[28]==0xa4&&state[0].bytes[43]==0xa4);
    assert(state[0].bytes[12]==0&&state[0].bytes[15]==0&&state[0].bytes[35]==6);
    assert(state[2].bytes[0]==0xa6&&state[2].bytes[1]==255&&state[2].bytes[4]==0xa4);
    assert(state[3].bytes[0]==0xa4&&state[3].bytes[1]==12&&effects[3].spawn_other_actor);
    assert(!effects[0].spawn_other_actor&&!effects[2].spawn_other_actor);
    memcpy(before,state,sizeof state);TaxiPassengerInitEffect old[4];memcpy(old,effects,sizeof old);
    r[1].point_count=0;
    assert(taxi_passengers_init_normal(state,r,4,effects)==-1);
    assert(!memcmp(before,state,sizeof state)&&!memcmp(old,effects,sizeof old));
    r[1].route=NULL;r[0].route=NULL;
    assert(!taxi_passengers_init_normal(state,r,4,effects)&&state[0].bytes[1]==255&&state[1].bytes[1]==255);
    assert(!taxi_passengers_init_normal(NULL,NULL,0,NULL));
    assert(taxi_passengers_init_normal(state,r,129,effects)==-1);
    puts("PASS: ordinary customer initialization, eligible-only ranges, preserved fields, disabled records and rollback.");
    return 0;
}
