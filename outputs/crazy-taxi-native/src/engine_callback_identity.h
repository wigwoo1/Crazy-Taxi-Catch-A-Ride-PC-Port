#ifndef TAXI_ENGINE_CALLBACK_IDENTITY_H
#define TAXI_ENGINE_CALLBACK_IDENTITY_H
#include <stdint.h>
/* Original2AC6C callback publication identities, without relocating or
   executing ROM code. Zero means unbound; callers must reject it. */
uint32_t taxi_engine_callback_identity(uint32_t global_owner);
#endif
