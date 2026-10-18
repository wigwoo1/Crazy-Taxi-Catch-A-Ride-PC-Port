#ifndef EARNED_FORECAST_H
#define EARNED_FORECAST_H
/* Diagnostic only: bounded native dynamics on copies. Not original game AI.
   No spawn/RNG/passenger simulation; optional copied normal camera/common update. Stop at the first predicted impact;
   returned hazard state must not be used as a completed real gameplay frame.
   Callbacks must be read-only; global diagnostic observers must be disabled.
   Without enabled camera and native height query, future traffic common updates are unsupported. */
#include "runtime_asset.h"
#include "traffic_actor.h"
#include "input.h"
#include "xbox.h"
#include "actions.h"
#include "camera_angle.h"
#include "traffic_presentation.h"
typedef struct {int enabled;int32_t heading,pitch,yaw_offset,yaw_target,base,preset_y,preset_z;TaxiCameraOffsets offsets;uint16_t mode;} EarnedForecastCamera;
typedef struct {TaxiDriveWorld car;TaxiDriveWorldContext context;TaxiInput buttons;TaxiTrafficActor traffic[50];size_t traffic_count;EarnedForecastCamera camera;} EarnedForecastState;
typedef struct {const TaxiRuntimeAsset *runtime;const TaxiAiPathAsset *roads;TaxiObstacleQuery obstacle;TaxiGroundQuery ground;void *terrain;TaxiTrafficHeightQuery traffic_height;} EarnedForecastResources;
enum {EARNED_FORECAST_PENDING_SPAWN=1,EARNED_FORECAST_TRAFFIC_COMMON=2};
/* unsupported!=0 is incomplete coverage, never an all-clear prediction.
   The state is the last completed prefix, before the unsupported next step. */
typedef struct {EarnedForecastState state;unsigned completed_steps,world_hit,traffic_hit,unsupported;size_t traffic_index;} EarnedForecastResult;
/* Profile0 normal driving only. Caller supplies an explicit future pad/delta
   sequence, max60 steps, delta1..60. Invalid inputs preserve *out. Caller state,
   road assets and live globals are never written. Future actors not spawned. */
int earned_forecast(const EarnedForecastState *,const EarnedForecastResources *,const TaxiXboxSample *,const int32_t *,size_t,EarnedForecastResult *);
#endif