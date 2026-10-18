#ifndef TAXI_GAME_START_H
#define TAXI_GAME_START_H
#include <stdint.h>
typedef struct {int16_t phase,hold,offset,scale;uint16_t rotation;int32_t axes[3];uint8_t active;} TaxiGameStart;
enum {TAXI_START_SOUND_READY=1,TAXI_START_SOUND_SET=2,TAXI_START_SOUND_GO=4,TAXI_START_ART=8,TAXI_START_FINISHED=16};
/* Ordinary settings branch BA04..BD40 only. Host supplies original axis scales;
   owns actor130AC, camera, art/palette replacement and audio dispatch. */
void taxi_game_start_begin(TaxiGameStart *s,int16_t width,const int32_t axes[3]);
/* SourceB928 ordinary initializer, distinct from demo/special mode. */
void taxi_game_start_begin_ordinary(TaxiGameStart *s,int16_t width);
unsigned taxi_game_start_step(TaxiGameStart *s,int32_t delta,int16_t width,int fade,int exiting,int audio_enabled);
/* Challenge-only opening B916/B92E and BA44..BA9A. Stops at phase0;
   caller must use recovered challenge Ready/Set/Go and audio owners next. */
void taxi_game_start_begin_challenge(TaxiGameStart *state,int16_t width);
int taxi_game_start_challenge_intro(TaxiGameStart *state,int32_t delta,
    int fade,int exiting);
/* SourceBBB0/BBF4: challenge Ready only, phase0->2 artwork transition,
   then shared GO/scale endpoint. Caller owns actor/art/audio dispatch. */
unsigned taxi_game_start_challenge_step(TaxiGameStart *state,int32_t delta,
    int16_t width,int fade,int exiting,int audio_enabled);
#endif
