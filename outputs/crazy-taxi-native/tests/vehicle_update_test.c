#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/vehicle_update.h"
int main(void) {
    static TaxiTrig t;t.cosine[0]=256;t.sine[1024]=255;
    static TaxiAngleTable table;
    TaxiCommand command={0};
    TaxiVehicleUpdate v={.motion={.orientation={.speed_544=4096,.flags_28=8},.scale_7c8=4096,.scale_e84=4096},.capacity_570=12800,.capacity_574=6400};
    TaxiVehicleUpdateContext c={.orientation={.delta=1}};TaxiVehicleUpdateEffects e={0};
    assert(!taxi_vehicle_update(&v,&c,&command,&t,&table,&e));
    /* Displacement uses speed before the traction tail's damping. */
    assert(v.motion.displacement.z==4&&v.motion.orientation.speed_544==4075);
    assert(v.motion.orientation.state_564==1&&!e.traction.score_requested);
    v.force_53c=-20000;c.traction.flag_262c=0;
    assert(!taxi_vehicle_update(&v,&c,&command,&t,&table,&e));
    assert(!v.motion.orientation.state_564&&e.traction.score_requested&&e.traction.score_c==25);
    v.force_53c=0;v.motion.orientation.force_540=4096;v.response_744=0;
    assert(!taxi_vehicle_update(&v,&c,&command,&t,&table,&e));
    assert(v.motion.orientation.force_540==3584&&v.response_744==-29);
    /* A wrapped lookup fails after motion has already changed temporary state. */
    v.force_53c=-0x01000000;v.motion.orientation.force_540=0x00a00000;
    TaxiVehicleUpdate before=v;TaxiVehicleUpdateContext prior=c;TaxiVehicleUpdateEffects old=e;
    assert(taxi_vehicle_update(&v,&c,&command,&t,&table,&e)==-1);
    assert(!memcmp(&v,&before,sizeof v)&&!memcmp(&c,&prior,sizeof c)&&!memcmp(&e,&old,sizeof e));
    puts("PASS: ordered native motion/traction/response update and tail-error rollback.");return 0;
}

