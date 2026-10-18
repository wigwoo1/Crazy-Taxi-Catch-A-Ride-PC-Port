#ifndef TAXI_GAME_AUDIO_FRAME_BRIDGE_H
#define TAXI_GAME_AUDIO_FRAME_BRIDGE_H
#include "game_audio_controller.h"
#include "drive_world.h"
#include "traffic_effects.h"
/* At originalAFB6, import recovered owners before passengers/current input.
   Input retains caller-owned gates/actions/settings/driver and external returns.
   Controller retains handles, event flags, cooldown/factor and last class.
   Transactional rejection for invalid passenger references. */
int taxi_game_audio_frame_import(TaxiGameAudioController *,TaxiGameAudioControllerInput *,const TaxiDriveWorld *,const TaxiDriveWorldContext *,const TaxiPassengerScene *,const TaxiTrafficGlobals *,uint8_t timer25d5);
/* Commit the shared source bytes after dispatch and before passenger services.
   No audio callbacks or sound allocation occur in this bridge. */
void taxi_game_audio_frame_commit(const TaxiGameAudioController *,TaxiDriveWorldContext *,TaxiPassengerScene *,TaxiTrafficGlobals *,uint8_t *timer25d5);
#endif
