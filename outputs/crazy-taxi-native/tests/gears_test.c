#include "../src/gears.h"
#include "../src/actions.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
static TaxiGearState initial(void) {
    return (TaxiGearState){.max_gear=3,.ratios={4096,2048,1024},.reverse_ratio=4096,
        .final_ratio=4096,.gear=1,.next_gear=1,.min_530=500,.max_532=4000,
        .up_534=3000,.down_536=1000,.rpm_538=2000,.speed_544=2000,.speed_568=2000,
        .scale_5b8=60,.scale_5cc=60,.boost_75c=-1};
}
int main(void) {
    TaxiGearState v=initial();TaxiGearInput in={.delta=1,.frame_delta=1};
    TaxiGearGlobals g={0};TaxiGearEffects e;
    v.flags_4ac=32;v.speed_544=30000;in.action4=1;in.sound_enabled=1;
    assert(taxi_gear_update(&v,&in,&g,&e)==0);
    assert(!(v.flags_4ac&32) && v.timer_e78==100 && v.state_e69==1 && v.state_e68==0);
    assert(e.count==2 && e.indices[0]==25 && e.indices[1]==43);
    v=initial();in=(TaxiGearInput){.action5=1,.delta=1,.frame_delta=1};
    assert(taxi_gear_update(&v,&in,&g,&e)==0);
    assert((v.flags_4ac&32) && v.timer_e7c==100 && v.timer_e78==0 && v.timer_e6c==0);
    v=initial();in=(TaxiGearInput){.delta=1,.frame_delta=1};v.rpm_538=3100;
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && v.shift_timer==5 && v.gear==1 && v.next_gear==2);
    for(int k=0;k<5;k++) assert(taxi_gear_update(&v,&in,&g,&e)==0);
    assert(v.shift_timer==0 && v.gear==2 && v.rpm_538==1000);
    v=initial();v.gear=2;v.rpm_538=900;
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && v.next_gear==1 && v.shift_timer==5);
    v=initial();v.rpm_538=3100;v.speed_544=-2000;
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && v.shift_timer==0);
    v=initial();v.gear=0;v.rpm_538=3100;
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && v.gear==1 && v.next_gear==1 && v.shift_timer==5);
    v=initial();v.flags_4ac=1;v.shift_timer=5;v.next_gear=2;in.command_flags=16;
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && v.shift_timer==5 && v.gear==1);
    in.command_flags=0;
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && v.shift_timer==0 && v.gear==2);
    v=initial();v.phase_e6a=2;v.phase_e6b=119;
    in=(TaxiGearInput){.action2=1,.delta=1,.frame_delta=1};in.held[8]=1;
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && v.phase_e6b==120);
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && v.phase_e6a==0 && v.phase_e6b==0);
    v=initial();v.phase_e6a=2;v.phase_e6b=119;v.speed_544=40000;in.frame_delta=200;
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && v.phase_e6b==63); /* Original uint8 wrap. */
    v=initial();v.shift_timer=1;v.next_gear=2;v.boost_75c=5000;
    in=(TaxiGearInput){.delta=1,.frame_delta=1};
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && v.rpm_538==2500 && v.boost_75c==4950);
    v=initial();v.shift_timer=1;v.next_gear=8;TaxiGearState before=v;TaxiGearGlobals oldg=g;
    assert(taxi_gear_update(&v,&in,&g,&e)==-1 && memcmp(&v,&before,sizeof v)==0 && memcmp(&g,&oldg,sizeof g)==0);
    v=initial();v.shift_timer=1;v.next_gear=2;v.scale_5cc=0;before=v;
    assert(taxi_gear_update(&v,&in,&g,&e)==-1 && memcmp(&v,&before,sizeof v)==0);
    v=initial();v.rpm_538=3100;before=v;in.freeze_2620=1;
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && memcmp(&v,&before,sizeof v)==0);
    /* Profile 0 shoulder edge uses the same pre-transition flags as the ROM. */
    TaxiInput buttons={0};buttons.pressed[8]=1;v=initial();v.flags_4ac=32;
    TaxiActionContext context={.profile=0,.vehicle_flags=v.flags_4ac};
    in=(TaxiGearInput){.action4=(uint8_t)taxi_action_query(&buttons,context,4),
        .action5=(uint8_t)taxi_action_query(&buttons,context,5),.delta=1};
    assert(in.action4 && !in.action5);
    assert(taxi_gear_update(&v,&in,&g,&e)==0 && !(v.flags_4ac&32));
    puts("PASS: native gear transitions, ratio timers, RPM helpers and action resolver integration.");
}
