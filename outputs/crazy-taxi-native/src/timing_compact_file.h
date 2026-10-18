#ifndef TAXI_TIMING_COMPACT_FILE_H
#define TAXI_TIMING_COMPACT_FILE_H
#include <stddef.h>
#include <stdint.h>
/* CTCLOCK2, version2, flags1, count64, payloadbytes64, identity128;
   160-byte header, compact records, CRC32 trailer. All integers LE.
   Caller provides buffer with payload at offset160 before sealing.
   No allocation or expansion; all records are validated before success.
   Identity is owner supplied; CRC detects corruption, not authentication. */
#define TAXI_COMPACT_HEADER 160u
#define TAXI_COMPACT_OVERHEAD 164u
int taxi_compact_file_seal(unsigned char *,size_t,const unsigned char id[128],uint64_t count);
int taxi_compact_file_validate(const unsigned char *,size_t,const unsigned char id[128],uint64_t *count);
#endif
