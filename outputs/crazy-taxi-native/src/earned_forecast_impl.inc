#include "earned_forecast.h"
#include <string.h>
#include "traffic_predict_prepare.h"
#include "forecast_camera_step.h"
int earned_forecast(const EarnedForecastState *initial,const EarnedForecastResources *r,const TaxiXboxSample *pads,const int32_t *delta,size_t count,EarnedForecastResult *out){
 if(!initial||!r||!r->runtime||!r->roads||!r->obstacle||!r->ground||!pads||!delta||!out||!count||count>60||initial->traffic_count>50)return -1;
 if(initial->camera.enabled&&(!r->traffic_height||(initial->camera.mode&~16u)!=2))return -1;
 for(size_t i=0;i<count;i++)if(delta[i]<1||delta[i]>60||!pads[i].connected)return -1;
 EarnedForecastResult next={0};next.state=*initial;next.traffic_index=50;
 for(size_t step=0;step<count;step++){
  EarnedForecastState *s=&next.state;TaxiXboxSample pad=pads[step];int32_t dt=delta[step];
  /* No source spawn/RNG or camera-dependent common step is guessed here. */
  for(size_t j=0;j<s->traffic_count;j++){
   if(s->traffic[j].countdown>0&&s->traffic[j].countdown<=dt){next.unsupported=EARNED_FORECAST_PENDING_SPAWN;next.traffic_index=j;*out=next;return 0;}
   if(step&&!s->camera.enabled&&s->traffic[j].countdown<=0){next.unsupported=EARNED_FORECAST_TRAFFIC_COMMON;next.traffic_index=j;*out=next;return 0;}
  }
  uint16_t keys=taxi_xbox_driving_keys(pad,0);taxi_input_update(&s->buttons,(TaxiInputSample){.active_low_keys=keys,.elapsed_ticks=dt});
  TaxiActionContext actions={.profile=0,.vehicle_flags=s->car.driver.power.gears.flags_4ac};
  int gas=taxi_action_query(&s->buttons,actions,2)||pad.accelerator>30,brake=taxi_action_query(&s->buttons,actions,3)||pad.brake>30;
  TaxiCommand cmd={0};cmd.bytes[4]=(uint8_t)((gas?4:0)|(brake?8:0)|(taxi_action_query(&s->buttons,actions,0)?1:0)|(taxi_action_query(&s->buttons,actions,1)?2:0));
  uint16_t analog=(uint16_t)taxi_xbox_vehicle_steering_command(pad.steering,(uint8_t)s->car.driver.power.gears.phase_e6a,s->context.driver.gear.state_2630);cmd.bytes[12]=(uint8_t)analog;cmd.bytes[13]=(uint8_t)(analog>>8);
  TaxiGearInput in={.delta=dt,.frame_delta=dt,.action2=(uint8_t)gas,.action3=(uint8_t)brake,.action4=(uint8_t)taxi_action_query(&s->buttons,actions,4),.action5=(uint8_t)taxi_action_query(&s->buttons,actions,5)};
  for(unsigned j=0;j<10;j++)in.held[j]=(uint8_t)(((keys^1023)>>j)&1);
  TaxiDriveWorldEffects effects;
  if(taxi_drive_world_step(&s->car,&s->context,&cmd,&in,0,&r->runtime->trig,&r->runtime->traction,&r->runtime->normals,r->obstacle,r->ground,r->terrain,&effects))return -1;
  next.completed_steps++;
  if(effects.world.movement.collided){next.world_hit=1;*out=next;return 0;}
  TaxiCameraView view={0};if(earned_forecast_camera_step(s,r,pad,dt,&view))return -1;
  for(size_t j=0;j<s->traffic_count;j++){
   TaxiTrafficActor *a=s->traffic+j;if(a->countdown>0){a->countdown-=dt;continue;}if(a->route>=256)return -1;
   const TaxiAiPathAsset *road=r->roads+a->route;
   int hit=traffic_predict_prepare(a,r->roads,dt,dt,s->car.world.height.position.x,s->car.world.height.position.z,s->car.world.speed_544);
   if(hit<0)return -1;if(hit){next.traffic_hit=1;next.traffic_index=j;*out=next;return 0;}
   if(!(a->flags&1)&&taxi_traffic_postimpact_motion(a,road,dt,dt,0,0,r->obstacle,r->terrain,&r->runtime->traction))return -1;
   if(s->camera.enabled&&taxi_traffic_presentation_prepare(a,view.position.x,view.position.z,dt,0,r->traffic_height,r->terrain)<0)return -1;
  }
 }
 *out=next;return 0;
}