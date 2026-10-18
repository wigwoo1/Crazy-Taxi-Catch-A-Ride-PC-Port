#ifndef TAXI_ATTRACT_SETUP_OWNED_H
#define TAXI_ATTRACT_SETUP_OWNED_H
#include "normal_restart_owned.h"
#include "restart_resources.h"
#include "attract_graphics_owned.h"
typedef struct {uint8_t flag_1ea6;uint32_t retained_sl,controller_identity;TaxiNormalRestart prefix;} TaxiAttractSetup;
/* Attract-only AC62..AD66 native composition. Caller already completed ABEC
   prefix and retains settings/city/RNG. Subsequent gameplay setup is pending. */
int taxi_attract_setup_owned(TaxiAttractSetup *,TaxiNormalRestartOwners *,TaxiRestartResources *,TaxiAttractGraphicsOwners *);
#endif
