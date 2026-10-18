#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "../src/menu_input.h"
int main(void) {
    for(unsigned key=0;key<10;key++) {
        TaxiMenuInput s={0};uint16_t mask=(uint16_t)(1u<<key);
        taxi_menu_input_step(&s,mask,1);
        assert(s.held[key]&&!s.previous[key]&&s.pressed[key]&&s.repeat[key]);
        assert(s.timer[key]==60&&s.pressed_mask==mask);
        for(unsigned i=0;i<59;i++) {
            taxi_menu_input_step(&s,mask,1);
            assert(!s.pressed_mask&&!s.pressed[key]&&!s.repeat[key]);
        }
        assert(s.timer[key]==1);
        taxi_menu_input_step(&s,mask,1);assert(s.repeat[key]&&s.timer[key]==30&&!s.pressed[key]);
        for(unsigned i=0;i<29;i++){taxi_menu_input_step(&s,mask,1);assert(!s.repeat[key]);}
        taxi_menu_input_step(&s,mask,1);assert(s.repeat[key]&&s.timer[key]==30);
        taxi_menu_input_step(&s,0,1);assert(!s.held[key]&&s.previous[key]&&!s.repeat[key]&&!s.timer[key]);
        taxi_menu_input_step(&s,mask,1);assert(s.pressed_mask==mask&&s.timer[key]==60);
        taxi_menu_input_step(&s,mask,61);assert(!s.pressed_mask&&s.repeat[key]&&s.timer[key]==30);
    }
    TaxiMenuInput s={0};taxi_menu_input_step(&s,0xffff,1);assert(s.pressed_mask==1023);
    taxi_menu_input_step(&s,0x31,1);assert(!s.pressed_mask&&s.held[0]&&s.held[4]&&s.held[5]&&!s.held[1]);
    /* Halfword subtraction wraps before the signed threshold comparison. */
    s.timer[0]=1;taxi_menu_input_step(&s,0x31,32770);assert(s.timer[0]==32767&&!s.repeat[0]);
    puts("Native menu input: all10 keys, held/previous/fresh-press banks,60/30 repeat boundaries, release/repress, overshoot and signed halfword wrap passed.");
    return 0;
}
