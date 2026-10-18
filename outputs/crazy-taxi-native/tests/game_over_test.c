#include "../src/game_over.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(void) {
    TaxiGameOver s;uint16_t widths[16]={121,105,91,75,62,50,37,24,11};
    assert(!taxi_game_over_begin(&s,9,240,widths));
    assert(s.position[0]==59*256&&s.current_x==120*256&&s.velocity[0]==-1952);
    TaxiGameOver previous=s;assert(!taxi_game_over_step(&s,1,1));
    assert(s.glyph==previous.glyph&&s.scale==previous.scale&&s.current_x==previous.current_x);
    for(unsigned glyph=0;glyph<9;glyph++) {
        for(unsigned tick=1;tick<=7;tick++) {
            assert(!taxi_game_over_step(&s,60,0));
            assert(s.glyph==glyph&&s.scale==(int)tick*32&&s.steps==tick);
        }
        assert(!taxi_game_over_step(&s,60,0));
        assert(s.glyph==glyph+1&&!s.scale&&!s.steps&&s.current_x==120*256);
    }
    assert(!taxi_game_over_step(&s,60,0)&&s.hold==60);
    assert(taxi_game_over_step(&s,1,0)==1&&s.exiting);
    assert(!taxi_game_over_step(&s,1,0));
    assert(taxi_game_over_begin(&s,17,240,widths)==-1);
    puts("Original Game Over: suffix positions, centered glyph starts, eight-call letter movement/scale, fade gating and strict61-tick exit passed; rendering pending.");
}
