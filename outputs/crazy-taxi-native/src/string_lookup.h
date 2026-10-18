#ifndef TAXI_STRING_LOOKUP_H
#define TAXI_STRING_LOOKUP_H
#include <stddef.h>
#include <stdint.h>
/* Original28FE4 after selected table resolution: null table or unsigned
   index>inclusive table maximum returns084A0CE8. Identities remain metadata.
   Invalid host ROM ranges return-1; no synthesized strings. */
int taxi_string_lookup(const uint8_t *rom,size_t size,uint32_t table,uint32_t index,uint32_t *identity);
#endif
