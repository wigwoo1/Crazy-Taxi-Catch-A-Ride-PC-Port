#include "music_effects.h"
uint16_t taxi_music_volume_tick(uint16_t volume,uint8_t command,uint8_t parameter){
    if(command!=6&&command!=10)return volume;
    unsigned up=parameter>>4,down=parameter&15;
    if(up){uint16_t next=(uint16_t)(volume+up);return next>64?64:next;}
    if(down)return volume<down?0:(uint16_t)(volume-down);
    return volume;
}
