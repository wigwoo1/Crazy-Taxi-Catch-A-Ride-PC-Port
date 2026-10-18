#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/vehicle_reset.h"
int main(void) {
    TaxiVehicleReset v;memset(&v,0x55,sizeof v);
    v.orientation=(TaxiOrientation){.position={123,-456,789},.heading_1c=123456,
        .heading_20=-1,.speed_544=10000,.force_540=-500,.state_564=1,
        .flags_358=255,.flags_28=255,.drift_69c=400,.timer_2a=10,.phase_e6a=2};
    TaxiOrientation before=v.orientation;int32_t global=99;
    taxi_vehicle_reset(&v,0,&global);
    assert(global==0&&v.state_654==2047*4096);
    before.drift_69c=0;before.timer_2a=0;before.flags_28=31;before.phase_e6a=0;
    assert(!memcmp(&before,&v.orientation,sizeof before));
    assert(v.state_e60==65535&&v.state_64c==2);
    assert(v.state_611==128&&v.state_612==128&&v.state_613==128);
    assert(v.state_614==3000&&v.state_620==3000);
    assert(v.state_754==57344&&v.state_758==28672&&v.state_75c==-1);
    /* After removing the explicit constants, every remaining reset field is zero. */
    v.orientation=(TaxiOrientation){0};v.state_654=0;v.state_e60=v.state_64c=0;
    v.state_611=v.state_612=v.state_613=0;v.state_614=v.state_620=0;
    v.state_754=v.state_758=v.state_75c=0;
    TaxiVehicleReset zero={0};assert(!memcmp(&v,&zero,sizeof v));
    const int32_t headings[]={0,1,-1,4096,INT32_MIN,INT32_MAX};
    for(unsigned i=0;i<sizeof headings/sizeof headings[0];i++) {
        v.orientation.heading_20=headings[i];taxi_vehicle_reset(&v,128,&global);
        assert(v.state_654==headings[i]);
        taxi_vehicle_reset(&v,0,&global);
        assert(v.state_654>=0&&v.state_654<4096*4096&&(v.state_654&4095)==0);
    }
    puts("PASS: native vehicle reset, preserved motion fields and heading-mode boundaries.");return 0;
}
