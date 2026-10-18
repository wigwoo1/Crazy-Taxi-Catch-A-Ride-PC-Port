#ifndef TAXI_TIMING_H
#define TAXI_TIMING_H
#include <stdint.h>
/* Original 080263d2..080263f2 signed-wrap delta and [1,60] clamp. */
int32_t taxi_frame_delta(uint32_t current,uint32_t previous);
/* Native scheduling clock: one simulation tick per original VBlank interval.
   Exact ratio 16777216 / 280896 Hz. No emulated hardware or CPU execution.
   Rendering may run independently and interpolate with taxi_clock_alpha.
   Does not choose how many original ticks to batch into one gameplay update. */
typedef struct {uint64_t phase;} TaxiSimulationClock;
int taxi_clock_advance(TaxiSimulationClock *clock,uint64_t elapsed_ns,uint32_t *ticks);
double taxi_clock_alpha(const TaxiSimulationClock *clock);
#endif
