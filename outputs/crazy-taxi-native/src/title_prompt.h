#ifndef TAXI_TITLE_PROMPT_H
#define TAXI_TITLE_PROMPT_H
#include <stdint.h>
typedef struct {int32_t x,y,z;uint8_t frame,group,priority,bucket,flags,palette;} TaxiTitlePrompt;
/* Original166b8 request portion only. Entry timer before update; clock755c.
   Initialization/sound/input/state transition remain owned by startup flow. */
int taxi_title_prompt(int32_t timer,uint32_t clock,uint8_t fade,
 uint16_t width,uint16_t height,TaxiTitlePrompt output[4]);
#endif
