#include "../src/impact.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
int main(void) {
    static TaxiTrig camera_trig;
    camera_trig.cosine[0]=256;camera_trig.sine[1024]=256;
    TaxiPoint camera_point=taxi_rotate_camera((TaxiPoint){1,2,3},1024,1024,0,&camera_trig);
    TaxiPoint body_point=taxi_rotate_point((TaxiPoint){1,2,3},1024,1024,0,&camera_trig);
    assert(camera_point.x==-3&&camera_point.y==1&&camera_point.z==-2);
    assert(body_point.x==2&&body_point.y==3&&body_point.z==1);
    camera_point=taxi_rotate_camera((TaxiPoint){-123,456,-789},0,0,0,&camera_trig);
    assert(camera_point.x==-123&&camera_point.y==456&&camera_point.z==-789);
    static uint8_t sine[8192]={0},cosine[8192]={0};static TaxiTrig t;
    cosine[1]=16; /* Original identity cosine: raw 4096, decoded 256. */
    sine[2048]=255;sine[2049]=15; /* Original quarter-turn sine is 255, not 256. */
    sine[6145]=240; /* Signed raw -4096. */
    taxi_trig_decode(&t,sine,cosine);
    assert(t.cosine[0]==256 && t.sine[1024]==255 && t.sine[3072]==-256);
    TaxiImpactState v={.speed_544=45520};TaxiImpactEffects e;
    taxi_vehicle_impact(&v,(TaxiPoint){0,0,256},0,0,&t,&e);
    assert(e.projected_x==0 && e.projected_z==-8192);
    assert(v.speed_544==5690 && v.state_73c==-40 && v.state_748==0);
    assert(v.state_750==524288 && v.state_74c==0 && v.state_34==-0x9c71c);
    assert(e.effect_slot==4 && e.sound_table_index==43);
    v=(TaxiImpactState){.speed_544=45520};
    taxi_vehicle_impact(&v,(TaxiPoint){256,0,0},0,0,&t,&e);
    assert(e.projected_x==-8192 && e.projected_z==0);
    assert(v.speed_544==34140 && v.state_748==-80 && e.effect_slot==0 && e.sound_table_index==41);
    v=(TaxiImpactState){.speed_544=-45520};
    taxi_vehicle_impact(&v,(TaxiPoint){0,0,256},0,0,&t,&e);
    assert(v.speed_544==-5690 && v.state_73c==-40);
    v=(TaxiImpactState){.speed_544=45520};
    taxi_vehicle_impact(&v,(TaxiPoint){0,0,256},0x80,0,&t,&e);
    assert(v.speed_544==5690 && v.state_73c==-16 && v.state_750==262144);
    v=(TaxiImpactState){.speed_544=45520};
    taxi_vehicle_impact(&v,(TaxiPoint){0,0,256},0x80,1,&t,&e);
    assert(v.state_73c==-40 && v.state_750==524288);
    v=(TaxiImpactState){.angle_1c=1024*4096,.speed_544=45520};
    taxi_vehicle_impact(&v,(TaxiPoint){0,0,256},0,0,&t,&e);
    assert(e.projected_x==-8160 && e.projected_z==0 && v.speed_544==34140);
    v=(TaxiImpactState){.angle_1c=-1,.speed_544=3};
    taxi_vehicle_impact(&v,(TaxiPoint){0,0,-256},0,0,&t,&e);
    assert(e.projected_x==0 && e.projected_z==8192 && v.speed_544==1);
    assert(e.effect_slot==5 && v.state_34==0x9c71c);
    v=(TaxiImpactState){.speed_544=INT_MAX};
    taxi_vehicle_impact(&v,(TaxiPoint){256,0,0},0,0,&t,&e);
    assert(v.speed_544==INT_MAX && v.state_748==-128); /* Wrapped product is -1024. */
    puts("PASS: native collision projections, impulses, speed loss and effect classification.");
}
