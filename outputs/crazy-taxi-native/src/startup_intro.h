#ifndef TAXI_STARTUP_INTRO_H
#define TAXI_STARTUP_INTRO_H
#include "startup_animation.h"
#include "startup_obj_setup.h"
typedef struct {
 TaxiStartupAnimation animation;TaxiStartupObjQueue queue;TaxiStartupObjTransport transport;
 uint16_t base_template[128][4];TaxiObjMatrixSlot matrix_template[32];
 uint16_t metrics[33][9],palette[256],width,height,obj_mask;uint32_t tile_base;
 uint16_t scroll,blend,alpha,brightness;uint32_t fade_request[4];unsigned fades,accepted,clipped;
} TaxiStartupIntro;
/* Caller owns source resource, reset-template initialization, fade and display. */
int taxi_startup_intro_animate(TaxiStartupIntro *,uint8_t fade_active,uint8_t fade_mode,uint8_t confirm,uint8_t start,uint16_t fade_bg,uint16_t fade_obj);
int taxi_startup_intro_step(TaxiStartupIntro *,uint8_t fade_active,uint8_t fade_mode,uint8_t confirm,uint8_t start,uint16_t fade_bg,uint16_t fade_obj);
#endif
