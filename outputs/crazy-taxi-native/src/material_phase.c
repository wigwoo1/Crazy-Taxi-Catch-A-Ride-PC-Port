#include "material_phase.h"
uint32_t taxi_material_phase_advance(uint32_t phase,int32_t delta,uint8_t freeze){
 return freeze?phase:phase+(uint32_t)delta;
}
