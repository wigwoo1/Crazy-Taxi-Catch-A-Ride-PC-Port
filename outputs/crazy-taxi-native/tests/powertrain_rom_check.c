#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/braking.h"
#include "../src/vehicle_config.h"
#include "rom_vehicle_fixture.h"
int main(int argc,char **argv) {
    setvbuf(stdout,NULL,_IONBF,0);
    if(argc!=2) {fprintf(stderr,"Usage: powertrain-rom-check ROM\n");return 2;}
    FILE *f=fopen(argv[1],"rb");assert(f);static TaxiTrig t;uint8_t sine[8192],cosine[8192],b[0x248];
    assert(!fseek(f,0x4a0d08,SEEK_SET)&&fread(sine,1,sizeof sine,f)==sizeof sine);
    assert(fread(cosine,1,sizeof cosine,f)==sizeof cosine);taxi_trig_decode(&t,sine,cosine);
    const long offsets[]={0x4a554c,0x4a5794};
    for(unsigned scene=0;scene<2;scene++) {
        assert(!fseek(f,offsets[scene],SEEK_SET)&&fread(b,1,sizeof b,f)==sizeof b);
        for(unsigned reverse=0;reverse<2;reverse++) {
            TaxiAcceleration v=load(b);if(reverse) v.gears.flags_4ac|=32;
            TaxiGearGlobals gg={0};TaxiAccelerationGlobals ag={0};uint8_t freeze=0;
            int32_t peak=0,coaststart=0,brakestart=0;int highestgear=1;
            for(unsigned tick=0;tick<1920;tick++) {
                int throttle=tick<600,brake=tick>=720;
                if(tick==600) coaststart=v.gears.speed_544;
                if(tick==720) brakestart=v.gears.speed_544;
                TaxiGearInput gi={.delta=1,.frame_delta=1,.action2=(uint8_t)throttle,.action3=(uint8_t)brake,
                    .command_flags=(uint8_t)(throttle?4:brake?8:0),.freeze_2620=freeze};
                TaxiGearEffects ge;
                assert(!taxi_gear_update(&v.gears,&gi,&gg,&ge));
                ag.state_2630=gg.state_2630;ag.state_1654=gg.state_1654;
                TaxiAccelerationInput ai={.delta=1,.frame_delta=1,.action2=(uint8_t)throttle};
                assert(!taxi_acceleration_update(&v,&ai,&ag,&t,&ge));
                gg.state_2630=ag.state_2630;gg.state_1654=ag.state_1654;
                TaxiBrakeInput bi={.delta=1,.action2=(uint8_t)throttle,.action3=(uint8_t)brake};TaxiBrakeEffects be;
                assert(!taxi_brake_update(&v,&bi,&freeze,&be));assert(!taxi_gear_clamp(&v.gears));
                int32_t magnitude=v.gears.speed_544<0?-v.gears.speed_544:v.gears.speed_544;
                if(magnitude>peak) peak=magnitude;if(v.gears.gear>highestgear) highestgear=v.gears.gear;
                if(brake&&!v.gears.speed_544) break;
            }
            printf("Template %08lx %s: peak %d, coast-start %d, brake-start %d, final %d, highest gear %d\n",
                offsets[scene]+0x08000000,reverse?"reverse":"forward",peak,coaststart,brakestart,v.gears.speed_544,highestgear);
            assert(peak>0);assert(v.gears.speed_544==0);
        }
    }
    fclose(f);puts("PASS: connected native powertrain probes with both original templates. Timing/setup are diagnostic.");return 0;
}
