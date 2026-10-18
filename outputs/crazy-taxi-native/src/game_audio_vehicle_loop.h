#ifndef TAXI_GAME_AUDIO_VEHICLE_LOOP_H
#define TAXI_GAME_AUDIO_VEHICLE_LOOP_H
#include "game_audio_engine.h"
typedef struct {int32_t vehicle564;uint8_t flags28,frozen,ended,fade,busy;uint32_t start;} TaxiGameAudioVehicleLoopInput;
/* Original10F10..10FB6 slot4 loop58: retain stale handle when busy query
   is false on blocked path; existing eligible handle emits no volume update. */
unsigned taxi_game_audio_vehicle_loop(uint32_t *handle,const TaxiGameAudioVehicleLoopInput *,TaxiGameAudioEngineRequest out[3]);
unsigned taxi_game_audio_vehicle_loop_dispatch(uint32_t *handle,const TaxiGameAudioVehicleLoopInput *i,TaxiGameAudioDispatch call,void *context);
#endif
