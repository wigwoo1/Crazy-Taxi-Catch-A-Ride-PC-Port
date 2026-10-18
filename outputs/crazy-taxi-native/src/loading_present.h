#ifndef TAXI_LOADING_PRESENT_H
#define TAXI_LOADING_PRESENT_H
#include "loading_present_bypass.h"
typedef struct {TaxiLoadingPresentGate gate;uint8_t animation_1684;} TaxiLoadingPresent;
/* Nine original register/stack arguments; position replaces args[2] source
   stack pointer for28288. Callback may mutate position and shared owners.
   Lookup returns a source string identity, never a host pointer. */
typedef int (*TaxiLoadingPresentCall)(void *,TaxiLoadingPresent *,uint32_t,const uint32_t[9],uint32_t position[3],uint32_t *result);
int taxi_loading_present(TaxiLoadingPresent *,TaxiLoadingPresentCall,void *);
#endif
