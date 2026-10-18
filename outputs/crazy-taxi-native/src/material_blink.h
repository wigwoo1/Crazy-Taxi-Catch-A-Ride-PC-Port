#ifndef TAXI_MATERIAL_BLINK_H
#define TAXI_MATERIAL_BLINK_H
#include <stdint.h>
#include <stdio.h>
void taxi_material_selection_trace(FILE *sink);
#include <stddef.h>
typedef struct {
    uint8_t cache[512];uint32_t clock;int32_t destination;
    uint16_t indices[4];int has_record;
} TaxiMaterialBlink;
typedef int (*TaxiMaterialPanel)(void *context,const uint8_t record[48],
    const uint8_t cache[512]);
/* Explicit selection event, including repeated selection. Source df9c sets
   selection, optionally calls panel, then clears cache; clock is retained.
   Callback failure retains selected state/cache, so caller must handle it. */
int taxi_material_blink_select(TaxiMaterialBlink *state,int32_t destination,
    const uint8_t *record,size_t size,uint8_t runtime_mode,
    TaxiMaterialPanel panel,void *context);
int taxi_material_blink_update(TaxiMaterialBlink *state,uint32_t delta,int frozen);
/* Original dec8..df7e. Record material indices are destination +40/42/44/46.
   Missing destination still advances the unfrozen clock. Invalid host index
   returns -1 before mutation; original valid ROM indices stay in512 bytes. */
int taxi_material_blink_step(uint8_t cache[512], uint32_t *clock,
    uint32_t delta, int frozen, int32_t destination,
    const uint16_t indices[4]);
#endif
