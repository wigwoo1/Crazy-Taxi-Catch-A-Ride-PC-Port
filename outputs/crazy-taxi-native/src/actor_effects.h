#ifndef TAXI_ACTOR_EFFECTS_H
#define TAXI_ACTOR_EFFECTS_H
#include "boost_effects.h"
typedef struct {TaxiEffectPool pool;unsigned ring;int32_t timers[4],impact_timer;TaxiBodyPoints body;} TaxiActorEffects;
typedef struct {
 uint8_t actor_flags,material_flags,alternate,action2,phase_e6a,voice_25d4;
 int32_t speed,scale,state_2630,delta,clock;
} TaxiActorEffectInput;
/* Collision217d4 uses the body's preceding-frame points; call before
   rebuilding145e4 points and the four actor-tail emitters plus208d4. */
int taxi_actor_effects_impact(TaxiActorEffects *,unsigned slot,uint8_t alternate,
 int32_t delta,int32_t clock);
int taxi_actor_effects_cold_init(TaxiActorEffects *);
int taxi_actor_effects_session_reset(TaxiActorEffects *);
/* Single-player frame owner: emits for one player then advances the shared
   nine-record pool exactly once. Original normal frame B02C..B044 emits for
   r7=02002634 (initialized AF7E); B094 calls global208D4 after world actor loop.
   Do not invoke this combined step once per traffic actor. Original alternate
   paths can advance the pool without emission; those need separate scheduling. */
int taxi_actor_effects_step(TaxiActorEffects *,const TaxiBodyPoints *,
 TaxiActorEffectInput,TaxiEffectHeight,void *);
int taxi_actor_effects_sprites(const TaxiActorEffects *,TaxiPoint camera,
 int16_t pitch,int16_t yaw,int16_t roll,const TaxiTrig *,int32_t focal,
 int32_t center_x,int32_t center_y,int32_t phase,int jitter_disabled,
 TaxiEffectRandom,void *,TaxiEffectSprite out[9]);
/* Original normalB094 still advances existing effects when B012 movement
   gate skips body reconstruction and owner emitters. Expiry callbacks remain. */
int taxi_actor_effects_advance(TaxiActorEffects *,TaxiActorEffectInput,TaxiEffectHeight,void *);
#endif
