#include "../src/driving.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
static TaxiSteeringState initial(void) {
    return (TaxiSteeringState){.angle_578=1024,.flags_358=4};
}
int main(void) {
    TaxiInput input={0};TaxiCommand command;
    memset(&command,255,sizeof command);
    taxi_command_pack(&command,&input);
    assert(command.bytes[0]==255 && command.bytes[3]==255);
    assert(command.bytes[4]==0 && command.bytes[5]==252);
    for(unsigned k=6;k<16;k++) assert(command.bytes[k]==0);
    static const uint8_t expected[10]={4,8,0,0,130,65,0,0,16,32};
    for(unsigned k=0;k<10;k++) {
        input=(TaxiInput){0};input.held[k]=1;
        taxi_command_pack(&command,&input);assert(command.bytes[4]==expected[k]);
    }
    static TaxiTrig trig;trig.sine[1024]=256;trig.sine[3072]=-256;
    TaxiSteeringState v=initial();int32_t result;
    command.bytes[4]=0;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0);
    assert(result==602 && v.flags_28==32);
    v=initial();command.bytes[4]=4;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0);
    assert(result==417 && v.flags_28==64);
    v=initial();command.bytes[4]=12;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0);
    assert(result==602 && v.flags_28==32); /* Brake takes precedence. */
    v=initial();v.phase_e6a=2;command.bytes[4]=4;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0 && result==3682);
    v=initial();v.phase_e6a=2;
    assert(taxi_steering_response(&v,&command,0x80,&trig,&result)==0 && result==2454);
    v=initial();v.angle_578=3072;command.bytes[4]=0;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0 && result==-602);
    v=initial();v.flags_28=8;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0 && result==150);
    v=initial();v.flags_358=0;v.limit_7b0=100;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0);
    assert(result==100 && v.state_7b8==0);
    v=initial();v.flags_358=0;v.limit_7b0=10000;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0 && v.state_7b8==1);
    v=initial();v.state_564=1;command.bytes[4]=4;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0 && result==463 && v.flags_28==0);
    v=initial();v.speed_544=426749;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0);
    v=initial();v.speed_544=426750;result=123;
    TaxiSteeringState saved=v;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==-1);
    assert(memcmp(&v,&saved,sizeof v)==0 && result==123);
    v.speed_544=INT_MIN;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==-1);
    /* Connect the actual input transition, command packer and steering query. */
    input=(TaxiInput){0};
    taxi_input_update(&input,(TaxiInputSample){.active_low_keys=1022,.elapsed_ticks=1});
    taxi_command_pack(&command,&input);v=initial();
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0 && result==417);
    int32_t previous=INT_MAX;
    command.bytes[4]=0;
    for(int32_t scaled=0;scaled<375;scaled++) {
        v=initial();v.speed_544=scaled*1138;
        assert(taxi_steering_response(&v,&command,0,&trig,&result)==0);
        assert(result<=previous && result>=0);previous=result;
        int32_t forward=result;v=initial();v.speed_544=-scaled*1138;
        assert(taxi_steering_response(&v,&command,0,&trig,&result)==0 && result==forward);
    }
    v=initial();v.phase_e6a=2;v.phase_e6b=120;command.bytes[4]=4;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0 && result==920);
    /* Actual failed/passing route turn entry speeds. Independently calculated
       from hash-verified084a59f4 curve and129e0 integer interpolation;
       identity test sine256, ordinary phase0/ground flags4/full throttle. */
    v=initial();v.speed_544=136339;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0 && result==46);
    v=initial();v.speed_544=184398;
    assert(taxi_steering_response(&v,&command,0,&trig,&result)==0 && result==22);
    puts("PASS: native command packing, steering curve, state gates and input integration.");
}
