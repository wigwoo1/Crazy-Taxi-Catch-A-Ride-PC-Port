#ifndef TAXI_BOOT_MEMORY_H
#define TAXI_BOOT_MEMORY_H
#include <stdint.h>
#include <stddef.h>
typedef struct {uint8_t external[262144],internal[32768];uint16_t wait_control;} TaxiBootMemory;
/* Original1E2E4..1E33E prefix: cold clears external RAM/all but top512
   internal bytes; warm retains RAM except physical-input gate1EAA. */
int taxi_boot_memory_prepare(TaxiBootMemory *);
/* Original2DB54 source signed16 ASR4 sine/cosine RAM tables. */
int taxi_boot_trig_load(TaxiBootMemory *,const uint8_t *,size_t);
#endif
