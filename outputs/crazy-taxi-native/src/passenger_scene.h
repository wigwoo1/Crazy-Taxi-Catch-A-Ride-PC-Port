#ifndef TAXI_PASSENGER_SCENE_H
#define TAXI_PASSENGER_SCENE_H
#include "passenger_asset.h"
#include <stdio.h>
void taxi_passenger_pickup_trace(FILE *sink);
void taxi_passenger_fare_trace(FILE *sink);
void taxi_passenger_money_trace(FILE *sink);
#include "passenger_boarding.h"
#include "passenger_idle.h"
#include "world_asset.h"
#include "fare_session.h"
#include "passenger_exit.h"
#include "settlement_display.h"
typedef struct {
    TaxiPassengerState customers[128];TaxiRoutePoint spawns[128];uint8_t destinations[128];
    TaxiPickupState pickup;unsigned count,eligible;int32_t candidate;
    TaxiFareSession session;
    uint8_t render_eligible[128],render_entries_valid; /* Source entry cull survives actor updates. */
    TaxiPassengerMarkerPhase marker_phase; /* Original5F28/29/2A shared actor owner. */
    uint32_t random_state;
    uint8_t use_native_random; /* Explicit partial-game RNG stream. */
} TaxiPassengerScene;
/* Diagnostic orchestration: explicit endpoint and prior-runtime height seeds.
   Spawn Y comes from original height lookup, not source-record Y. The original
   runtime height reset provenance is unresolved; viewer supplies explicit zero.
   Pickup completion starts recovered fare/trip state. Use frame() for connected
   delivery and clocks. Original RNG and sprite fade appearance remain pending. */
int taxi_passenger_scene_init(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,int32_t endpoint_y,int32_t prior_runtime_y);
/* Loading observer follows both native scalar passes after route/height
   resolution. Full original helper scheduling remains a separate dependency. */
int taxi_passenger_scene_init_observed(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,int32_t endpoint_y,int32_t prior_runtime_y,
    TaxiPassengerInitObserver observer,void *context);
/* Normal startup ac80 music selection precedes passenger227ac initialization.
   Carry caller's shared RNG state into a fresh scene, without reseeding. */
int taxi_passenger_scene_init_shared(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,int32_t endpoint_y,int32_t prior_runtime_y,uint32_t random_state,
    TaxiPassengerInitObserver observer,void *context);
/* Live caller-owned state may advance from audio/loading observers. Capture
   after initialization, including when pointer aliases out->random_state. */
int taxi_passenger_scene_init_live(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,int32_t endpoint_y,int32_t prior_runtime_y,const uint32_t *random_state,
    TaxiPassengerInitObserver observer,void *context);
/* Original session reset preserves each customer buffer Y and RNG state.
   Endpoint stack Y remains an explicit unresolved input. */
int taxi_passenger_scene_reinit(TaxiPassengerScene *scene,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,int32_t endpoint_y);
/* Same-city reentry: retain per-customer runtime Y, present both loading
   passes, and capture caller RNG after callbacks even when it aliases out. */
int taxi_passenger_scene_reinit_live(TaxiPassengerScene *,const TaxiPassengerAsset *,
    TaxiWorldAsset *,int32_t,const uint32_t *,TaxiPassengerInitObserver,void *);
/* Caller supplies already initialized canonical fare session before passenger
   passes. Warm customer Y/RNG ownership retained; full227AC helper oracle pending. */
int taxi_passenger_scene_init_prepared_live(TaxiPassengerScene *,const TaxiPassengerAsset *,
    TaxiWorldAsset *,int32_t,int32_t,uint8_t,const TaxiFareSession *,const uint32_t *,TaxiPassengerInitObserver,void *);
int taxi_passenger_scene_tick(TaxiPassengerScene *scene,TaxiRoutePoint taxi,int32_t speed);
typedef struct {
    TaxiPoint taxi;
    int32_t speed,delta,camera_word_10,random_result;
    uint32_t material;
    uint8_t freeze,delivery_gate,select_pressed;
    TaxiCashGlyphs *cash_glyphs; /* Optional caller-owned original collection display. */
    uint8_t fade,start_active; /* Original BA04 entry gates. */
    TaxiRankDisplayOwner *rank_display; /* Retained HUD state, optional scalar-only users. */
} TaxiPassengerFrameInput;
typedef struct {
    uint8_t delivered,timed_out,main_expired,palette_refresh,expiry_fade;
    /* Ordered original destination-selection requests: pickup then removal.
       Post-frame reporting only; exact synchronous panel dispatch remains external. */
    unsigned destination_event_count;int32_t destination_events[2];
} TaxiPassengerFrameEffects;
/* Connected normal-mode diagnostic frame. Exit camera field and RNG supplied
   explicitly; marker, palette/audio dispatch and sprite fade appearance pending. */
typedef int (*TaxiPassengerSelectionObserver)(void *context,
    const TaxiPassengerScene *scene,int32_t destination);
/* Pickup selection observer runs before fare initialization/RNG completion
   and actor updates. External side effects cannot be rolled back on failure.
   Removal (-1) runs before trip-end position/RNG/settlement work. */
/* Early-frame normal clock/timeout service. Caller supplies zeroed or existing
   effects; events accumulate. Current combined wrapper preserves old ordering. */
int taxi_passenger_scene_clock_service_observed(TaxiPassengerScene *,const TaxiPassengerAsset *,TaxiWorldAsset *,const TaxiTrig *,const TaxiPassengerFrameInput *,TaxiPassengerFrameEffects *,TaxiPassengerSelectionObserver,void *);
/* Pickup/delivery/customer service without clock/expiry work. Effects
   accumulate; caller can place this after driving and early clock service. */
int taxi_passenger_scene_late_service_observed(TaxiPassengerScene *,const TaxiPassengerAsset *,TaxiWorldAsset *,const TaxiTrig *,const TaxiPassengerFrameInput *,TaxiPassengerFrameEffects *,TaxiPassengerSelectionObserver,void *);
int taxi_passenger_scene_frame_observed(TaxiPassengerScene *scene,
    const TaxiPassengerAsset *asset,TaxiWorldAsset *world,const TaxiTrig *trig,
    const TaxiPassengerFrameInput *input,TaxiPassengerFrameEffects *effects,
    TaxiPassengerSelectionObserver observer,void *context);
int taxi_passenger_scene_frame(TaxiPassengerScene *scene,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *input,
    TaxiPassengerFrameEffects *effects);
/* Crazy Turn shared passenger services, mode80 only. No ordinary clocks,
   bonus transfer, end RNG or money settlement. Retained16A8 is supplied by
   original Turn HUD owner. Full renderer/scheduler ownership stays caller-owned. */
int taxi_passenger_scene_turn_frame(TaxiPassengerScene *,const TaxiPassengerAsset *,TaxiWorldAsset *,const TaxiTrig *,const TaxiPassengerFrameInput *,TaxiPassengerFrameEffects *,uint8_t *request_rank);
/* Selection/removal observer uses original pre-fare/pre-trip-end order;
   scene state remains atomic, external observer effects cannot roll back. */
int taxi_passenger_scene_turn_frame_observed(TaxiPassengerScene *,const TaxiPassengerAsset *,TaxiWorldAsset *,const TaxiTrig *,const TaxiPassengerFrameInput *,TaxiPassengerFrameEffects *,uint8_t *,TaxiPassengerSelectionObserver,void *);
/* Original mode5 shared pickup/delivery; caller owns Bound timer/failure services. */
int taxi_passenger_scene_bound_frame_observed(TaxiPassengerScene *,const TaxiPassengerAsset *,TaxiWorldAsset *,const TaxiTrig *,const TaxiPassengerFrameInput *,TaxiPassengerFrameEffects *,uint8_t *,TaxiPassengerSelectionObserver,void *);
/* Originalpickup B098 precedes traffic66B4; delivery B4C6 follows it. */
int taxi_passenger_scene_pickup_service_observed(TaxiPassengerScene *,const TaxiPassengerAsset *,TaxiWorldAsset *,const TaxiTrig *,const TaxiPassengerFrameInput *,TaxiPassengerFrameEffects *,TaxiPassengerSelectionObserver,void *);
int taxi_passenger_scene_delivery_service_observed(TaxiPassengerScene *,const TaxiPassengerAsset *,TaxiWorldAsset *,const TaxiTrig *,const TaxiPassengerFrameInput *,TaxiPassengerFrameEffects *,TaxiPassengerSelectionObserver,void *);
/* Inline B098 pickup/fare precedes B31C camera; B36C customer actors follow. */
int taxi_passenger_scene_pickup_transition_observed(TaxiPassengerScene *,const TaxiPassengerAsset *,TaxiWorldAsset *,const TaxiTrig *,const TaxiPassengerFrameInput *,TaxiPassengerFrameEffects *,TaxiPassengerSelectionObserver,void *);
int taxi_passenger_scene_customer_service(TaxiPassengerScene *,const TaxiPassengerFrameInput *);
#endif
