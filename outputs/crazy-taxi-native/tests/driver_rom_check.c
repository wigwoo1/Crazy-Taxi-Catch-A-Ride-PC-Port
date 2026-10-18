#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/driver.h"
#include "../src/approach.h"
#include "../src/drive_world.h"
#include "../src/vehicle_config.h"
#include "rom_vehicle_fixture.h"
static int32_t add(int32_t a,int32_t b) {uint32_t u=(uint32_t)a+(uint32_t)b;int32_t s;memcpy(&s,&u,4);return s;}
static int no_obstacle(void *ctx,TaxiPoint p,TaxiObstacleHit *h) {(void)ctx;(void)p;(void)h;return 0;}
static int flat_ground(void *ctx,TaxiPoint p,TaxiGroundHit *h) {
    (void)p;if(*(int *)ctx) return -1;
    *h=(TaxiGroundHit){.height=0,.normal={0,4096,0},.polygon=0};return 1;
}
int main(int argc,char **argv) {
    if(argc!=2) return 2;
    setvbuf(stdout,NULL,_IONBF,0);
    FILE *f=fopen(argv[1],"rb");assert(f);static TaxiTrig trig;static TaxiAngleTable table;
    uint8_t s[8192],c[8192],a[514],b[0x248],n[8193*2];static TaxiNormalAngles normals;
    assert(!fseek(f,0x7ec9ec,SEEK_SET)&&fread(n,1,sizeof n,f)==sizeof n);
    assert(!taxi_normal_angles_decode(&normals,n,sizeof n));
    assert(!fseek(f,0x4a0d08,SEEK_SET)&&fread(s,1,sizeof s,f)==sizeof s);
    assert(fread(c,1,sizeof c,f)==sizeof c);taxi_trig_decode(&trig,s,c);
    assert(!fseek(f,0x7f09ee,SEEK_SET)&&fread(a,1,sizeof a,f)==sizeof a);taxi_angle_table_decode(&table,a);
    const long offsets[]={0x4a554c,0x4a5794};
    for(unsigned config=0;config<2;config++) for(unsigned direction=0;direction<2;direction++) {
        assert(!fseek(f,offsets[config],SEEK_SET)&&fread(b,1,sizeof b,f)==sizeof b);
        TaxiVehicleConfig setup;assert(!taxi_vehicle_config_decode(&setup,b,sizeof b));
        TaxiDriver v={0};v.power=load(b);v.steering_rate_57a=half(b,0x57a);v.rpm_53a=half(b,0x53a);
        v.motion.motion.orientation.flags_358=4;v.motion.motion.orientation.state_564=1;
        v.motion.motion.dimension_594=setup.dimension_594;
        v.motion.capacity_570=setup.capacity_570;v.motion.capacity_574=setup.capacity_574;
        /* Diagnostic unit multiplier and flat origin, not full scene setup. */
        v.motion.motion.scale_7c8=4096;
        TaxiDriveWorld combined={.driver=v};
        combined.world.height.body.denominator_754=57344;combined.world.height.body.denominator_758=28672;
        combined.world.height.scale_e84=4096;
        TaxiDriveWorldContext combined_context={0};TaxiDriveWorldEffects combined_effects;
        int fail_ground=0;
        for(unsigned step=0;step<960;step++) {
            int throttle=step<600,brake=step>=720;
            TaxiCommand command={0};command.bytes[4]=(uint8_t)(throttle?4:brake?8:0);
            if(step>=120&&step<180) command.bytes[4]|=(uint8_t)(direction?2:1);
            TaxiGearInput input={.delta=1,.frame_delta=1,.action2=(uint8_t)throttle,.action3=(uint8_t)brake};
            assert(!taxi_drive_world_step(&combined,&combined_context,&command,&input,0,&trig,&table,&normals,
                no_obstacle,flat_ground,&fail_ground,&combined_effects));
            assert(combined_effects.world.ground_hit&&combined.world.height.position.y==0);
            assert(combined.driver.power.gears.speed_544==combined.world.speed_544);
            assert(combined.driver.motion.motion.orientation.position.x==combined.world.height.position.x);
        }
        printf("Combined flat-world template %08lx %s: position %d,%d speed %d\n",offsets[config]+0x08000000,
            direction?"right":"left",combined.world.height.position.x,combined.world.height.position.z,combined.world.speed_544);
        assert(combined.world.height.position.x!=0);
        fail_ground=1;TaxiDriveWorld oldcombined=combined;TaxiDriveWorldContext oldcontext=combined_context;
        TaxiDriveWorldEffects oldeffects=combined_effects;TaxiCommand command={0},oldcommand=command;
        TaxiGearInput input={.delta=1,.frame_delta=1};
        assert(taxi_drive_world_step(&combined,&combined_context,&command,&input,0,&trig,&table,&normals,
            no_obstacle,flat_ground,&fail_ground,&combined_effects)==-1);
        assert(!memcmp(&combined,&oldcombined,sizeof combined)&&!memcmp(&combined_context,&oldcontext,sizeof combined_context));
        assert(!memcmp(&command,&oldcommand,sizeof command)&&!memcmp(&combined_effects,&oldeffects,sizeof combined_effects));
        TaxiDriverContext context={0};TaxiDriverEffects effects;unsigned turns=0,slips=0,tick;
        for(tick=0;tick<1920;tick++) {
            int throttle=tick<600,brake=tick>=720;
            TaxiCommand cmd={0};cmd.bytes[4]=(uint8_t)(throttle?4:brake?8:0);
            if(tick>=120&&tick<180) cmd.bytes[4]|=(uint8_t)(direction?2:1);
            int16_t prior_rpm_53a=v.rpm_53a;
            TaxiGearInput in={.delta=1,.frame_delta=1,.action2=(uint8_t)throttle,.action3=(uint8_t)brake};
            if(taxi_driver_step(&v,&context,&cmd,&in,0,0,&trig,&table,&effects)) {
                fprintf(stderr,"Driver failure template %u direction %u tick %u speed %d steering %d\n",config,direction,tick,v.power.gears.speed_544,v.power.gears.steering_578);return 1;
            }
            int16_t expected_rpm_53a=v.power.gears.gear?(int16_t)taxi_approach_s32(prior_rpm_53a,v.power.gears.rpm_538,256):v.power.gears.rpm_538;assert(v.rpm_53a==expected_rpm_53a);
            TaxiOrientation *o=&v.motion.motion.orientation;TaxiPoint d=v.motion.motion.displacement;
            o->position.x=add(o->position.x,d.x);o->position.z=add(o->position.z,d.z);
            if(v.power.gears.steering_578) turns++;
            if(!o->state_564) slips++;
            if(brake&&!v.power.gears.speed_544) break;
        }
        TaxiOrientation *o=&v.motion.motion.orientation;
        printf("Template %08lx %s: position %d,%d heading %d, turn steps %u, slip steps %u, stopped at %u\n",
            offsets[config]+0x08000000,direction?"right":"left",o->position.x,o->position.z,o->heading_1c,turns,slips,tick);
        assert(turns>0&&o->position.x!=0&&tick<1920&&!v.power.gears.speed_544);
        TaxiDriver before=v;TaxiDriverContext prior=context;TaxiCommand cmd={0};TaxiGearInput bad={.delta=0,.frame_delta=1};
        assert(taxi_driver_step(&v,&context,&cmd,&bad,0,0,&trig,&table,&effects)==-1);
        assert(!memcmp(&v,&before,sizeof v)&&!memcmp(&context,&prior,sizeof context));
    }
    fclose(f);puts("PASS: connected native steering, powertrain and motion probes; flat diagnostic setup, no scene collision.");return 0;
}
