#ifndef TAXI_LOADING_INDEXED_OWNED_H
#define TAXI_LOADING_INDEXED_OWNED_H
#include "loading_strings_owned.h"
typedef struct {TaxiLoadingStringsOwner strings;uint8_t **frame;size_t frame_size;const uint32_t *fill;} TaxiLoadingIndexedOwner;
/* Shared backbuffer pointer/fill are re-read at each native clear/image.
   Text/sprite/presentation children remain explicit caller-owned services. */
int taxi_loading_indexed_owned(TaxiLoadingPresent *,const TaxiLoadingIndexedOwner *);
#endif
