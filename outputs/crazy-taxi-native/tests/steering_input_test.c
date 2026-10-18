#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "../src/steering_input.h"
int main(void) {
    TaxiGearState v={.phase_e6a=1,.phase_e6b=100};TaxiCommand c={0};c.bytes[4]=1;
    taxi_steering_input(&v,&c,100,2,0,1);assert(v.steering_578==200&&v.phase_e6a==2&&!v.phase_e6b);
    for(int i=0;i<30;i++) taxi_steering_input(&v,&c,100,1,0,0);
    assert(v.steering_578==1536);c.bytes[4]=2;
    for(int i=0;i<40;i++) taxi_steering_input(&v,&c,100,1,0,0);
    assert(v.steering_578==-1536);c.bytes[4]=0;
    for(int i=0;i<16;i++) taxi_steering_input(&v,&c,100,1,0,0);
    assert(v.steering_578==0);c.bytes[4]=3;
    taxi_steering_input(&v,&c,100,1,0,0);assert(v.steering_578==100);
    taxi_steering_input(&v,&c,100,1,1,0);assert(v.steering_578==100);
    c.bytes[12]=8;taxi_steering_input(&v,&c,100,1,1,0);assert(v.steering_578==64&&(c.bytes[4]&3)==2);
    c.bytes[12]=0xf8;c.bytes[13]=0xff;
    taxi_steering_input(&v,&c,100,1,1,0);assert(v.steering_578==-64&&(c.bytes[4]&3)==1);
    c.bytes[12]=7;c.bytes[13]=0;taxi_steering_input(&v,&c,100,1,1,0);assert(v.steering_578==-64);
    c.bytes[12]=0;c.bytes[13]=0x20;
    taxi_steering_input(&v,&c,100,1,1,0);assert(v.steering_578==0);
    puts("PASS: native digital steering ramp, phase transition, centring and analog halfword behavior.");return 0;
}
