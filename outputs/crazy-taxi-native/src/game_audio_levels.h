#ifndef TAXI_GAME_AUDIO_LEVELS_H
#define TAXI_GAME_AUDIO_LEVELS_H
#include <stdint.h>
typedef struct {int32_t speed,max_speed,scale,cooldown,delta;int16_t rpm,factor;uint8_t muted,ended,outer;} TaxiGameAudioLevelsInput;
typedef struct {int32_t cooldown,speed_class,ratio,low,high,pitch;int16_t factor;} TaxiGameAudioLevels;
/* Original10B54..10C56 prefix; no handle starts/stops or device mixing. */
int taxi_game_audio_levels(const TaxiGameAudioLevelsInput *,TaxiGameAudioLevels *);
#endif
