#ifndef TAXI_WORLD_UPDATE_H
#define TAXI_WORLD_UPDATE_H
#include "movement.h"
#include "height_update.h"
typedef int (*TaxiGroundQuery)(void *context,TaxiPoint point,TaxiGroundHit *hit);
typedef struct {
    TaxiHeightState height;
    int32_t displacement_x,displacement_z;
    int32_t heading_1c,heading_20,drift_69c;
    int32_t impulse_74c,impulse_750,impulse_34;
    int32_t speed_544,state_40,state_44;
    uint16_t state_2c,angle_18,angle_1a,angle_38,angle_3a,material_e60;
    int32_t height_3c,ground_36c;
    uint8_t red_374,green_375,blue_376;
    uint8_t camera_flags_29;
    uint32_t contact_378,previous_37c;
    TaxiWorldContact snapshot_714;
} TaxiWorldState;
typedef struct {
    TaxiMovementResult movement;
    TaxiImpactEffects impact;
    TaxiHeightEffects height;
    int ground_hit;
} TaxiWorldEffects;
/* Native state arithmetic of 08011f80, composed with movement/impact, gravity,
   contact decay, height transitions and final body pose. External effect
   handlers remain requests (see height_update.h). Pure query callbacks must
   return -1 invalid, 0 no hit, 1 ground (obstacle callback also allows 2).
   All state/global/effect output rolls back on error; callback side effects
   cannot be rolled back. This is not full spawn or outer driver bookkeeping. */
int taxi_world_update(TaxiWorldState *state,TaxiHeightGlobals *globals,
    const TaxiHeightInput *input,const TaxiTrig *trig,const TaxiNormalAngles *angles,
    TaxiObstacleQuery obstacle,TaxiGroundQuery ground,void *context,TaxiWorldEffects *effects);
#endif

