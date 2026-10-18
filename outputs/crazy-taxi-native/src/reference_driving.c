#include "reference_driving.h"
#include "vehicle_start_driver.h"
#include "vehicle_start_world.h"
#include "input_snapshot.h"
#include "actions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int32_t word(const uint8_t*p){uint32_t u=(uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
static int read_exact(const char*path,void*buffer,size_t size){FILE*f=fopen(path,"rb");if(!f)return -1;int bad=fread(buffer,1,size,f)!=size||fgetc(f)!=EOF||ferror(f);if(fclose(f))bad=1;return bad?-1:0;}
static TaxiDriverSoundObserver sound_observer;
static void *sound_observer_context;
void taxi_reference_driving_set_sound_observer(TaxiDriverSoundObserver observer,void *context){sound_observer=observer;sound_observer_context=context;}
static TaxiReferencePostDriverHook pre_driver_hook;
static void *pre_driver_context;
void taxi_reference_driving_set_pre_driver_hook(TaxiReferencePostDriverHook hook,void *context){pre_driver_hook=hook;pre_driver_context=context;}
static TaxiReferencePostDriverHook post_driver_hook;
static void *post_driver_context;
void taxi_reference_driving_set_post_driver_hook(TaxiReferencePostDriverHook hook,void *context){post_driver_hook=hook;post_driver_context=context;}
void taxi_reference_driving_free(TaxiReferenceDriving*f){if(f){free(f->steps);memset(f,0,sizeof(*f));}}
int taxi_reference_driving_load(TaxiReferenceDriving*out,const char*path){
 if(!out||out->steps||!path)return -1;TaxiReferenceDriving f={0};FILE*in=fopen(path,"r");if(!in)return -1;
 char magic[40]={0};int bad=fscanf(in,"%39s %u",magic,&f.count)!=2||(strcmp(magic,"SOURCE_DRIVING_REFERENCE_V1")&&strcmp(magic,"SOURCE_DRIVING_REFERENCE_V2")&&strcmp(magic,"SOURCE_DRIVING_REFERENCE_V3")&&strcmp(magic,"SOURCE_DRIVING_REFERENCE_V4"))||!f.count||f.count>16000;
 if(!bad){f.steps=calloc(f.count,sizeof(*f.steps));if(!f.steps)bad=1;}
 int observed=strcmp(magic,"SOURCE_DRIVING_REFERENCE_V1")!=0,dynamic_owners=!strcmp(magic,"SOURCE_DRIVING_REFERENCE_V3")||!strcmp(magic,"SOURCE_DRIVING_REFERENCE_V4"),verify_field=!strcmp(magic,"SOURCE_DRIVING_REFERENCE_V4");uint64_t ticks=0;for(unsigned i=0;!bad&&i<f.count;i++){TaxiReferenceDrivingStep*s=f.steps+i;
  if(fscanf(in,"%u %u %u %u %d %d %d %d %d",&s->raw,&s->prior,&s->delta,&s->frame,&s->expected.x,&s->expected.y,&s->expected.z,&s->heading,&s->speed)!=9||s->raw>1023||!s->prior||s->prior>60||!s->delta||s->delta>60||!s->frame||s->frame>60){bad=1;break;}if(observed){unsigned long long stamp;if(fscanf(in,"%llu",&stamp)!=1||stamp<=ticks||stamp-ticks>60){bad=1;break;}ticks=(uint64_t)stamp;}else ticks+=s->delta;s->tick=ticks;if(dynamic_owners){unsigned f1697,f153c,freeze2620,freeze177b;if(fscanf(in,"%u %u %u %u",&f1697,&f153c,&freeze2620,&freeze177b)!=4||f1697>1||f153c>1||freeze2620>1||freeze177b>1){bad=1;break;}s->dynamic_owners=1;s->flag_1697=(uint8_t)f1697;s->flag_153c=(uint8_t)f153c;s->freeze_2620=(uint8_t)freeze2620;s->freeze_177b=(uint8_t)freeze177b;}if(verify_field){unsigned verify;if(fscanf(in," %u",&verify)!=1||verify>3){bad=1;break;}s->verify_mask=(uint8_t)verify;}else s->verify_mask=3;
 }
 char extra;if(!bad&&fscanf(in," %c",&extra)==1)bad=1;if(ferror(in))bad=1;if(fclose(in))bad=1;
 uint8_t ram[0x40000],iw[0x8000];if(!bad&&(read_exact("outputs/crazy-taxi-native/reports/original-core-reference/boot4200.ewram",ram,sizeof ram)||read_exact("outputs/crazy-taxi-native/reports/original-core-reference/boot4200.03000000.bin",iw,sizeof iw)))bad=1;
 if(!bad){TaxiVehicleStartPacked packed={0};if(taxi_driving_shared_snapshot_read(&f.shared,ram,sizeof ram,iw+0x53c0,48))bad=1;
  memcpy(packed.actor,ram+0x2634,sizeof packed.actor);packed.state_2630=f.shared.context.gear.state_2630;
  if(taxi_vehicle_start_driver_export(&f.car.driver,&f.context.driver,&packed,f.shared.settings)||taxi_vehicle_start_world_export(&f.car.world,&packed)||taxi_input_snapshot_read(&f.input,ram+0x34dc,68))bad=1;
  f.context.driver=f.shared.context;f.command=f.shared.command;f.car.speed_divisor_548=word(ram+0x2634+0x548);f.car.camera_offset_690=word(ram+0x2634+0x690);f.car.initial_camera_pitch_650=word(ram+0x2634+0x650);f.car.initial_camera_heading_654=word(ram+0x2634+0x654);f.car.initial_camera_mode_64c=(uint16_t)(ram[0x2634+0x64c]|((uint16_t)ram[0x2634+0x64d]<<8));
  f.context.height=(TaxiHeightGlobals){.distance_1540=word(ram+0x1540),.takeoff_1544={word(ram+0x1544),word(ram+0x1548),word(ram+0x154c)},.flag_153c=ram[0x153c],.flag_153d=ram[0x153d],.flag_153e=ram[0x153e],.flag_1697=ram[0x1697]};
  f.pitch=word(ram+0x2634+0x650);f.heading=word(ram+0x2634+0x654);f.offset=word(ram+0x2618);f.target=word(ram+0x261c);f.base=word(ram+0x2634+0x680);f.offsets=(TaxiCameraOffsets){word(ram+0x2628),word(ram+0x2634+0x76c),word(ram+0x2634+0x770)};f.intro=ram[0x1696];f.paused=ram[0x5f20];f.suppress=ram[0x1eaa];f.parent_distance=word(ram+0x1678);f.parent_height=word(ram+0x167c);
 }
 if(bad){taxi_reference_driving_free(&f);return -1;}*out=f;return 0;
}
int taxi_reference_driving_step(TaxiReferenceDriving*f,const TaxiTrig*t,const TaxiAngleTable*a,const TaxiNormalAngles*n,TaxiObstacleQuery obstacle,TaxiGroundQuery ground,void*world){
 if(!f||f->next>=f->count)return -1;const TaxiReferenceDrivingStep*s=f->steps+f->next;
 if(s->dynamic_owners){f->context.height.flag_1697=s->flag_1697;f->context.height.flag_153c=s->flag_153c;f->context.driver.freeze_2620=s->freeze_2620;f->shared.freeze_177b=s->freeze_177b;}
 taxi_input_update(&f->input,(TaxiInputSample){.active_low_keys=(uint16_t)s->raw,.elapsed_ticks=s->prior,.suppress_physical=f->suppress});TaxiActionContext actions={.profile=(f->shared.settings>>1)&3,.vehicle_flags=f->car.driver.power.gears.flags_4ac};taxi_command_pack(&f->command,&f->input);
 TaxiGearInput in={.delta=(int32_t)s->delta,.frame_delta=(int32_t)s->frame,.settings=f->shared.settings,.freeze_177b=f->shared.freeze_177b,.sound_enabled=f->shared.sound_enabled};memcpy(in.held,f->input.held,10);in.action2=(uint8_t)taxi_action_query(&f->input,actions,2);in.action3=(uint8_t)taxi_action_query(&f->input,actions,3);in.action4=(uint8_t)taxi_action_query(&f->input,actions,4);in.action5=(uint8_t)taxi_action_query(&f->input,actions,5);
 if(pre_driver_hook&&pre_driver_hook(pre_driver_context,f,s,t))return -1;
 TaxiDriveWorldEffects e;if(taxi_drive_world_step_observed(&f->car,&f->context,&f->command,&in,0,t,a,n,obstacle,ground,world,&e,sound_observer,sound_observer_context))return -1;f->last_effects=e;if(post_driver_hook&&post_driver_hook(post_driver_context,f,s,t))return -1;
 if((s->verify_mask&1)&&(f->car.world.height.position.x!=s->expected.x||f->car.world.height.position.y!=s->expected.y||f->car.world.height.position.z!=s->expected.z||f->car.world.heading_1c!=s->heading))return -1;if((s->verify_mask&2)&&f->car.world.speed_544!=s->speed)return -1;
 f->heading=taxi_camera_yaw_step(f->heading,f->car.world.heading_20,(int32_t)s->delta,f->intro,t);f->pitch=taxi_camera_pitch_step(f->pitch,(int16_t)f->car.world.angle_18,(int32_t)s->delta,t);f->offset=taxi_camera_yaw_offset_step(f->offset,f->target,(int32_t)s->frame);taxi_camera_mode2_offsets(&f->offsets,f->car.world.height.scale_e84,f->car.driver.power.gears.flags_4ac,(int32_t)s->frame,f->paused,f->parent_distance,f->parent_height);f->next++;return 0;
}
TaxiCameraView taxi_reference_driving_view(const TaxiReferenceDriving*f,const TaxiTrig*t){return taxi_camera_normal_view(f->car.world.height.position,f->pitch,f->heading,f->offset,&f->offsets,f->base,t);}
