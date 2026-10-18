#ifndef TAXI_TITLE_SCROLL_H
#define TAXI_TITLE_SCROLL_H
#include <stdint.h>
typedef struct {int32_t timer,x_q8;} TaxiTitleScroll;
/*166fe..671a +5f10..f88, title descriptor Q8 coefficient256.
  Source callback motion runs before prompt gate, independent of fade. */
int taxi_title_scroll_step(TaxiTitleScroll *state,int32_t delta,uint16_t viewport_width);
#endif
