#include "../src/text_layout.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(void) {
    /* A16px tile with crop2/ink7 advances10px at spacing1. A last
       glyph uses its crop/bounds even when a pair table is registered. */
    const uint16_t m[27]={2,2,0,0,2,0,7,16,0, 1,1,0,0,0,0,5,8,0, 1,1,0,0,0,0,8,8,0};
    const uint8_t pairs[9]={0,6,8,0,0,0,0,0,0};
    assert(taxi_text_correction_q8(m,3,0,1,0,1,256)==6*256);
    assert(taxi_text_correction_q8(m,3,0,1,pairs,1,256)==5*256);
    assert(taxi_text_correction_q8(m,3,0,0,pairs,1,256)==6*256);
    assert(taxi_text_correction_q8(m,3,0,3,pairs,1,256)==0);
    assert(taxi_text_correction_q8(m,3,3,1,0,1,256)==0);
    assert(taxi_text_correction_q8(m,3,2,0,0,3,256)==-3*256);
    assert(taxi_text_correction_q8(m,3,0,1,pairs,1,-128)==-640);
    puts("Original text spacing: null pair table, final glyph crop, invalid indices and signed scale passed; renderer connection pending.");
    return 0;
}
