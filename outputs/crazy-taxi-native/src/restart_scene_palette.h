#ifndef TAXI_RESTART_SCENE_PALETTE_H
#define TAXI_RESTART_SCENE_PALETTE_H
#include <stdint.h>
#include <stddef.h>
/* Original AD66 scene owner+12, city*104, descriptor+24 palette identity.
   Copies immutable source colors into native storage; no engine relocation. */
int taxi_restart_scene_palette(const uint8_t *,size_t,uint8_t city,uint32_t *descriptor,uint32_t *identity,uint16_t [256]);
#endif
