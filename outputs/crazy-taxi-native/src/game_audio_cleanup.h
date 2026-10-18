#ifndef TAXI_GAME_AUDIO_CLEANUP_H
#define TAXI_GAME_AUDIO_CLEANUP_H
#include <stdint.h>
#include "game_audio_engine.h"
/* Original10AAC retains ten owner slots020025E8..0200260C. Emit every
   nonzero handle in slot order, including repeated handles. No owner reset. */
unsigned taxi_game_audio_cleanup_requests(const uint32_t handles[10],uint32_t stops[10]);
/* Original11490 reset clears nine slots; slot5 (020025FC) is retained. */
void taxi_game_audio_handles_reset(uint32_t handles[10]);
unsigned taxi_game_audio_cleanup_dispatch(const uint32_t handles[10],TaxiGameAudioDispatch,void *context);
#endif
