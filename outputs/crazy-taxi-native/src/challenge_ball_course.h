#ifndef TAXI_CHALLENGE_BALL_COURSE_H
#define TAXI_CHALLENGE_BALL_COURSE_H
#include "challenge_assets.h"
#include "simulation_start.h"
#include "passenger_asset.h"
#include "passenger_init.h"
#include "challenge_ball.h"
#include "challenge_ball_projection.h"
typedef struct {
 TaxiChallengeAssets resources;
 TaxiPassengerAsset actors;
 uint8_t balloon_indices[8192];
} TaxiChallengeBallCourse;
/* Initialize to zero. Loads original challenge2 resource dependencies from
   assets root. Failed replacement retains current course; successful load
   owns storage. Does not invent retained actor Y or run session callbacks. */
int taxi_challenge_ball_course_load(TaxiChallengeBallCourse *course,const char *assets_root);
void taxi_challenge_ball_course_free(TaxiChallengeBallCourse *course);
/* Original placement/configuration/reset with selected caller template,
   course GRID marker and submode2. Gameplay/actor/camera/audio state excluded. */
int taxi_challenge_ball_course_start(TaxiChallengeBallCourse *course,TaxiDriveWorld *vehicle,TaxiDriveWorldContext *context,const uint8_t *vehicle_template,size_t size,uint8_t settings);
/* Uses caller-retained actor+20 as the original ground-query hint.
   Other unwritten actor bytes remain retained. Returns1 initialized,0 ground
   unavailable,-1 invalid; 0/-1 leave actor untouched. Retained-Y owner is
   deliberately caller-owned, not substituted with source-record Y. */
/* Original22C2A..22C94 submode2 actor initialization, all20 atomically. */
int taxi_challenge_ball_course_actors(TaxiChallengeBallCourse *course,TaxiPassengerState actors[20]);
/* Source controller before vehicle update: caller gates/status/effect ownership
   retained; car contact uses native vehicle X/Z. Source2DB54 RAM tables map
   sine to X, cosine to Z. No emulated BIOS/CPU execution. */
int taxi_challenge_ball_course_control(const TaxiChallengeBallCourse *course,
 TaxiChallengeBall *state,TaxiPassengerState actors[20],const TaxiDriveWorld *vehicle,
 TaxiBallInput gates,TaxiBallStatus *status,TaxiBallEffects *effects,const TaxiTrig *trig);
/* Original233D8 traversal. Caller advances once per simulation render tick;
   presentation redraws must use pause1/delta0 copies. Returns visible mask,
   ground unavailable suppresses that actor; invalid preparation is atomic. */
int taxi_challenge_ball_course_render(TaxiChallengeBallCourse *course,
 TaxiPassengerState actors[20],TaxiPoint player,TaxiPoint camera,int32_t yaw,int32_t pitch,
 int32_t horizontal,int32_t vertical,int pause,int32_t delta,uint32_t factor,
 const TaxiTrig *trig,TaxiBallProjection projections[20],uint32_t *visible);
#endif
