#ifndef TAXI_EFFECT_PRESENTATION_INTERPOLATION_H
#define TAXI_EFFECT_PRESENTATION_INTERPOLATION_H
#include "effect_presentation_slots.h"
/* Native screen-space presentation extension. Positive matching generations
   must come from observed emission events, not inferred record age. Source
   after-sample visibility, frame, jitter and flags stay discrete. No RNG. */
TaxiEffectSprite taxi_effect_presentation_interpolate(const TaxiEffectPresentationSlot *before,const TaxiEffectPresentationSlot *after,uint64_t before_generation,uint64_t after_generation,double alpha,int *blended);
#endif
