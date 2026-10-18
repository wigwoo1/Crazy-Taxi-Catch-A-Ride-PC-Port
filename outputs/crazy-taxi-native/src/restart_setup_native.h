#ifndef TAXI_RESTART_SETUP_NATIVE_H
#define TAXI_RESTART_SETUP_NATIVE_H
#include "passenger_setup_owned.h"
#include "normal_restart_flow.h"
#include "vehicle_start_gameplay.h"
#include "runtime_asset.h"
#include "session_init_fare.h"
#include "passenger_scene.h"
#include "traffic_init.h"
#include "game_audio_frame_bridge.h"
#include "pause_menu.h"
#include "effect_pool.h"
typedef struct {
 TaxiDriveWorld *car;TaxiDriveWorldContext *drive;TaxiVehicleStartPacked *vehicle;
 const TaxiRuntimeAsset *runtime;const TaxiSpawnMarker *marker;TaxiVehicleStartPoint resolved;
 TaxiSessionInit *session;TaxiPassengerScene *scene;TaxiPassengerSetupOwned *passengers;
 TaxiTrafficActor *traffic;size_t traffic_capacity;uint32_t *traffic_parameters;uint8_t *traffic_sprites;
 TaxiTrafficGlobals *traffic_globals;uint8_t *traffic_timer;uint32_t *counter_67d0; /* Source reset mirror; consumer semantics stay caller-owned. */
 TaxiGameAudioController *audio;TaxiGameAudioEngine *engine_audio;int32_t *audio_auxiliary;
 TaxiEffectPool *effects;unsigned *effect_ring;TaxiPauseMenu *pause;
 /* Explicit consumer aliases of B6EC's source1696/2618/261C/262C fields. */
 uint8_t *intro_1696,*flag_262c;int32_t *yaw_2618,*yaw_target_261c;
 int32_t retained_endpoint_y;
 /* Opt in after final graphics D764: capture its qualified native EE10 inputs
    once on first SETUP call. Default0 preserves explicit diagnostic inputs. */
 uint8_t use_loading_scratch;
 TaxiNormalRestartFlow *flow;uint8_t index,phase;
} TaxiRestartSetupNative;
/* All eight original AE4A..AE6E SETUP calls with canonical native owners.
   Matches TaxiRestartFlowCall:0 helper returned,1 passenger child pending,-1error.
   Call only for SETUP; caller dispatches prefix/graphics/entry elsewhere.
   Same flow and owners persist across waits. No earlier helper is replayed.
   Source512settings,1684 andNPCcount aliases must agree with passenger owner.
   Retained actor/contact scratch and all resource/music lifetimes stay caller
   supplied unless use_loading_scratch selects the completed native graphics
   D764 producer. That requires both values written by the last mode1/mask1
   fade-to-visible frame, correct generation/owner/kind and idle loading child.
   Only endpointY/material are copied; other contact bytes remain caller-owned.
   Missing/stale/incomplete producer rejects before any helper runs.
   Source exit/channel cleanup precedes this owner;11490 stops none.
   Ordinary override0/mode!=80 only. Invalid domain fails, without a substitute.
   Phase0fresh,1ready/waiting,2executing,3complete,255aborted; genuine restart
   creates a fresh coordinator while retaining its bound game objects. */
int taxi_restart_setup_native(void *,TaxiNormalRestartFlow *,uint32_t,const uint32_t[7],uint32_t *);
/* Delegated227AC cancel hook; inside helper callbacks it is ignored. */
void taxi_restart_setup_native_cancel(TaxiRestartSetupNative *);
#endif
