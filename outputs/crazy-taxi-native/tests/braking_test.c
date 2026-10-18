#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/braking.h"
static TaxiAcceleration fixture(void) {
    TaxiAcceleration v={.gears={.speed_544=65536,.gear=2,.ratios={4096,4096},.reverse_ratio=4096,
        .final_ratio=4096,.scale_5b8=4096,.scale_5cc=4096,.boost_75c=-1},
        .mass_5a4=4096,.brake_608=10000,.brake_60c=10000};return v;
}
int main(void) {
    TaxiAcceleration v=fixture();TaxiBrakeInput in={.delta=1,.action3=1,.sound_enabled=1};
    TaxiBrakeEffects e={0};uint8_t freeze=0;
    assert(!taxi_brake_update(&v,&in,&freeze,&e));assert(v.gears.speed_544==65280&&v.state_6a9==128&&!e.sound_requested);
    v=fixture();in.delta=60;assert(!taxi_brake_update(&v,&in,&freeze,&e));assert(v.gears.speed_544==62976);
    v=fixture();v.gears.speed_544=-65536;v.gears.flags_4ac=32;
    assert(!taxi_brake_update(&v,&in,&freeze,&e)&&v.gears.speed_544==-62976);
    v=fixture();v.gears.speed_544=256;in.delta=1;
    v.gears.timer_e6c=v.gears.timer_e70=v.gears.timer_e78=v.gears.timer_e7c=v.gears.timer_e80=v.timer_e74=1;
    assert(!taxi_brake_update(&v,&in,&freeze,&e));assert(!v.gears.speed_544&&e.sound_requested&&e.sound_index==24);
    assert(!v.gears.timer_e6c&&!v.gears.timer_e70&&!v.gears.timer_e78&&!v.gears.timer_e7c&&!v.gears.timer_e80&&!v.timer_e74);
    v=fixture();in.action3=0;TaxiAcceleration old=v;
    assert(!taxi_brake_update(&v,&in,&freeze,&e)&&!memcmp(&v,&old,sizeof v));
    v.gears.flags_4ac=32;assert(!taxi_brake_update(&v,&in,&freeze,&e)&&v.gears.speed_544<65536&&!v.state_6a9);
    v=fixture();in.flag_1697=1;in.settings=128;in.flag_153c=1;old=v;
    assert(!taxi_brake_update(&v,&in,&freeze,&e)&&!freeze&&!memcmp(&v,&old,sizeof v));
    in.flag_153c=0;assert(!taxi_brake_update(&v,&in,&freeze,&e)&&freeze&&v.gears.speed_544<65536);
    v=fixture();v.mass_5a4=0;old=v;TaxiBrakeEffects prior=e;uint8_t priorfreeze=freeze;
    assert(taxi_brake_update(&v,&in,&freeze,&e)==-1);
    assert(!memcmp(&v,&old,sizeof v)&&!memcmp(&e,&prior,sizeof e)&&freeze==priorfreeze);
    puts("PASS: native braking energy, signed speed, direction mismatch, stop sound and rollback.");return 0;
}
