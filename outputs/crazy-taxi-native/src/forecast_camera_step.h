#ifndef EARNED_FORECAST_CAMERA_STEP_H
#define EARNED_FORECAST_CAMERA_STEP_H
#include "earned_forecast.h"
/* Internal diagnostic composition. Caller supplies normal, unpaused, intro0,
   equal global/frame delta state, mode2 and valid runtime. Mutates copies only. */
static int earned_forecast_camera_step(EarnedForecastState *s,const EarnedForecastResources *r,TaxiXboxSample pad,int32_t dt,TaxiCameraView *out){
 TaxiCameraView view={0};
  if(s->camera.enabled){
   EarnedForecastCamera *camera=&s->camera;
   camera->heading=taxi_camera_yaw_step(camera->heading,s->car.world.heading_20,dt,0,&r->runtime->trig);
   camera->pitch=taxi_camera_pitch_step(camera->pitch,(int16_t)s->car.world.angle_18,dt,&r->runtime->trig);
   camera->yaw_offset=taxi_camera_yaw_offset_step(camera->yaw_offset,camera->yaw_target,dt);
   taxi_camera_command(&camera->mode,taxi_xbox_camera_command(0,pad.look_back));
   if(taxi_camera_select_mode(&camera->mode,1,0)!=2)return -1;
   taxi_camera_mode2_offsets(&camera->offsets,s->car.world.height.scale_e84,s->car.driver.power.gears.flags_4ac,dt,0,camera->preset_y?camera->preset_y:4096,camera->preset_z?camera->preset_z:12288);
   view=taxi_camera_mode2_view(s->car.world.height.position,camera->pitch,camera->heading,camera->yaw_offset,&camera->offsets,camera->base,s->car.camera_offset_690,camera->mode,&s->car.world.camera_flags_29,&r->runtime->trig);
  }
 *out=view;return 0;
}
#endif
