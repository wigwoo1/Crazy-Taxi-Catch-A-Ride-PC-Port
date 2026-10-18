#ifndef TAXI_STARTUP_INIT_H
#define TAXI_STARTUP_INIT_H
#include <stdint.h>
typedef struct {
 uint8_t retained_1ea5,initialized_1ea4,child_signal_3892,selected_font;
 uint32_t dispatcher_3878,font_assets[6];
} TaxiStartupInit;
/* Finite1E1F0..1E24A setup before persistent dispatcher. Native requests
   identify source routines/assets; no ARM code executes in the runtime.
   Screen0 and save(1,1) callbacks may update retained_1ea5. */
typedef int (*TaxiStartupInitCall)(void *,TaxiStartupInit *,uint32_t target,uint32_t argument0,uint32_t argument1);
int taxi_startup_init_begin(TaxiStartupInit *,TaxiStartupInitCall,void *);
#endif
