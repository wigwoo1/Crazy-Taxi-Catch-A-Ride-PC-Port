#ifndef TAXI_CHALLENGE_FLAG_COURSE_H
#define TAXI_CHALLENGE_FLAG_COURSE_H
#include "challenge_assets.h"
#include "simulation_start.h"
#include "passenger_asset.h"
#include "passenger_init.h"
#include "challenge_flag.h"
typedef struct {
 TaxiChallengeAssets resources;
 TaxiPassengerAsset actors;
 uint8_t flag_indices[2048];
} TaxiChallengeFlagCourse;
/* Initialize to zero. Loads original challenge1 resource dependencies from
   assets root. Failed replacement retains current course; successful load
   owns storage. Does not invent retained actor Y or run session callbacks. */
int taxi_challenge_flag_course_load(TaxiChallengeFlagCourse *course,const char *assets_root);
void taxi_challenge_flag_course_free(TaxiChallengeFlagCourse *course);
/* Original placement/configuration/reset with selected caller template,
   course GRID marker and submode1. Gameplay/actor/camera/audio state excluded. */
int taxi_challenge_flag_course_start(TaxiChallengeFlagCourse *course,TaxiDriveWorld *vehicle,TaxiDriveWorldContext *context,const uint8_t *vehicle_template,size_t size,uint8_t settings);
/* Uses caller-retained actor+20 as the original ground-query hint.
   Other unwritten actor bytes remain retained. Returns1 initialized,0 ground
   unavailable,-1 invalid; 0/-1 leave actor untouched. Retained-Y owner is
   deliberately caller-owned, not substituted with source-record Y. */
int taxi_challenge_flag_course_actor(TaxiChallengeFlagCourse *course,TaxiPassengerState *actor);
/* Phase0 scalar callback connection: positions come from native vehicle and
   initialized Flag actor; caller still owns original intro/outer/pause state,
   rank/audio/palette requests and common result flow. No vehicle motion here. */
int taxi_challenge_flag_course_control(const TaxiChallengeFlagCourse *course,TaxiPassengerState *actor,const TaxiDriveWorld *vehicle,TaxiChallengeFlag *timer,const TaxiChallengeFlagInput *gates,TaxiChallengeFlagStatus *status,TaxiChallengeFlagEffects *effects);
#endif
