#ifndef TAXI_TITLE_CALLBACK_H
#define TAXI_TITLE_CALLBACK_H
#include "title_scroll.h"
#include "title_prompt.h"
typedef struct {uint8_t sound_done;TaxiTitleScroll scroll;} TaxiTitleCallback;
typedef struct {int sound_index;unsigned request_count;TaxiTitlePrompt requests[4];} TaxiTitleCallbackOutput;
/* Original166b8..167b2; caller owns shared source RNG and clock. */
int taxi_title_callback(TaxiTitleCallback *state,uint32_t *random,int sound,
 int32_t delta,uint32_t clock,uint8_t fade,uint16_t width,uint16_t height,
 TaxiTitleCallbackOutput *output);
#endif
