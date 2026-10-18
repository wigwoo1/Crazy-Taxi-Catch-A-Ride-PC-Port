#ifndef TAXI_CREDITS_RUNTIME_H
#define TAXI_CREDITS_RUNTIME_H
#include "credits_flow.h"
#include "palette_fade.h"
typedef struct {TaxiCreditsFlow flow;TaxiPaletteFade fade;uint16_t scroll[3],bldy;uint8_t exiting,complete;} TaxiCreditsRuntime;
typedef struct {TaxiCreditsFrame frame;unsigned row;uint8_t running,stop_music,returned;} TaxiCreditsRuntimeFrame;
/* Caller performs243A4 music/layer/palette setup first; retained fade cadence
   is explicit. This wrapper covers4458..446C running gates, running frame,
   source26318 fade-service boundary and4600..4626 return/scroll/music stop. */
void taxi_credits_runtime_begin(TaxiCreditsRuntime *,uint8_t interval,uint8_t reload);
int taxi_credits_runtime_frame(TaxiCreditsRuntime *,int32_t delta,int32_t sine,
 const uint8_t line[31],uint8_t effects,int confirm,int start,TaxiCreditsRuntimeFrame *);
#endif
