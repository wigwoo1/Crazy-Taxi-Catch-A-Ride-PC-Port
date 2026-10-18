#ifndef TAXI_TIMING_TAPE_H
#define TAXI_TIMING_TAPE_H
#include "host_time.h"
#include <stddef.h>
/* Diagnostic only. Caller owns storage and serializes fields explicitly.
   No padding bytes are compared. Site tags must identify each call boundary.
   Replay still executes the existing per-tick native service. This is not a
   full-session replay: input/state/asset identity must be checked by its owner. */
typedef struct {
 uint32_t site,ticks;
 uint64_t ns;
 TaxiHostTime before,after;
} TaxiTimingRecord;
typedef struct {
 TaxiTimingRecord *records;
 size_t count,capacity,cursor;
 int replay,failed;
} TaxiTimingTape;
int taxi_timing_tape_init(TaxiTimingTape *,TaxiTimingRecord *,size_t capacity,
                         size_t count,int replay);
/* Fatal errors latch. Never retry after callbacks: they cannot be rolled back.
   Invalid record/pre-state rejects before any clock mutation or callback.
   In replay wall_ns is ignored; recorded ns is passed to the normal clock. */
int taxi_timing_tape_advance(TaxiTimingTape *,uint32_t site,TaxiHostTime *,
                            uint64_t wall_ns,TaxiHostTickService,void *,uint32_t *);
int taxi_timing_tape_finish(const TaxiTimingTape *);
#endif