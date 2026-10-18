#ifndef TAXI_ATTRACT_GRAPHICS_H
#define TAXI_ATTRACT_GRAPHICS_H
#include <stdint.h>
typedef int (*TaxiAttractGraphicsCall)(void *,uint32_t,const uint32_t arguments[6]);
/* Original override-enabled ACC6..AD66 branch only. Three native resource
   services remain caller-bound; no synthetic attract assets are generated. */
int taxi_attract_graphics_prefix(TaxiAttractGraphicsCall,void *);
#endif
