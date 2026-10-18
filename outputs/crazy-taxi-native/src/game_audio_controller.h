#ifndef TAXI_GAME_AUDIO_CONTROLLER_H
#define TAXI_GAME_AUDIO_CONTROLLER_H
#include "game_audio_levels.h"
#include "game_audio_events.h"
#include "game_audio_aux.h"
#include "game_audio_vehicle_loop.h"
#include "game_audio_reward_event.h"
#include "game_audio_turn_loop.h"
#include "game_audio_voice.h"
#include "game_audio_context_voice.h"
typedef struct {
 uint32_t handles[10];int32_t cooldown,amplitude,hold_timer;int16_t factor;
 uint8_t flag0,flag1,pending2,pending3,enabled4,reward_cooldown,last;
 int8_t context_selection,driver_selection,classification;
} TaxiGameAudioController;
typedef struct {
 int32_t speed,max_speed,scale,delta,vehicle564;int16_t rpm,angle578,gate00e4;
 uint8_t muted,ended,outer,intro,frozen,fade,effects,action2,action3,override2620,flags28;
 uint8_t settings,challenge_driver;int8_t driver,current_class,alternate_class;
 uint32_t starts[9];uint8_t busy[9];int32_t random;
} TaxiGameAudioControllerInput;
/* Ordered original10B54 requests; callbacks remain explicit supplied boundaries.
   op9 is one-shot effect(index=handle); op5 action query; op8 RNG query.
   Slot9 is outside this routine and is retained. Max emitted requests32. */
int taxi_game_audio_controller_step(TaxiGameAudioController *,const TaxiGameAudioControllerInput *,TaxiGameAudioEngineRequest out[32],unsigned *count);
/* Immediate callbacks in original branch order. Callback may mutate channel
   state, but must not mutate this controller or its input during dispatch.
   Invalid prefix arithmetic rejects before any callback; output count unchanged. */
int taxi_game_audio_controller_dispatch(TaxiGameAudioController *,const TaxiGameAudioControllerInput *,TaxiGameAudioDispatch,void *,unsigned *count);
/* Original2901C reward callback can change shared25D6 before context voice.
   Read the actual shared selector after op7, including inactive reward paths.
   Read callback is pure; it must not mutate controller/input. NULL retains the
   explicit fixture boundary used by dispatch(). */
typedef int8_t (*TaxiGameAudioSelectionRead)(void *);
int taxi_game_audio_controller_dispatch_shared(TaxiGameAudioController *,const TaxiGameAudioControllerInput *,TaxiGameAudioDispatch,void *,TaxiGameAudioSelectionRead,unsigned *count);
/* Original11490..114EE represented reset. Retains slot5, cooldown2614,
   reward cooldown25D5, last25D8 and classification2610; clears separate25E0.
   Does not stop channels; caller must preserve original cleanup order. */
void taxi_game_audio_controller_reset(TaxiGameAudioController *,int32_t *auxiliary_timer25e0);
#endif
