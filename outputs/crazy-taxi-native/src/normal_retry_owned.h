#ifndef TAXI_NORMAL_RETRY_OWNED_H
#define TAXI_NORMAL_RETRY_OWNED_H
#include "normal_exit.h"
#include "pause_menu.h"
#include "ordinary_loading_pending.h"
#include "session_init.h"
#include "passenger_scene.h"
#include "game_audio_controller.h"
#include "audio_channel.h"
#include "audio_settings_state.h"
#include "music_tracker.h"
typedef struct {
 TaxiPauseMenu *pause;TaxiOrdinaryLoadingPending *loading;TaxiSessionInit *session;TaxiPassengerScene *scene;
 TaxiGameAudioController *sounds;TaxiAudioChannel *channels;TaxiAudioSettingsState *audio;TaxiMusicTracker *tracker;
 const uint16_t *output_rate;const uint8_t *override_1708;uint8_t *exit_5f1a;
 TaxiNormalExit parent;TaxiNormalExitRequests requests;unsigned stops;uint8_t phase,executing;
} TaxiNormalRetryOwned;
/* Ordinary21C5C Yes -> E5A8(0,32,201,201), then B580 cleanup -> terminalAC70.
   Fresh zero coordinator over persistent canonical native owners. This does
   not run frames, replace input, start AC70, or return to the old game loop.
   Call fade once for an actual Retry fade_out event; returns0/-1.
   Transfer returns0 while fade/publication is pending,2 at terminalAC70,
   -1 invalid/failed. Repeated2 retains state, never repeats cleanup. Caller
   starts actual native restart on that transfer, replacing this coordinator
   only on genuine new game entry. Source frozen/outer/handle slots remain
   retained until their actual restart helpers write them. Quit is separate.
   Sound handles are native1..4 identities; output_rate is original07564. */
int taxi_normal_retry_owned_fade(TaxiNormalRetryOwned *,const TaxiPauseEvents *);
int taxi_normal_retry_owned_transfer(TaxiNormalRetryOwned *);
/* Pause Quit is a separate continuation over the same canonical owners.
   Requires outer2/exit_flag1 from actual Yes. Same201 fade and source cleanup
   order, transfer returns1 once cleanup is terminal (idempotent), no AC70.
   prepare:2 ONCE for actual243A4 credits,0 pending,1 menu callback ready.
   resume waits unchanged on returned0, commits only real child return. Credits
   leave frozen set until completion. Natural-expiry results are not this API.
   phase0 fresh/1 fading/2 cleaned/3 credits/4 menu-ready/255 aborted. */
int taxi_normal_quit_owned_fade(TaxiNormalRetryOwned *,const TaxiPauseEvents *);
int taxi_normal_quit_owned_transfer(TaxiNormalRetryOwned *);
int taxi_normal_quit_owned_prepare(TaxiNormalRetryOwned *);
int taxi_normal_quit_owned_resume(TaxiNormalRetryOwned *,int credits_returned);
/* Natural expiry observes actual Scene outer2/ended0 and exit0/unfrozen.
   Starts original E5A8(0,32,1,1), then shares source cleanup ordering.
   This does not fabricate a pause event or finish asynchronous result children. */
int taxi_normal_expiry_owned_fade(TaxiNormalRetryOwned *);
int taxi_normal_expiry_owned_transfer(TaxiNormalRetryOwned *);
#endif
