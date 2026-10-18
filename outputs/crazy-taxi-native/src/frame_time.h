#ifndef TAXI_FRAME_TIME_H
#define TAXI_FRAME_TIME_H
#include <stdint.h>
typedef struct {
 uint32_t previous_3524,elapsed_755c,animation_1e10;
 int32_t delta_3b00;
 uint8_t page_7540;
} TaxiFrameTime;
/* Original263D2..641C: elapsed VBlank ticks, signed clamp1..60,
   previous-counter retention, pause-gated elapsed and presentation counters.
   Caller supplies the VBlank counter3520; this does not emulate hardware. */
int taxi_frame_time_update(TaxiFrameTime *state,uint32_t counter_3520,uint8_t freeze_5f20);
/* Original BA44 phase-1 copy and ordinary B00A movement copy. */
int32_t taxi_frame_time_intro_delta(int32_t prior,int32_t frame,int16_t phase,uint8_t active,uint8_t fade,uint8_t exiting);
int32_t taxi_frame_time_normal_delta(int32_t prior,int32_t frame,uint8_t active,uint8_t freeze,uint8_t ended,uint8_t demo);
#endif
