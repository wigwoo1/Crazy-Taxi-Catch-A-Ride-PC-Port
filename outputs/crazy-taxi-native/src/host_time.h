#ifndef TAXI_HOST_TIME_H
#define TAXI_HOST_TIME_H
#include "timing.h"
#include "frame_time.h"
/* Host clock advancement never performs gameplay or presentation work.
   Commit only at the recovered presentation boundary, after helper work.
   This clock does not prescribe original workload-dependent update cadence. */
typedef struct {TaxiSimulationClock clock;uint32_t counter;TaxiFrameTime frame;} TaxiHostTime;
int taxi_host_time_advance(TaxiHostTime *,uint64_t elapsed_ns,uint32_t *ticks);
/* Deliver one native service immediately after each successive source counter
   increment, including wrap and catch-up ticks. The service must not change
   the host clock or commit presentation. Invalid time/input invokes no service
   and leaves the clock unchanged. A fatal service error must not be retried.
   This exposes source frame order; it does not choose intra-helper IRQ timing. */
typedef void (*TaxiHostTickService)(void *,uint32_t counter);
int taxi_host_time_advance_serviced(TaxiHostTime *,uint64_t elapsed_ns,
    TaxiHostTickService,void *context,uint32_t *ticks);
int taxi_host_time_present(TaxiHostTime *,uint8_t frozen);
/* Host adaptation:0 means no new tick;1 means committed;-1 invalid. */
int taxi_host_time_present_due(TaxiHostTime *,uint8_t frozen);
/* Caller-controlled commit boundary, minimum1..60 ticks. Deferral leaves every
   state byte unchanged. This does not choose a source-faithful update policy. */
int taxi_host_time_present_after_ticks(TaxiHostTime *,uint8_t frozen,uint32_t minimum_ticks);
#endif

