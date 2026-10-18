#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/acceleration.h"
static TaxiTrig trig;
static TaxiAcceleration fixture(void) {
    TaxiAcceleration v={.gears={.scale_5b8=60,.scale_5cc=60,.boost_75c=-1},
        .efficiency_4e4=4096,.mass_5a4=4096,.radius_5bc=4096,.radius_5d0=4096};return v;
}
int main(void) {
    TaxiAcceleration v=fixture();TaxiGearEffects e={0};
    assert(!taxi_acceleration_integrate(&v,60000,4096,1,1,&trig,&e));
    assert(v.acceleration_6a0==60000&&v.gears.speed_544==1000&&v.gears.rpm_538==1000);
    assert(v.response_53c==-191&&v.maximum_550==1000);
    v=fixture();assert(!taxi_acceleration_integrate(&v,60000,-4096,1,1,&trig,&e));
    assert(v.gears.speed_544==-1000&&v.gears.rpm_538==1000&&v.state_6aa==128&&v.response_53c==191);
    v=fixture();v.gears.timer_e6c=50;v.timer_e74=50;
    assert(!taxi_acceleration_integrate(&v,600,4096,60,1,&trig,&e)&&v.gears.speed_544==2400);
    v=fixture();v.timer_e74=50;
    assert(!taxi_acceleration_integrate(&v,600,4096,60,1,&trig,&e)&&v.gears.speed_544==600);
    v=fixture();v.gears.flags_4ac=32;v.gears.timer_e70=50;
    assert(!taxi_acceleration_integrate(&v,600,-4096,60,1,&trig,&e)&&v.gears.speed_544==-900);
    v=fixture();v.boost_558=60;v.gears.flags_4ac=2;
    assert(!taxi_acceleration_integrate(&v,600,4096,60,1,&trig,&e)&&v.gears.speed_544==600);
    v=fixture();v.boost_558=60;
    assert(!taxi_acceleration_integrate(&v,600,4096,60,1,&trig,&e)&&v.gears.speed_544==660);
    v=fixture();v.gears.shift_timer=1;v.gears.rpm_538=123;v.gears.scale_5cc=0;
    assert(!taxi_acceleration_integrate(&v,600,4096,60,0,&trig,&e)&&v.gears.rpm_538==123);
    v=fixture();v.response_53c=320;v.acceleration_6a0=123;v.gears.speed_544=500;
    assert(!taxi_acceleration_integrate(&v,600,0,0,0,&trig,&e));
    assert(v.response_53c==310&&v.acceleration_6a0==123&&v.gears.speed_544==500);
    v=fixture();v.gears.speed_544=1138;v.drag_528=4096;v.drag_590=4096;
    assert(!taxi_acceleration_integrate(&v,600,4096,60,0,&trig,&e)&&v.acceleration_6a0==600);
    /* 100 speed units: wrapped fixed-point drag is 476. */
    v=fixture();v.gears.speed_544=113800;v.drag_528=4096;v.drag_590=4096;
    assert(!taxi_acceleration_integrate(&v,600,4096,60,0,&trig,&e)&&v.acceleration_6a0==124);
    v=fixture();v.rolling_52c=4096;
    assert(!taxi_acceleration_integrate(&v,600,4096,60,0,&trig,&e)&&v.acceleration_6a0==-47615);
    const int deltas[]={0,61};
    for(unsigned i=0;i<2;i++) {
        v=fixture();TaxiAcceleration old=v;TaxiGearEffects prior=e;
        assert(taxi_acceleration_integrate(&v,600,4096,deltas[i],0,&trig,&e)==-1);
        assert(!memcmp(&v,&old,sizeof v)&&!memcmp(&e,&prior,sizeof e));
    }
    v=fixture();v.gears.scale_5cc=0;TaxiAcceleration old=v;
    assert(taxi_acceleration_integrate(&v,600,4096,1,0,&trig,&e)==-1&&!memcmp(&v,&old,sizeof v));
    TaxiAccelerationInput in={.action2=1,.delta=1,.frame_delta=1};
    TaxiAccelerationGlobals globals={0};
    v=fixture();v.gears.gear=2;v.gears.ratios[1]=4096;v.gears.final_ratio=4096;
    v.gears.max_532=8500;v.gears.rpm_538=500;
    for(unsigned i=0;i<16;i++) v.curve[i]=600;
    assert(!taxi_acceleration_update(&v,&in,&globals,&trig,&e));
    assert(v.gears.speed_544==10&&v.scale_4b0==4096&&v.gears.rpm_538==10);
    v.gears.flags_4ac=32;v.gears.speed_544=0;
    assert(!taxi_acceleration_update(&v,&in,&globals,&trig,&e)&&v.gears.speed_544==-10);
    v.gears.flags_4ac=0;v.gears.speed_544=0;v.gears.timer_e78=50;v.gears.timer_e80=50;
    assert(!taxi_acceleration_update(&v,&in,&globals,&trig,&e));
    assert(v.gears.speed_544==40&&v.scale_e84==0x14cc&&globals.state_25dc==8192&&globals.state_25d2==1);
    in.action2=0;v.gears.timer_e80=0;v.gears.speed_544=0;
    assert(!taxi_acceleration_update(&v,&in,&globals,&trig,&e));
    assert(v.gears.speed_544==0&&!v.gears.timer_e6c&&!v.gears.timer_e70);
    in.settings=128;in.flag_1697=1;old=v;in.delta=0;
    assert(!taxi_acceleration_update(&v,&in,&globals,&trig,&e)&&!memcmp(&v,&old,sizeof v));
    in=(TaxiAccelerationInput){.action2=1,.delta=1,.frame_delta=1};
    v.gears.max_532=20000;v.gears.rpm_538=16000;
    old=v;TaxiAccelerationGlobals prior=globals;
    assert(taxi_acceleration_update(&v,&in,&globals,&trig,&e)==-1);
    assert(!memcmp(&v,&old,sizeof v)&&!memcmp(&globals,&prior,sizeof globals));
    puts("PASS: native force/speed integration, reverse, boosts, resistance and invalid-step rollback.");return 0;
}
