#ifndef TAXI_MATERIAL_PHASE_H
#define TAXI_MATERIAL_PHASE_H
#include <stdint.h>
/* Original2C858: engine+F4 adds global3B00 when signed byte5F20 is zero.
   Use unsigned addition to retain ARM wrap behavior. Caller owns ordering. */
uint32_t taxi_material_phase_advance(uint32_t phase,int32_t delta,uint8_t freeze);
#endif
