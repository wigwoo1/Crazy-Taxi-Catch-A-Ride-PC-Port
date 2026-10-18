#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "../src/runtime_asset.h"
#include "../src/world_asset.h"
#include "../src/view_coordinates.h"
#include "../src/taxi_sprite.h"
/* Flat driving isolates steering direction from map walls. Project native
   displacement through exactly the same coordinate conversion as the view. */
static void check_view_controls(const TaxiRuntimeAsset *runtime,TaxiWorldAsset *world) {
    /* Compare camera interpolation coordinates to recovered fixed-point
       matrix rotation, allowing its 8-bit trig/matrix rounding. This catches
       handedness and pitch-sign errors without requiring identical rounding. */
    double largest_camera_error=0;
    for(int heading=0;heading<4096;heading+=64) for(int pitch=-512;pitch<=512;pitch+=128) {
        TaxiPoint original=taxi_rotate_point((TaxiPoint){0,468,-3072},pitch,heading,0,&runtime->trig);
        double camera[3];
        taxi_view_camera_offset(taxi_view_yaw(heading*4096,0),-pitch*360.0/4096,468,3072,camera);
        double expected[3]={taxi_view_x(original.x),original.y,original.z};
        for(unsigned axis=0;axis<3;axis++) {
            double error=fabs(camera[axis]-expected[axis]);
            if(error>largest_camera_error) largest_camera_error=error;
            assert(error<64);
        }
    }
    printf("PASS: camera coordinate signs match recovered matrix across 576 heading/pitch cases (max rounding difference %.2f raw units).\n",largest_camera_error);
    double largest_projection_error=0;
    for(int heading=0;heading<4096;heading+=64) for(int pitch=-512;pitch<=512;pitch+=128) {
        TaxiPoint point={733,467,2011};
        TaxiPoint original=taxi_rotate_point(point,-pitch,-heading,0,&runtime->trig);
        double y=taxi_view_yaw(heading*4096,0)*3.141592653589793/180;
        double p=pitch*3.141592653589793/2048;
        /* Renderer GL Y(y)*X(p), after its source-X reflection. */
        double x=taxi_view_x(point.x),vy=cos(p)*point.y-sin(p)*point.z;
        double vz=sin(p)*point.y+cos(p)*point.z;
        double actual[3]={cos(y)*x+sin(y)*vz,vy,-sin(y)*x+cos(y)*vz};
        double expected[3]={original.x,original.y,-original.z};
        for(unsigned axis=0;axis<3;axis++) {
            double error=fabs(actual[axis]-expected[axis]);
            if(error>largest_projection_error) largest_projection_error=error;
            assert(error<64);
        }
    }
    printf("PASS: source projection rotation order/sign across 576 mixed poses (max Q8 difference %.2f raw units).\n",largest_projection_error);
    for(unsigned config=0;config<2;config++) for(unsigned heading=0;heading<4096;heading+=1024) {
        TaxiSpawnMarker marker=runtime->starts[0];marker.heading=(uint16_t)heading;
        TaxiDriveWorld car;TaxiDriveWorldContext context;
        assert(taxi_simulation_start(&car,&context,runtime->templates[config],584,&marker,0,
            taxi_world_asset_ground,world)==1);
        assert(car.initial_camera_pitch_650==0&&car.initial_camera_mode_64c==2);
        assert(car.initial_camera_heading_654==(int32_t)(((heading+2048)&4095)*4096));
        TaxiDriveWorld special;TaxiDriveWorldContext special_context;
        assert(taxi_simulation_start(&special,&special_context,runtime->templates[config],584,&marker,128,
            taxi_world_asset_ground,world)==1);
        assert(special.initial_camera_heading_654==(int32_t)(heading*4096)&&special.initial_camera_mode_64c==2);
        TaxiGearInput input={.delta=1,.frame_delta=1,.action2=1};
        TaxiDriverEffects effects;
        for(unsigned tick=0;tick<120;tick++) {
            TaxiCommand command={0};command.bytes[4]=4;
            assert(!taxi_driver_step(&car.driver,&context.driver,&command,&input,0,0,
                &runtime->trig,&runtime->traction,&effects));
        }
        double yaw=taxi_view_yaw(heading*4096,0),angle=yaw*3.141592653589793/180;
        /* These verify camera/selector arithmetic, not anatomical front/rear
           identification of the low-resolution source artwork. */
        assert(taxi_sprite_frame(taxi_view_sprite_angle(yaw),heading*4096)==0);
        assert(taxi_sprite_frame(taxi_view_sprite_angle(yaw+180),heading*4096)==32);
        assert(taxi_sprite_frame(taxi_view_sprite_angle(yaw+90),heading*4096)==48);
        assert(taxi_sprite_frame(taxi_view_sprite_angle(yaw-90),heading*4096)==16);
        for(unsigned side=1;side<=2;side++) {
            TaxiDriver driver=car.driver;TaxiDriverContext c=context.driver;
            double x=0,z=0;
            for(unsigned tick=0;tick<30;tick++) {
                TaxiCommand command={0};command.bytes[4]=(uint8_t)(4|side);
                assert(!taxi_driver_step(&driver,&c,&command,&input,0,0,&runtime->trig,&runtime->traction,&effects));
                x+=driver.motion.motion.displacement.x;z+=driver.motion.motion.displacement.z;
            }
            double screen_right=cos(angle)*taxi_view_x(x)+sin(angle)*z;
            assert(side==1?screen_right<-1:screen_right>1);
        }
    }
    puts("PASS: left/right driving projection and four camera-to-sprite angle mappings; visual front/rear identity is not asserted.");
}
/* Standalone native simulation check. Takes an assets directory, never a ROM. */
int main(int argc,char **argv) {
    if(argc!=2) return 2;
    char path[4096];static TaxiRuntimeAsset runtime;
    assert(snprintf(path,sizeof path,"%s/driving.ctr",argv[1])<(int)sizeof path);
    if(taxi_runtime_asset_load(&runtime,path)) return 1;
    const char *names[]={"city1","city2","startup"};
    /* These are current native-source regression outputs, not ROM trajectory
       oracles. The older October 5 executable predates later motion/world
       source revisions; its historical coordinates are in the baseline report. */
    const TaxiPoint expected[3][2]={{{30640,19200,-27140},{34165,19200,-27108}},
        {{-88134,53760,-87159},{-85479,53420,-91021}},{{660,27324,0},{31100,11770,0}}};
    for(unsigned scene=0;scene<3;scene++) {
        TaxiWorldAsset world={0};assert(snprintf(path,sizeof path,"%s/%s.ctw",argv[1],names[scene])<(int)sizeof path);
        if(taxi_world_asset_load(&world,path)) return 1;
        if(scene==0) check_view_controls(&runtime,&world);
        for(unsigned config=0;config<2;config++) {
            TaxiDriveWorld car;TaxiDriveWorldContext context;TaxiDriveWorldEffects effects;
            assert(taxi_simulation_start(&car,&context,runtime.templates[config],584,&runtime.starts[scene],0,
                taxi_world_asset_ground,&world)==1);
            for(unsigned tick=0;tick<600;tick++) {
                TaxiCommand command={0};command.bytes[4]=(uint8_t)(tick<400?4:8);
                TaxiGearInput input={.delta=1,.frame_delta=1,.action2=(uint8_t)(tick<400),.action3=(uint8_t)(tick>=400)};
                assert(!taxi_drive_world_step(&car,&context,&command,&input,0,&runtime.trig,&runtime.traction,&runtime.normals,
                    taxi_world_asset_obstacle,taxi_world_asset_ground,&world,&effects));
            }
            TaxiPoint p=car.world.height.position;
            if(p.x!=expected[scene][config].x||p.y!=expected[scene][config].y||p.z!=expected[scene][config].z)
                fprintf(stderr,"Mismatch %s config%u: got %d,%d,%d expected %d,%d,%d\n",names[scene],config,p.x,p.y,p.z,expected[scene][config].x,expected[scene][config].y,expected[scene][config].z);
            assert(p.x==expected[scene][config].x&&p.y==expected[scene][config].y&&p.z==expected[scene][config].z);
            assert(!car.world.speed_544);
            printf("PASS current-source native diagnostic: %s config%u at %d,%d,%d, stopped; not ROM trajectory parity\n",names[scene],config,p.x,p.y,p.z);
        }
        taxi_world_asset_free(&world);
    }
    return 0;
}

