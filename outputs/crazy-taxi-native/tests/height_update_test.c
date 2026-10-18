#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/height_update.h"
static TaxiNormalAngles angles;
static TaxiHeightState initial(void) {
    TaxiHeightState v={.position={40000,100,0},.state_698=99,.scale_e84=4096,.flags_28=16};
    v.body.denominator_754=57344;v.body.denominator_758=28672;return v;
}
int main(void) {
    TaxiWorldContact c={.height=100,.normal={0,4096,0}};
    TaxiHeightInput in={.delta=1};TaxiHeightGlobals g={0};TaxiHeightEffects e={0};
    TaxiHeightState v=initial();v.displacement_y=32;
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e));
    assert(v.position.y==100&&v.state_698==0&&v.flags_28==16&&!e.took_off);
    v.displacement_y=33;
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e));
    assert(e.took_off&&v.position.y==133&&v.flags_28==24&&v.body.velocity_730==-7);
    assert(g.takeoff_1544.y==100&&g.takeoff_1544.x==40000);
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e));
    assert(!e.landed&&!e.took_off&&v.position.y==166&&v.body.response_704==-7&&g.distance_1540==33);
    v.position.y=132;v.body.response_704=140;v.body.response_70c=140;
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e));
    assert(e.landed&&v.position.y==100&&v.flags_28==16&&g.distance_1540==0);
    assert(v.body.position_734==110&&v.body.position_740==110);
    const int gaps[]={512,513,1024,1025,1536,1537,2048,2049};
    const int args[]={0,25,25,50,50,75,75,0};
    for(unsigned i=0;i<8;i++) {
        v=initial();v.flags_28|=8;v.position.y=100+gaps[i];
        assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e));
        assert(e.airborne_call==(i!=0)&&e.airborne_arg2==args[i]&&e.airborne_arg1==(i==7));
    }
    in.settings=0x80;v=initial();v.position.x=0x8a00;v.displacement_y=33;
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e));
    assert(!e.took_off&&v.position.y==100&&v.flags_28==16);
    v.position.x++;assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e));
    assert(e.took_off&&v.body.velocity_730==-3);
    v=initial();v.displacement_y=33;v.scale_e84=4095;
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e)&&v.body.velocity_730==-2);
    in.submode=5;v=initial();v.displacement_y=33;
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e)&&v.body.velocity_730==-10);
    in.delta=4000;v=initial();v.displacement_y=128001;
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e)&&v.body.velocity_730==512);
    in=(TaxiHeightInput){.delta=1,.settings=0x80,.completion_result=9};
    v=initial();v.flags_28|=8;g=(TaxiHeightGlobals){.takeoff_1544={0,100,0}};
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e));
    assert(e.landed&&e.completion_call&&g.distance_1540==40000&&g.flag_153c==1&&g.flag_153d==9&&g.flag_153e==1&&g.flag_1697==1);
    v=initial();v.flags_28|=8;g=(TaxiHeightGlobals){.takeoff_1544={1600,100,0},.flag_153c=1,.flag_153d=7};
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e));
    assert(g.distance_1540==38400&&!e.completion_call&&!g.flag_153c&&g.flag_153d==7&&g.flag_1697==1);
    v=initial();v.flags_28|=8;v.position=(TaxiPoint){65536,200,0};g=(TaxiHeightGlobals){.takeoff_1544={0,200,0}};
    assert(!taxi_height_update(&v,&g,&c,&in,&angles,&e)&&g.distance_1540==0);
    v=initial();v.body.denominator_758=0;TaxiHeightState old=v;TaxiHeightGlobals oldg=g;TaxiHeightEffects olde=e;
    assert(taxi_height_update(&v,&g,&c,&in,&angles,&e)==-1);
    assert(!memcmp(&v,&old,sizeof v)&&!memcmp(&g,&oldg,sizeof g)&&!memcmp(&e,&olde,sizeof e));
    puts("PASS: native road/takeoff/flight/landing branches, height boundaries, challenge gates, overflow and rollback.");return 0;
}
