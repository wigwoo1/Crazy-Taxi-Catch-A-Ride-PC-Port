#ifndef TAXI_STARTUP_SAVE_OWNED_H
#define TAXI_STARTUP_SAVE_OWNED_H
#include "startup_init_owned.h"
#include "save_load_owned.h"
/* Complete pending10298(1,1) using the same settings/runtime/default-table
   owners as the startup parent. Errors leave the parent suspended. */
int taxi_startup_save_owned(TaxiStartupInitContinuation *,TaxiSaveLoad *,TaxiSaveLoadOwners *);
#endif
