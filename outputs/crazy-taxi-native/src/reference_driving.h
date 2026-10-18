#ifndef TAXI_REFERENCE_DRIVING_H
#define TAXI_REFERENCE_DRIVING_H
/* Offline-reference diagnostic fixture only. Never used by playable startup. */
#include "drive_world.h"
#include "driving_shared_snapshot.h"
#include "camera_angle.h"
#include "input.h"
typedef struct{unsigned raw,prior,delta,frame;TaxiPoint expected;int32_t heading,speed;uint64_t tick;uint8_t dynamic_owners,flag_1697,flag_153c,freeze_2620,freeze_177b,verify_mask;} TaxiReferenceDrivingStep;
typedef struct TaxiReferenceDriving TaxiReferenceDriving;
typedef int (*TaxiReferencePostDriverHook)(void *,TaxiReferenceDriving *,const TaxiReferenceDrivingStep *,const TaxiTrig *);
struct TaxiReferenceDriving{TaxiDriveWorld car;TaxiDriveWorldContext context;TaxiInput input;TaxiCommand command;TaxiDrivingSharedSnapshot shared;TaxiReferenceDrivingStep *steps;unsigned count,next;TaxiDriveWorldEffects last_effects;int32_t pitch,heading,offset,target,base,parent_distance,parent_height;TaxiCameraOffsets offsets;uint8_t intro,paused,suppress;};
int taxi_reference_driving_load(TaxiReferenceDriving *,const char *);
/* Offline service boundary after input/owner import and command packing,
   before any driver/world physics. A failure prevents physics/post service
   and does not advance the fixture; callback/input mutations are not rolled back. */
void taxi_reference_driving_set_pre_driver_hook(TaxiReferencePostDriverHook,void *);
void taxi_reference_driving_set_post_driver_hook(TaxiReferencePostDriverHook,void *);
/* Offline immediate driver helper sound boundary, default NULL. */
void taxi_reference_driving_set_sound_observer(TaxiDriverSoundObserver,void *);
int taxi_reference_driving_step(TaxiReferenceDriving *,const TaxiTrig *,const TaxiAngleTable *,const TaxiNormalAngles *,TaxiObstacleQuery,TaxiGroundQuery,void *);
TaxiCameraView taxi_reference_driving_view(const TaxiReferenceDriving *,const TaxiTrig *);
void taxi_reference_driving_free(TaxiReferenceDriving *);
#endif
