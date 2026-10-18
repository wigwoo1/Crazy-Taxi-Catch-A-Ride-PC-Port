#ifndef TAXI_BOOT_SERVICES_H
#define TAXI_BOOT_SERVICES_H
#include "boot_memory.h"
/* Original16140->161E0 initializes frame-service flags/callback identity. */
int taxi_boot_timing_init(TaxiBootMemory *);
/* OriginalF69C initializes the32x32 channel scale lookup at02001A10. */
int taxi_boot_shade_init(TaxiBootMemory *);
#endif
