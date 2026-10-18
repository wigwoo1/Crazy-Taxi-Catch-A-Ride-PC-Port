#include "graphics_generation.h"
int taxi_graphics_generation_advance(uint32_t *generation,uint32_t scene,uint32_t transform){if(!generation)return -1;if(!(scene|transform))return 0;*generation+=1u;return 1;}
