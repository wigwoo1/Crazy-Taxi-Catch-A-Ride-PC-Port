#ifndef TAXI_EFFECT_PRESENTATION_SLOTS_H
#define TAXI_EFFECT_PRESENTATION_SLOTS_H
#include "effect_sprite.h"
typedef struct {TaxiEffectRecord record;TaxiEffectSprite sprite,unjittered;uint8_t visible;} TaxiEffectPresentationSlot;
/* Associate already-produced source sprites with stable pool indices.
   Visibility-only projection disables jitter; recorded sprite/RNG choices
   remain authoritative. No effect update or RNG callback occurs. */
int taxi_effect_presentation_slots(const TaxiEffectRecord records[9],TaxiPoint camera,int16_t pitch,int16_t yaw,int16_t roll,const TaxiTrig *,int32_t phase,const TaxiEffectSprite *requests,unsigned count,TaxiEffectPresentationSlot out[9]);
#endif
