#ifndef TAXI_SCREEN_SEQUENCE_H
#define TAXI_SCREEN_SEQUENCE_H
#include "screen_flow.h"
typedef struct {
 uint32_t image,timer,background,callback;
 uint16_t fade_in_bg,fade_in_obj,fade_out_bg,fade_out_obj;
 int8_t type;
 uint8_t flags;
} TaxiScreenDescriptor;
typedef struct {
 TaxiScreenFlow flow;
 uint32_t descriptor_slot;
 uint16_t index;
 uint32_t audio_755c,callback_3570;
 uint8_t state_3568,state_356a,animation_3574;
 uint16_t count_356c,limit_356e;
} TaxiScreenSequence;
/* Routine identities tag native service contracts; they are never executed ARM. */
typedef int (*TaxiScreenSequenceCall)(void *,TaxiScreenSequence *,TaxiScreenDescriptor *,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
/* Actual16390..163D4: owned per-descriptor setup before pixel-stream preparation. */
int taxi_screen_sequence_prepare(TaxiScreenSequence *,TaxiScreenDescriptor *,TaxiScreenSequenceCall,void *);
/* Actual16660..1668C, called only after remaining<=0 and fade inactive.
   Advance descriptor/index, stop effects, then inspect the next descriptor.
   Returns1 for next screen,0 for terminal null-image/type-1 sentinel,-1 on error. */
int taxi_screen_sequence_advance(TaxiScreenSequence *,TaxiScreenDescriptor *,TaxiScreenSequenceCall,void *);
#endif
