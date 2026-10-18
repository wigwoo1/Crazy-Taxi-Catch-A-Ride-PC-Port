#ifndef TAXI_IMPACT_VOICE_H
#define TAXI_IMPACT_VOICE_H
#include "random.h"
/* Source0802a90a..a928: signed active-trip byte, one inclusive random
   draw, voice020025d6=draw%3+6. A request only; no audio playback. */
static inline int taxi_impact_voice(int collided,uint8_t active,uint32_t *rng,uint8_t *voice){
    if(!collided||!active)return 0;
    *voice=(uint8_t)(taxi_random_range(rng,0,65535)%3+6);
    return 1;
}
#endif
