#ifndef TAXI_SCREEN_FLOW_H
#define TAXI_SCREEN_FLOW_H
#include <stdint.h>
typedef struct {int32_t remaining_359c,elapsed;uint8_t fade_1771,fade_1772,child_active_3898;} TaxiScreenFlow;
typedef int (*TaxiScreenFlowCall)(void *,TaxiScreenFlow *,uint32_t target,uint32_t a,uint32_t b,uint32_t c,uint32_t d);
/* Original16542..6654 shared descriptor control after presentation.
   Drawing height must equal160 and fade must be inactive to process timer/input.
   Descriptor flags and callback identity remain caller-owned native asset data. */
int taxi_screen_flow_step(TaxiScreenFlow *,int32_t delta,uint8_t flags,uint32_t callback,
 uint32_t drawing_height,uint8_t fade_active,uint8_t confirm,uint8_t start,uint8_t sound,
 uint16_t bg_mask,uint16_t obj_mask,TaxiScreenFlowCall,void *);
#endif
