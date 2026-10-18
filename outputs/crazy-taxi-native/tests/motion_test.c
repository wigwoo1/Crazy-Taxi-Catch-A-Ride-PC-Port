#include "../src/motion.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    static TaxiTrig t;t.cosine[0]=256;t.sine[512]=181;t.cosine[512]=181;t.sine[1024]=255;
    assert(taxi_angle_difference(4095,1)==2 && taxi_angle_difference(1,4095)==-2);
    assert(taxi_angle_difference(0,2048)==-2048 && taxi_angle_difference(2048,0)==2048);
    assert(taxi_angle_difference(-1,4095)==0);
    TaxiOrientation v={.heading_1c=12345,.heading_20=999,.drift_69c=1000};
    TaxiOrientationContext c={.delta=1};TaxiOrientationEffects e;
    taxi_orientation_step(&v,&c,&t,&e);
    assert(v.heading_20==12345 && v.drift_69c==0);
    v=(TaxiOrientation){.flags_358=4,.force_540=4096};
    taxi_orientation_step(&v,&c,&t,&e);
    assert(v.heading_1c==4096 && v.heading_20==4096 && v.drift_69c==0 && v.timer_2a==2);
    v=(TaxiOrientation){.flags_358=4,.state_564=1,.timer_2a=8,.drift_69c=1024*4096,.speed_544=4096};
    c=(TaxiOrientationContext){0};
    taxi_orientation_step(&v,&c,&t,&e);
    assert(v.heading_1c==512*4096 && v.drift_69c==512*4096 && v.heading_20==512*4096);
    assert(v.position.x==63 && v.position.z==153 && v.speed_544==3809);
    v=(TaxiOrientation){.flags_358=4,.state_564=1,.timer_2a=8,.phase_e6a=2};
    c=(TaxiOrientationContext){.settings=0x80,.submode=3,.last_heading_1658=1024*4096,.counter_1650=1};
    taxi_orientation_step(&v,&c,&t,&e);
    assert(c.counter_1650==2 && c.counter_164c==1 && c.amount_1654==114 && !e.score_requested);
    v=(TaxiOrientation){.flags_358=4,.state_564=1,.timer_2a=8,.phase_e6a=2,.speed_544=70000};
    c=(TaxiOrientationContext){.flag_1694=1,.last_heading_1658=1024*4096};
    taxi_orientation_step(&v,&c,&t,&e);
    assert(e.score_requested && e.score_a==1 && e.score_b==0 && e.score_c==50 && c.amount_1654==0);
    TaxiMotionState m={.orientation={.speed_544=4096},.scale_7c8=4096,.scale_e84=4096};
    TaxiCommand command={0};c=(TaxiOrientationContext){.delta=1};
    assert(taxi_motion_prepare(&m,&c,&command,&t,&e)==0);
    assert(m.displacement.x==0 && m.displacement.y==0 && m.displacement.z==4);
    m.orientation.speed_544=-4096;c.delta=2;
    assert(taxi_motion_prepare(&m,&c,&command,&t,&e)==0 && m.displacement.z==-9);
    m.orientation.speed_544=4096;m.orientation.angle_18=1024;c.delta=1;
    assert(taxi_motion_prepare(&m,&c,&command,&t,&e)==0 && m.displacement.y==-4 && m.displacement.z==0);
    TaxiMotionState saved=m;c.delta=0;
    assert(taxi_motion_prepare(&m,&c,&command,&t,&e)==-1 && memcmp(&m,&saved,sizeof m)==0);
    /* Nonzero turn branch with a controlled nonzero steering sine. */
    m=(TaxiMotionState){.orientation={.speed_544=4096,.flags_358=4},
        .steering={.angle_578=1024},.scale_7c8=4096,.scale_e84=4096,.dimension_594=2000};
    TaxiSteeringState s=m.steering;s.flags_358=4;s.speed_544=4096;int32_t response;
    assert(taxi_steering_response(&s,&command,0,&t,&response)==0);
    t.sine[(uint32_t)response&4095]=128;c=(TaxiOrientationContext){.delta=1};
    assert(taxi_motion_prepare(&m,&c,&command,&t,&e)==0);
    assert(m.response_57c==response && m.orientation.heading_1c!=0);
    puts("PASS: native heading/drift adjustment, pivot correction, movement generation and invalid-step rollback.");
}
