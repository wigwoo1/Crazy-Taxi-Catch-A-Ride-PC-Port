#ifndef TAXI_LOADING_STRINGS_OWNED_H
#define TAXI_LOADING_STRINGS_OWNED_H
#include "loading_present.h"
#include <stddef.h>
typedef struct {const uint8_t *rom;size_t rom_size;const uint32_t *selected_table;TaxiLoadingPresentCall service;void *context;} TaxiLoadingStringsOwner;
/* Bind original28FE4 to shared resolved selected table; every remaining
   native child retains its caller-owned service and mutable loading state. */
int taxi_loading_strings_owned(TaxiLoadingPresent *,const TaxiLoadingStringsOwner *);
#endif
