#ifndef TAXI_TITLE_FLOW_H
#define TAXI_TITLE_FLOW_H
#include <stdint.h>
typedef struct {int32_t remaining,elapsed;uint8_t menu_requested;} TaxiTitleFlow;
typedef struct {uint8_t begin_exit_fade;int sound_index;} TaxiTitleActions;
/* Original16250 type5/16542..16646: flags3, timeout900.
   Call after title callback, with original fade byte and drawing-complete flag. */
void taxi_title_flow_init(TaxiTitleFlow *state);
int taxi_title_flow_step(TaxiTitleFlow *state,int32_t delta,uint8_t fade,
 int drawing_complete,uint8_t confirm,uint8_t start,int sound,TaxiTitleActions *actions);
#endif
