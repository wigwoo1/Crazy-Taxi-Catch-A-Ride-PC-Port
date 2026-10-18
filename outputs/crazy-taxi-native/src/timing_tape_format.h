#ifndef TAXI_TIMING_TAPE_FORMAT_H
#define TAXI_TIMING_TAPE_FORMAT_H
#include "timing_tape.h"
/* Portable diagnostic format v1: explicit little-endian fields, CRC32.
   Identity is four SHA256 digests supplied by the owner: ROM, build, options,
   initial state. This module compares identity, it does not compute it.
   CRC detects accidental corruption, not malicious modification.
   Buffers must not overlap. Encoding requires a finished successful tape.
   Decoding is atomic: rejected data changes no records or output count.
   Owner must separately check session/input completeness and preserve failures. */
#define TAXI_TIMING_ID_BYTES 128
int taxi_timing_format_size(size_t count,size_t *bytes);
int taxi_timing_format_encode(const TaxiTimingTape *,const unsigned char identity[128],
                             unsigned char *data,size_t size);
int taxi_timing_format_decode(const unsigned char *data,size_t size,
                             const unsigned char expected_identity[128],
                             TaxiTimingRecord *records,size_t capacity,size_t *count);
/* Single explicit v1 record, with native clock arithmetic validation. */
int taxi_timing_record_encode(const TaxiTimingRecord *,unsigned char raw[74]);
int taxi_timing_record_decode(const unsigned char raw[74],TaxiTimingRecord *);
int taxi_timing_format_validate(const unsigned char *,size_t,const unsigned char id[128],size_t *count);
#endif