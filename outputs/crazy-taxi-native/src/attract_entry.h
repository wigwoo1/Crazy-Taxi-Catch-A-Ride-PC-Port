#ifndef TAXI_ATTRACT_ENTRY_H
#define TAXI_ATTRACT_ENTRY_H
#include <stdint.h>
/* Original ABEC..AC62 attract-only prefix; common gameplay setup is pending. */
typedef struct {uint8_t settings[8],flag_153c,flag_1697,saved_city;uint32_t scene_identity;} TaxiAttractEntry;
int taxi_attract_entry(TaxiAttractEntry *,uint8_t override_enabled,uint32_t *random);
#endif
