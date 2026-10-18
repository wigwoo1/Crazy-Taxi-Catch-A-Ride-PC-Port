#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/free_rev.h"
static TaxiAcceleration initial(void) {
    TaxiAcceleration v={.scale_4b0=4096,.coasting_4e0=4096,.radius_5bc=4096,.radius_5d0=4096,.mass_5a4=4096,.response_53c=99,
        .gears={.ratios={4096},.final_ratio=4096,.max_532=8500,.scale_5b8=60,.scale_5cc=60,.speed_544=123,.boost_75c=-1}};
    for(unsigned i=0;i<16;i++) v.curve[i]=100;return v;
}
int main(void) {
    TaxiAcceleration v=initial();TaxiGearEffects e={0};
    assert(!taxi_free_rev(&v,1,4,&e));
    assert(v.gears.speed_568==10&&v.gears.rpm_538==10&&v.gears.speed_544==123&&!v.response_53c);
    v=initial();v.gears.speed_568=1000;
    assert(!taxi_free_rev(&v,1,0,&e)&&v.gears.speed_568==891&&v.gears.rpm_538==891);
    v=initial();v.gears.flags_4ac=4;v.radius_5d0=0;v.gears.scale_5cc=0;
    assert(!taxi_free_rev(&v,1,4,&e)&&v.gears.speed_568==10);
    v=initial();v.gears.rpm_538=8501;
    assert(!taxi_free_rev(&v,1,4,&e)&&(v.gears.flags_4ac&2)&&v.gears.speed_568==0);
    v=initial();v.gears.shift_timer=1;v.gears.speed_568=1000;v.gears.rpm_538=2000;
    assert(!taxi_free_rev(&v,1,0,&e)&&v.gears.speed_568==1000&&v.gears.rpm_538==2000);
    v=initial();v.gears.boost_75c=1000;
    assert(!taxi_free_rev(&v,1,4,&e)&&v.gears.rpm_538==500&&v.gears.boost_75c==950);
    v=initial();v.mass_5a4=0;TaxiAcceleration old=v;TaxiGearEffects olde=e;
    assert(taxi_free_rev(&v,1,4,&e)==-1&&!memcmp(&v,&old,sizeof v)&&!memcmp(&e,&olde,sizeof e));
    v=initial();old=v;assert(taxi_free_rev(&v,0,4,&e)==-1&&!memcmp(&v,&old,sizeof v));
    v=initial();v.gears.final_ratio=0;v.mass_5a4=0;
    assert(!taxi_free_rev(&v,0,4,&e)&&!v.response_53c);
    puts("PASS: free-rev throttle/coast, wheel RPM, road-speed preservation, gates, boost and rollback.");return 0;
}
