#ifndef TAXI_SESSION_INIT_H
#define TAXI_SESSION_INIT_H
#include <stdint.h>
/* Native data owners for complete B6EC. Byte arrays preserve unrelated
   packed fields exactly; no hardware/ROM addresses are dereferenced. */
typedef struct {
 uint8_t shared_1534[8],session_165e[174],sound_2618[24],effect_5f20[12],projection_5310[6],scroll_7530[16];
 uint8_t settings[512],snapshot[512],outer_1ea8;
 uint16_t reset_430c;uint32_t counter_67d0;int effect_slots[12];
} TaxiSessionInit;
int taxi_session_init(TaxiSessionInit *,uint16_t width);
#endif
