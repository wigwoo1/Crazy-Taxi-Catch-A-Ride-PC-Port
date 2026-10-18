#ifdef NDEBUG
#undef NDEBUG
#endif
#include "xbox.h"
#include "steering_input.h"
#include "motion.h"
#include <assert.h>
#include <stdio.h>
#include <limits.h>
static TaxiTrig trig;
static int64_t turn_heading(int16_t stick,int digital,unsigned heading){
    TaxiMotionState motion={.orientation={.speed_544=136339},
        .steering={.limit_7b0=INT_MAX},.scale_7c8=4096,.scale_e84=4096,.dimension_594=2000};
    motion.orientation.heading_1c=motion.orientation.heading_20=(int32_t)(heading*4096);
    TaxiGearState gear={0};int64_t x=0,z=0;
    for(unsigned frame=0;frame<30;frame++){
        TaxiCommand command={0};command.bytes[4]=(uint8_t)(4|digital);
        int16_t analog=taxi_xbox_steering_command(stick);
        command.bytes[12]=(uint8_t)analog;command.bytes[13]=(uint8_t)((uint16_t)analog>>8);
        taxi_steering_input(&gear,&command,100,1,0,0);
        motion.steering.angle_578=(uint16_t)gear.steering_578;
        TaxiOrientationContext context={.delta=1};TaxiOrientationEffects effects;
        assert(!taxi_motion_prepare(&motion,&context,&command,&trig,&effects));
        x+=motion.displacement.x;z+=motion.displacement.z;
    }
    TaxiPoint right=taxi_rotate_point((TaxiPoint){4096,0,0},0,(int32_t)heading,0,&trig);
    return x*right.x+z*right.z;
}
static int64_t turn(int16_t stick,int digital){return turn_heading(stick,digital,0);}
int main(int argc,char **argv){
    assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);
    unsigned char data[16388];assert(fread(data,1,sizeof data,f)==sizeof data);fclose(f);
    assert(data[0]=='C'&&data[1]=='T'&&data[2]=='R'&&data[3]=='1');
    taxi_trig_decode(&trig,data+4,data+8196);
    assert(turn(0,0)==0);
    assert(turn(-32768,0)<0&&turn(32767,0)>0);
    assert(turn(-16384,0)<0&&turn(16384,0)>0);
    assert(turn(0,1)<0&&turn(0,2)>0);
    for(unsigned heading=0;heading<4096;heading+=1024) {
        assert(turn_heading(0,0,heading)==0);
        assert(turn_heading(-32768,0,heading)<0);
        assert(turn_heading(32767,0,heading)>0);
        assert(turn_heading(-16384,0,heading)<0);
        assert(turn_heading(16384,0,heading)>0);
        assert(turn_heading(0,1,heading)<0);
        assert(turn_heading(0,2,heading)>0);
    }
    puts("PASS four cardinal headings: analog/digital steering agree in original vehicle-relative lateral direction.");
    puts("PASS native Xbox analog and digital turn direction through original trig/steering/motion; controlled fixture, no physical controller claim.");
}
