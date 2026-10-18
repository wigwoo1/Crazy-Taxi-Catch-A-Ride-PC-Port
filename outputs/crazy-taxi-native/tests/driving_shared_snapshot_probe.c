#include "../src/driving_shared_snapshot.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){assert(argc==3);uint8_t e[0x40000],i[0x8000];FILE *f=fopen(argv[1],"rb");assert(f);assert(fread(e,1,sizeof(e),f)==sizeof(e));fclose(f);f=fopen(argv[2],"rb");assert(f);assert(fread(i,1,sizeof(i),f)==sizeof(i));fclose(f);TaxiDrivingSharedSnapshot s;memset(&s,0xa5,sizeof(s));TaxiDrivingSharedSnapshot before=s;assert(taxi_driving_shared_snapshot_read(&s,e,0x7602,i+0x53c0,48)==-1);assert(!memcmp(&s,&before,sizeof(s)));assert(taxi_driving_shared_snapshot_read(&s,e,sizeof(e),i+0x53c0,48)==0);
#define P(x) printf("%lld ",(long long)(x))
P(s.delta);P(s.frame_delta);P(s.settings);P(s.flag_1697);P(s.flag_153c);P(s.freeze_177b);P(s.sound_enabled);P(s.context.gear.state_2630);P(s.context.gear.state_1654);P(s.context.gear.state_25d4);P(s.context.power.state_25d2);P(s.context.power.state_25dc);P(s.context.freeze_2620);P(s.context.motion.orientation.submode);P(s.context.motion.orientation.flag_1ea8);P(s.context.motion.orientation.flag_1694);P(s.context.motion.orientation.counter_164c);P(s.context.motion.orientation.counter_1650);P(s.context.motion.orientation.last_heading_1658);P(s.context.motion.traction.flag_1696);P(s.context.motion.traction.flag_262c);for(unsigned n=0;n<16;n++)P(s.command.bytes[n]);puts("");return 0;}
