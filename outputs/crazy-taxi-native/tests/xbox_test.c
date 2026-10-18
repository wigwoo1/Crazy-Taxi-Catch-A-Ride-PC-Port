#ifdef NDEBUG
#undef NDEBUG
#endif
#include "xbox.h"
#include "steering_input.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    const unsigned native[10]={0x1000,0x2000,0x20,0x10,8,4,1,2,0x200,0x100};
    for(unsigned buttons=0;buttons<65536;buttons++){
        TaxiXboxSample s=taxi_xbox_decode((uint16_t)buttons,0,0,255,127);unsigned held=0;
        for(unsigned k=0;k<10;k++)if(buttons&native[k])held|=1u<<k;
        assert(s.look_back==!!(buttons&0x80));
        assert(s.connected&&s.active_low_keys==(0x3ffu^held)&&s.accelerator==255&&s.brake==127&&!s.steering);
    }
    for(unsigned command=0;command<256;command++){assert(taxi_xbox_camera_command((uint8_t)command,0)==(command&254));assert(taxi_xbox_camera_command((uint8_t)command,1)==(command|1));}
    assert(taxi_xbox_decode(0,7849,0,0,0).active_low_keys==0x3ff);
    assert(taxi_xbox_decode(0,7850,0,0,0).active_low_keys==(0x3ff^16));
    assert(taxi_xbox_decode(0,-7850,0,0,0).active_low_keys==(0x3ff^32));
    assert(taxi_xbox_decode(0,0,7850,0,0).active_low_keys==(0x3ff^64));
    assert(taxi_xbox_decode(0,0,-7850,0,0).active_low_keys==(0x3ff^128));
    assert(!taxi_xbox_poll(4).connected&&taxi_xbox_poll(4).active_low_keys==0x3ff);
    assert(taxi_xbox_steering_command(-32768)==192&&taxi_xbox_steering_command(32767)==-192);
    assert(taxi_xbox_steering_command(-7850)==8&&taxi_xbox_steering_command(7850)==-8);
    int previous=192;
    for(int lx=-32768;lx<=32767;lx++){
        int value=taxi_xbox_steering_command((int16_t)lx);
        assert(value<=previous&&value>=-192&&value<=192);previous=value;
        if(lx>=-7849&&lx<=7849)assert(!value);
        else assert(lx<0?value>=8:value<=-8);
        if(value){
            TaxiGearState gear={0};TaxiCommand command={0};
            command.bytes[4]=12;
            command.bytes[12]=(uint8_t)value;command.bytes[13]=(uint8_t)((uint16_t)value>>8);
            taxi_steering_input(&gear,&command,100,1,0,0);
            assert(gear.steering_578==value*8);
            assert((command.bytes[4]&12)==12);
        }
    }
    puts("Native Xbox mapping: all65536 button combinations, trigger preservation, stick dead zones and invalid slot PASS; no physical input claim.");
    return 0;
}
