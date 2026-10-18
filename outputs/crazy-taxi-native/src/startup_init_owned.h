#ifndef TAXI_STARTUP_INIT_OWNED_H
#define TAXI_STARTUP_INIT_OWNED_H
#include "startup_init.h"
#include "input.h"
/* Caller-owned native state for the recovered defaults and input-reset services.
   Screen0 and save(1,1) remain explicit child lifecycles. */
typedef struct {
 uint8_t *settings,*runtime_rows;const uint8_t *first_ten;
 TaxiInput *input;uint8_t *music_volume,*effects_volume,*language;
} TaxiStartupInitOwners;
typedef int (*TaxiStartupInitChild)(void *,TaxiStartupInit *,uint32_t target,uint32_t argument0,uint32_t argument1);
int taxi_startup_init_owned(TaxiStartupInit *,const TaxiStartupInitOwners *,TaxiStartupInitChild,void *);
/* A pending child keeps initialization suspended while the host renders frames.
   The child may mutate init/owners before resume. Zero means fully initialized. */
typedef enum {TAXI_STARTUP_INIT_DONE=0,TAXI_STARTUP_INIT_SCREEN=1,TAXI_STARTUP_INIT_SAVE=2} TaxiStartupInitPending;
typedef struct {TaxiStartupInit *init;TaxiStartupInitOwners owners;TaxiStartupInitPending pending;} TaxiStartupInitContinuation;
int taxi_startup_init_owned_begin(TaxiStartupInitContinuation *,TaxiStartupInit *,const TaxiStartupInitOwners *);
int taxi_startup_init_owned_resume(TaxiStartupInitContinuation *);
#endif
