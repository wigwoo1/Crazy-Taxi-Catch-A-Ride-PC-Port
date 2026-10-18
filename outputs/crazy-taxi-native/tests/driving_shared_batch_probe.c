#include "../src/driving_shared_snapshot.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(void){_setmode(_fileno(stdin),_O_BINARY);uint8_t e[0x40000]={0},i[48];
while(fread(e+0x1534,1,32,stdin)==32){
const unsigned offsets[]={0x164c,0x1770,0x1ea0,0x1eb0,0x25d0,0x7600,0x3b00};const unsigned sizes[]={192,32,16,32,100,4,4};
for(unsigned n=0;n<7;n++)assert(fread(e+offsets[n],1,sizes[n],stdin)==sizes[n]);assert(fread(i,1,48,stdin)==48);
TaxiDrivingSharedSnapshot s;memset(&s,0xa5,sizeof(s));TaxiDrivingSharedSnapshot before=s;assert(taxi_driving_shared_snapshot_read(&s,e,0x7602,i,48)==-1);assert(!memcmp(&s,&before,sizeof(s)));assert(taxi_driving_shared_snapshot_read(&s,e,sizeof(e),i,48)==0);
#define P(x) printf("%lld ",(long long)(x))
P(s.delta);P(s.frame_delta);P(s.settings);P(s.flag_1697);P(s.flag_153c);P(s.freeze_177b);P(s.sound_enabled);P(s.context.gear.state_2630);P(s.context.gear.state_1654);P(s.context.gear.state_25d4);P(s.context.power.state_25d2);P(s.context.power.state_25dc);P(s.context.freeze_2620);P(s.context.motion.orientation.submode);P(s.context.motion.orientation.flag_1ea8);P(s.context.motion.orientation.flag_1694);P(s.context.motion.orientation.counter_164c);P(s.context.motion.orientation.counter_1650);P(s.context.motion.orientation.last_heading_1658);P(s.context.motion.traction.flag_1696);P(s.context.motion.traction.flag_262c);for(unsigned n=0;n<16;n++)P(s.command.bytes[n]);puts("");}return 0;}
