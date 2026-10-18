#ifndef TAXI_CHALLENGE_TURN_COURSE_H
#define TAXI_CHALLENGE_TURN_COURSE_H
#include "challenge_assets.h"
#include "passenger_asset.h"
#include "passenger_init.h"
#include "simulation_start.h"
#include "challenge_turn.h"
typedef struct {TaxiChallengeAssets resources;TaxiPassengerAsset actors;} TaxiChallengeTurnCourse;
void taxi_challenge_turn_course_free(TaxiChallengeTurnCourse *);
int taxi_challenge_turn_course_load(TaxiChallengeTurnCourse *,const char *root);
/* Retains caller actor Y as original ground query seed; reset provenance unresolved. */
int taxi_challenge_turn_course_actor(TaxiChallengeTurnCourse *,TaxiPassengerState *);
int taxi_challenge_turn_course_start(TaxiChallengeTurnCourse *,TaxiDriveWorld *,TaxiDriveWorldContext *,TaxiChallengeTurn *,uint8_t *option,const uint8_t *vehicle_template,size_t size,uint8_t settings);
int taxi_challenge_turn_course_control(const TaxiChallengeTurnCourse *,TaxiDriveWorldContext *,TaxiChallengeTurn *,TaxiChallengeTurnInput,TaxiChallengeTurnStatus *,TaxiChallengeTurnEffects *);
#endif
