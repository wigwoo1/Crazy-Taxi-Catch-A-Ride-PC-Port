#ifdef NDEBUG
#undef NDEBUG
#endif
#include "music_effects.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    for(unsigned volume=0;volume<65536;volume++)for(unsigned arg=0;arg<256;arg++){
        unsigned high=arg/16,low=arg%16,expected=volume;
        if(high){expected=(volume+high)%65536;if(expected>64)expected=64;}
        else if(low)expected=volume<low?0:volume-low;
        assert(taxi_music_volume_tick((uint16_t)volume,6,(uint8_t)arg)==expected);
        assert(taxi_music_volume_tick((uint16_t)volume,10,(uint8_t)arg)==expected);
    }
    for(unsigned command=0;command<256;command++)if(command!=6&&command!=10)
        assert(taxi_music_volume_tick(37,(uint8_t)command,255)==37);
    puts("PASS original music tick volume slides: all16-bit volumes/parameters, high-nibble precedence, source wrap and command retention.");
    return 0;
}
