#ifndef TAXI_DRIVE_WORLD_H
#define TAXI_DRIVE_WORLD_H
#include "driver.h"
#include <stdio.h>
#include "world_update.h"
typedef struct {TaxiDriver driver;TaxiWorldState world;
    /* Final11804->11540 initialization values after template copy. */
    int32_t speed_divisor_548;
    int32_t camera_offset_690;
    int32_t initial_camera_pitch_650,initial_camera_heading_654;
    uint16_t initial_camera_mode_64c;
} TaxiDriveWorld;
typedef struct {TaxiDriverContext driver;TaxiHeightGlobals height;} TaxiDriveWorldContext;
typedef struct {TaxiDriverEffects driver;TaxiWorldEffects world;} TaxiDriveWorldEffects;
/* Normal single-driver simulation arithmetic in 0800b022..0800b026 order.
   On entry world owns position/headings/body angles; driver owns powertrain
   speed/flags/scales. After success both copies agree. Initialization remains
   caller-owned: this function is not the original spawn routine. Includes neutral/airborne free-rev; excludes
   outer render/effect bookkeeping. The submode comes from the
   driver's orientation context. completion_result resolves the pending
   challenge handler as described in height_update.h. All writes roll back. */
int taxi_drive_world_step(TaxiDriveWorld *state,TaxiDriveWorldContext *context,
    TaxiCommand *command,const TaxiGearInput *input,uint8_t completion_result,
    const TaxiTrig *trig,const TaxiAngleTable *traction_angles,const TaxiNormalAngles *normal_angles,
    TaxiObstacleQuery obstacle,TaxiGroundQuery ground,void *query_context,TaxiDriveWorldEffects *effects);
/* Same mathematics with driver helper sound requests observed before world
   movement. External observer effects are not rolled back on later failure. */
int taxi_drive_world_step_observed(TaxiDriveWorld *,TaxiDriveWorldContext *,
    TaxiCommand *,const TaxiGearInput *,uint8_t,const TaxiTrig *,
    const TaxiAngleTable *,const TaxiNormalAngles *,TaxiObstacleQuery,TaxiGroundQuery,
    void *,TaxiDriveWorldEffects *,TaxiDriverSoundObserver,void *);
/* Startup actor zero: 130AC gear-only then 11F80 world update. Retains world
   displacements and skips steering, acceleration, brake and motion generation.
   Input gear state owns shared speed/flags; world owns position and residual
   movement. No outer fade gate, other-actor loop or camera update is implied.
   State/context/effects commit atomically; query callbacks must be pure. */
int taxi_drive_world_start_step(TaxiDriveWorld *state,TaxiDriveWorldContext *context,
    const TaxiGearInput *input,uint8_t completion_result,const TaxiTrig *trig,
    const TaxiNormalAngles *angles,TaxiObstacleQuery obstacle,TaxiGroundQuery ground,
    void *query_context,TaxiDriveWorldEffects *effects);
/* NormalB08C blocked-movement branch: original130AC(actor,0) only;
   position, displacement, terrain and body angles remain untouched. */
int taxi_drive_world_gear_step(TaxiDriveWorld *,TaxiDriveWorldContext *,const TaxiGearInput *,TaxiDriveWorldEffects *);
void taxi_drive_world_gear_trace(FILE *sink);
#endif


