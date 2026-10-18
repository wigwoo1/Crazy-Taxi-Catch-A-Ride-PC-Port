#ifndef TAXI_CHALLENGE_BOUND_COURSE_H
#define TAXI_CHALLENGE_BOUND_COURSE_H
#include "challenge_assets.h"
#include "passenger_asset.h"
#include "passenger_init.h"
#include "simulation_start.h"
#include "challenge_bound.h"
typedef struct {TaxiChallengeAssets resources;TaxiPassengerAsset actors;} TaxiChallengeBoundCourse;
void taxi_challenge_bound_course_free(TaxiChallengeBoundCourse *);
int taxi_challenge_bound_course_load(TaxiChallengeBoundCourse *,const char *root);
/* Retains caller actor Y as original ground query seed; reset provenance unresolved. */
int taxi_challenge_bound_course_actor(TaxiChallengeBoundCourse *,TaxiPassengerState *);
int taxi_challenge_bound_course_start(TaxiChallengeBoundCourse *,TaxiDriveWorld *,TaxiDriveWorldContext *,TaxiChallengeBound *,uint8_t *option,const uint8_t *vehicle_template,size_t size,uint8_t settings);
int taxi_challenge_bound_course_control(const TaxiChallengeBoundCourse *,TaxiDriveWorldContext *,TaxiChallengeBound *,TaxiChallengeBoundInput,TaxiChallengeBoundStatus *,TaxiChallengeBoundEffects *);
#endif
