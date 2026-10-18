#ifndef TAXI_TIMING_COMPACT_H
#define TAXI_TIMING_COMPACT_H
#include <stddef.h>
#include <stdint.h>
/* Lossless codec for the existing 74-byte explicit LE timing record.
   Separate from game clock validation and file identity/integrity framing.
   Zero-initialize state at stream start. No padding or game state is imported.
   Failure leaves output, used count and predictor unchanged. */
#define TAXI_TIMING_COMPACT_MAX 173u
typedef struct {uint64_t prior[17];} TaxiTimingCompact;
int taxi_timing_compact_encode(TaxiTimingCompact *,const unsigned char raw[74],
 unsigned char *,size_t capacity,size_t *used);
int taxi_timing_compact_decode(TaxiTimingCompact *,const unsigned char *,size_t,
 unsigned char raw[74],size_t *used);
#endif
