#ifndef TAXI_SHADOW_H
#define TAXI_SHADOW_H
#include <stdint.h>
typedef struct {uint16_t palette[256];uint8_t pixels[64*64*32];} TaxiShadow;
/* Original155f6 frame bank. Strict CTX1; failed loads retain caller state. */
int taxi_shadow_load(TaxiShadow *out,const char *path);
/* Index0 keyed; caller supplies current software-renderer palette. */
int taxi_shadow_rgba(const TaxiShadow *shadow,unsigned frame,
    const uint16_t palette[256],uint8_t rgba[64*32*4]);
#endif
