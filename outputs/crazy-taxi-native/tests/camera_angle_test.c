#include "../src/camera_angle.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(void) {
    assert(taxi_camera_yaw_offset_step(0,100,1)==32);
    assert(taxi_camera_yaw_offset_step(96,100,1)==100);
    assert(taxi_camera_yaw_offset_step(100,0,1)==68);
    assert(taxi_camera_yaw_offset_step(4,0,1)==0);
    assert(taxi_camera_yaw_offset_step(-40,-100,2)==-100);
    assert(taxi_camera_yaw_offset_step(33,33,7)==33);
    assert(taxi_camera_yaw_offset_step(0,100,0)==0);
    {uint16_t mode=2;assert(taxi_camera_select_mode(&mode,0,0)==1&&mode==1);
    mode=18;assert(taxi_camera_select_mode(&mode,1,0)==2&&mode==18);
    assert(taxi_camera_select_mode(&mode,2,0)==-1&&mode==18);
    assert(taxi_camera_select_mode(&mode,2,1)==2&&mode==18);
    mode=0;assert(taxi_camera_select_mode(&mode,1,0)==0);
    TaxiCameraOffsets fixed={.zoom_2628=123};int32_t roll=0;
    assert(!taxi_camera_fixed_offsets(0,&fixed,&roll)&&roll==0x4f000&&fixed.height_76c==0x159000&&fixed.distance_770==0x333000&&fixed.zoom_2628==123);
    assert(!taxi_camera_fixed_offsets(1,&fixed,&roll)&&fixed.height_76c==0x14c000&&fixed.distance_770==0x280000);
    assert(taxi_camera_fixed_offsets(2,&fixed,&roll)==-1&&fixed.distance_770==0x280000);
    }
    TaxiCameraOffsets offsets={0};
    taxi_camera_mode2_offsets(&offsets,4096,0,1,0,4096,12288);
    assert(offsets.zoom_2628==0&&offsets.height_76c==468*4096&&offsets.distance_770==3072*4096);
    taxi_camera_mode2_offsets(&offsets,4710,0,1,0,4096,12288);
    assert(offsets.zoom_2628==32768&&offsets.distance_770==(3072+8)*4096);
    taxi_camera_mode2_offsets(&offsets,4710,0,64,0,4096,12288);
    assert(offsets.zoom_2628==512*4096);
    taxi_camera_mode2_offsets(&offsets,4096,32,1,1,4096,12288);
    assert(offsets.zoom_2628==512*4096);
    taxi_camera_mode2_offsets(&offsets,4096,32,200,0,4096,12288);
    assert(offsets.zoom_2628==-384*4096);
    assert(taxi_camera_transform_yaw(-1,0)==-1);
    assert(taxi_camera_transform_yaw(4095,0)==0);
    assert(taxi_camera_transform_yaw(1024*4096,0)==1024);
    assert(taxi_camera_transform_yaw(4095*4096,2)==4097);
    TaxiTrig t={0};
    t.cosine[0]=256;t.sine[1024]=256;
    TaxiPoint projected={7,8,9};
    assert(taxi_camera_project(&projected,(TaxiPoint){-100,-200,-300},(TaxiPoint){0},-1024,-1024,0,100,&t));
    assert(projected.x==270&&projected.y==130&&projected.z==200);
    assert(taxi_camera_project(&projected,(TaxiPoint){100,50,1000},(TaxiPoint){0},0,0,0,192,&t));
    assert(projected.x==139&&projected.y==71&&projected.z==1000);
    assert(taxi_camera_project(&projected,(TaxiPoint){-100,-50,1000},(TaxiPoint){0},0,0,0,192,&t));
    assert(projected.x==101&&projected.y==89);
    projected=(TaxiPoint){7,8,9};
    assert(!taxi_camera_project(&projected,(TaxiPoint){0},(TaxiPoint){0},0,0,0,192,&t));
    assert(projected.x==7&&projected.y==8&&projected.z==9);
    /* Original positive local-Z movement and negative local-Z camera:
       preserve the camera behind forward travel at cardinal headings. */
    TaxiPoint forward0=taxi_rotate_point((TaxiPoint){0,0,256},0,0,0,&t);
    TaxiPoint behind0=taxi_camera_position((TaxiPoint){0},0,0,468*4096,3072*4096,0,&t);
    assert((int64_t)forward0.x*behind0.x+(int64_t)forward0.z*behind0.z<0);
    TaxiPoint forward1=taxi_rotate_point((TaxiPoint){0,0,256},0,1024,0,&t);
    TaxiPoint behind1=taxi_camera_position((TaxiPoint){0},0,1024*4096,468*4096,3072*4096,0,&t);
    assert((int64_t)forward1.x*behind1.x+(int64_t)forward1.z*behind1.z<0);
    TaxiPoint camera=taxi_camera_position((TaxiPoint){100,200,300},0,0,468*4096,3072*4096,0,&t);
    assert(camera.x==100&&camera.y==668&&camera.z==-2772);
    camera=taxi_camera_position((TaxiPoint){100,200,300},0,1024*4096,468*4096,3072*4096,0,&t);
    assert(camera.x==3172&&camera.y==668&&camera.z==300);
    camera=taxi_camera_position((TaxiPoint){0},0,0,-1,4095,50,&t);
    assert(camera.y==-1&&camera.z==50); /* Signed floor before rotation. */
    assert(taxi_camera_yaw_step(123,123,1,0,&t)==123);
    t.sine[256]=32;
    assert(taxi_camera_yaw_step(256*4096,0,1,0,&t)==256*4096-12288);
    assert(taxi_camera_yaw_step(-256*4096,0,1,0,&t)==-256*4096+12288);
    assert(taxi_camera_yaw_step(256*4096,0,2,0,&t)==256*4096-24576);
    assert(taxi_camera_yaw_step(1000*4096,0,1,0,&t)==853*4096);
    assert(taxi_camera_yaw_step(-1000*4096,0,1,0,&t)==-853*4096);
    assert(taxi_camera_yaw_step(2000*4096,0,1,1,&t)==1991*4096);
    assert(taxi_camera_yaw_step(2048*4096,0,1,0,&t)==3243*4096);
    t.sine[1]=32;
    /* Preserve original crossing correction, even where it overshoots. */
    assert(taxi_camera_yaw_step(4096,0,1,0,&t)==0);
    assert(taxi_camera_yaw_step(-4096,0,1,0,&t)==16384);
    assert(taxi_camera_yaw_step(-1,0,1,0,&t)==12287+8192);
    assert(taxi_camera_pitch_step(0,256,1,&t)==-24576);
    assert(taxi_camera_pitch_step(0,-256,1,&t)==24576);
    assert(taxi_camera_pitch_step(0,0,1,&t)==0);
    uint16_t mode=2;
    camera=taxi_camera_rear_position((TaxiPoint){100,200,300},0,0,-256,&t);
    assert(camera.x==100&&camera.y==840&&camera.z==300);
    camera=taxi_camera_rear_position((TaxiPoint){100,200,300},0,0,256,&t);
    assert(camera.x==100&&camera.y==840&&camera.z==812);
    assert(taxi_camera_command(&mode,1)==1 && mode==18);
    assert(taxi_camera_command(&mode,3)==0 && mode==18); /* Hold takes priority. */
    assert(taxi_camera_command(&mode,0)==1 && mode==2);
    assert(taxi_camera_command(&mode,2)==0 && mode==3);
    taxi_camera_command(&mode,2);assert(mode==0);
    taxi_camera_command(&mode,2);assert(mode==2);
    mode=1;taxi_camera_command(&mode,2);assert(mode==2);
    {
        TaxiCameraOffsets offsets={123,0,0};int32_t roll=0;uint8_t flags=0x80;
        taxi_camera_mode3_offsets(&offsets,680,101,&roll,&flags);
        assert(offsets.zoom_2628==123&&offsets.height_76c==0x159000);
        assert(offsets.distance_770==629*4096&&roll==0x4f000&&flags==0x82);
        taxi_camera_mode3_offsets(&offsets,680,-101,&roll,&flags);
        assert(offsets.distance_770==730*4096);
        taxi_camera_mode3_offsets(&offsets,0,INT32_MIN,&roll,&flags);
        assert(offsets.distance_770==0); /* Original wrap before shift. */
    }
    {
        TaxiCameraView view=taxi_camera_mode3_view((TaxiPoint){100,200,300},0,0,0,(TaxiPoint){0,0,0},-101,&t);
        assert(view.pitch==0&&view.yaw==0&&view.roll==0);
        assert(view.position.x==100&&view.position.y==545&&view.position.z==249);
        view=taxi_camera_mode3_view((TaxiPoint){100,200,300},0,0,0,(TaxiPoint){65536,-1,65536},100,&t);
        assert(view.pitch==0&&view.yaw==-1&&view.roll==0); /* Halfword wrap and ASR, not division. */
    }
    {
        TaxiCameraOffsets offsets={0,468*4096,3072*4096};
        const int32_t values[]={-4097,-4096,-1,0,4095,4096};
        const int16_t expected[]={-2,-1,-1,0,0,1};
        for(unsigned i=0;i<6;i++){
            TaxiCameraView view=taxi_camera_normal_view((TaxiPoint){0,0,0},values[i],0,-1,&offsets,0,&t);
            assert(view.pitch==expected[i]&&view.yaw==-1&&view.roll==0);
        }
    }
    puts("PASS: recovered camera tracking and original rear-view/mode command transitions.");
}



