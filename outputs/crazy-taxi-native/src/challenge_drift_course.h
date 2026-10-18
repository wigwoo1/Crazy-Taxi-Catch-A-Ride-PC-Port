#ifndef TAXI_CHALLENGE_DRIFT_COURSE_H
#define TAXI_CHALLENGE_DRIFT_COURSE_H
#include "challenge_assets.h"
#include "simulation_start.h"
#include "challenge_drift.h"
typedef struct {TaxiChallengeAssets resources;} TaxiChallengeDriftCourse;
/* Zero initialize; source course3 resources, atomic replacement. */
int taxi_challenge_drift_course_load(TaxiChallengeDriftCourse *course,const char *assets_root);
void taxi_challenge_drift_course_free(TaxiChallengeDriftCourse *course);
/* GRID/source vehicle/submode3 followed by AA64 scalar reset. Returns1
   initialized,0 no ground,-1 invalid; failures retain every output. */
int taxi_challenge_drift_course_start(TaxiChallengeDriftCourse *course,TaxiDriveWorld *car,TaxiDriveWorldContext *context,TaxiChallengeDrift *timer,uint8_t *option3,const uint8_t *vehicle_template,size_t size,uint8_t settings);
/* Before native physics; count comes from recovered orientation context.
   Synchronizes native ended/success gates; rank/palette/audio caller-owned. */
int taxi_challenge_drift_course_control(const TaxiChallengeDriftCourse *course,TaxiDriveWorldContext *context,TaxiChallengeDrift *timer,TaxiChallengeDriftInput gates,TaxiChallengeDriftStatus *status,TaxiChallengeDriftEffects *effects);
#endif
