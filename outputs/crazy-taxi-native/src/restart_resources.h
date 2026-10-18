#ifndef TAXI_RESTART_RESOURCES_H
#define TAXI_RESTART_RESOURCES_H
#include "palette_registration.h"
#include "display_mode.h"
#include "graphics_memory_base.h"
typedef struct {TaxiPaletteRegistration *palette;TaxiDisplayMode *display;TaxiGraphicsMemoryBase *bases;} TaxiRestartResources;
/* Native AC70 resource callback; addresses remain source identities only. */
int taxi_restart_resources_call(void *,uint32_t,uint32_t,uint32_t,uint32_t *);
#endif
