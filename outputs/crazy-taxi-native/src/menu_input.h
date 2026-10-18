#ifndef TAXI_MENU_INPUT_H
#define TAXI_MENU_INPUT_H
#include <stdint.h>
typedef struct {
    uint8_t held[10],previous[10],repeat[10],pressed[10];
    int16_t timer[10];
    uint16_t pressed_mask;
} TaxiMenuInput;
/* Normal physical-input branch of08015c3c. Active-high native mask uses
   original GBA bit order A,B,Select,Start,Right,Left,Up,Down,R,L.
   Recorded input and special mode overrides remain caller responsibilities. */
void taxi_menu_input_step(TaxiMenuInput *state,uint16_t held_mask,int32_t delta);
/* Source15c96..15d0e: gate1eaa suppresses physical held bits; recorded
   mode1708 overrides all except A/Start using active-high halfword170c. */
uint16_t taxi_menu_input_source_mask(uint16_t physical,uint8_t gate,
    uint8_t recorded_mode,uint16_t recorded);
#endif
