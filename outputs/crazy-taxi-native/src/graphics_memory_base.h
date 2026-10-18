#ifndef TAXI_GRAPHICS_MEMORY_BASE_H
#define TAXI_GRAPHICS_MEMORY_BASE_H
#include <stdint.h>
/* Original7378/73C0 globals0200152C/1530. Address identities are data;
   no host pointer dereference or graphics-memory allocation implied. */
typedef struct {uint32_t base_152c,base_1530;} TaxiGraphicsMemoryBase;
int taxi_graphics_memory_base_first(TaxiGraphicsMemoryBase *,uint8_t);
int taxi_graphics_memory_base_second(TaxiGraphicsMemoryBase *,uint8_t);
#endif
