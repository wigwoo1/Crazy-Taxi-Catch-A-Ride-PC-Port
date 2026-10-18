#ifndef TAXI_MOVEMENT_H
#define TAXI_MOVEMENT_H
#include "ground.h"
#include "impact.h"
typedef int (*TaxiObstacleQuery)(void *context,TaxiPoint point,TaxiObstacleHit *hit);
typedef struct {
    TaxiPoint position,normal;
    unsigned queries;
    int collided;
} TaxiMovementResult;
/* Read-only diagnostic observer: must not mutate state/query context or reenter. */
typedef void (*TaxiMovementObserver)(TaxiPoint,TaxiPoint,int,TaxiObstacleQuery,void*,const TaxiMovementResult*);
void taxi_movement_observer(TaxiMovementObserver observer);
/* Position-only path of 0802a27c (r0=null, r1=position).
   preserve_query_y is the original settings/submode exception. No vehicle
   impulse, sound, camera shake or damage is applied by this routine.
   Returns 0 on success, -1 on invalid query data. */
int taxi_move_position(TaxiPoint start,TaxiPoint displacement,int preserve_query_y,
    TaxiObstacleQuery query,void *context,TaxiMovementResult *result);
typedef struct {
    TaxiPoint position;
    TaxiImpactState impact;
    int32_t state_e84;
} TaxiVehicleBody;
/* Vehicle-state path: position correction followed by the impact arithmetic.
   This is collision movement, not the throttle/steering update. Effects are
   meaningful only when result->collided is true. */
int taxi_move_vehicle(TaxiVehicleBody *body,TaxiPoint displacement,uint8_t settings,
    uint8_t submode,const TaxiTrig *trig,TaxiObstacleQuery query,void *context,
    TaxiMovementResult *result,TaxiImpactEffects *effects);
#endif
