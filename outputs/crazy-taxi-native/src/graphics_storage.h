#ifndef TAXI_GRAPHICS_STORAGE_H
#define TAXI_GRAPHICS_STORAGE_H
#include <stdint.h>
/* Original2AC6C stores/loops AE5A..AEF2. Source addresses identify owners,
   never native pointers. Normal/special pools hold persistent polygon generation stamps.
   heads_2b6ec is retained initialization storage with unqualified consumers. */
typedef struct {
 uint32_t state_6844,background_6850,state_6904;
 uint32_t pool_1b6ec[12288],pool_276ec[4096],heads_2b6ec[128];
} TaxiGraphicsStorage;
int taxi_graphics_storage_reset(TaxiGraphicsStorage *);
#endif
