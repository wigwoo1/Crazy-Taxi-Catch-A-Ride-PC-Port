#ifndef TAXI_SAVE_LOAD_H
#define TAXI_SAVE_LOAD_H
#include <stdint.h>
/* Original10298 logical owners; source identities are metadata, never host pointers. */
typedef struct {
 uint8_t *settings,*runtime_rows;const uint8_t *first_ten;
 uint8_t *device_block; /* Current 8-byte block within settings; read/verify only. */
 uint8_t preserve_settings,music_enabled,effects_enabled,music_volume,effects_volume,language;
 /* interrupt_callback is source02003548 timer metadata, not frame352C. */
 uint32_t current_clock,previous_clock,delta,interrupt_callback;uint16_t wait_control;
 uint32_t music_module;uint8_t start_order; /* Caller-resolved immutable ROM selection. */
 unsigned sectors_checked,attempts;
} TaxiSaveLoad;
/* Device/audio/wait services may update shared owners and settings. Return zero
   on host success; result holds the original low16 device verification status. */
typedef int (*TaxiSaveLoadService)(void *,TaxiSaveLoad *,uint32_t target,uint32_t argument0,uint32_t argument1,uint32_t *result);
int taxi_save_load(TaxiSaveLoad *,uint32_t slot,uint32_t fallback_defaults,TaxiSaveLoadService,void *);
#endif
