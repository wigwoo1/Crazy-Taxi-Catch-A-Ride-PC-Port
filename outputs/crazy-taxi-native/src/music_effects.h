#ifndef TAXI_MUSIC_EFFECTS_H
#define TAXI_MUSIC_EFFECTS_H
#include <stdint.h>
/* Source274bc..27556: non-row tick effects6/10 change channel+16.
   High nibble has priority; other commands preserve volume. */
uint16_t taxi_music_volume_tick(uint16_t volume,uint8_t command,uint8_t parameter);
#endif
