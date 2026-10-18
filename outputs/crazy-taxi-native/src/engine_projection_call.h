#ifndef TAXI_ENGINE_PROJECTION_CALL_H
#define TAXI_ENGINE_PROJECTION_CALL_H
#include <stdint.h>
/* Native operation reached through original global03006940. Unsupported
   bindings fail before writing output; no ROM execution or function casts. */
int taxi_engine_projection_call(uint32_t global_owner,uint32_t numerator,uint32_t denominator,uint32_t *result);
#endif
