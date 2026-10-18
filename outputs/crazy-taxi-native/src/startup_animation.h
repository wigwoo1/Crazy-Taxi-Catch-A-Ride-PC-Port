#ifndef TAXI_STARTUP_ANIMATION_H
#define TAXI_STARTUP_ANIMATION_H
#include <stdint.h>
typedef struct {
 uint8_t phase_3574,palette_snapshot;
 int32_t alpha_3578,alpha_step_357c,scroll_3580,scroll_step_3584,scale_3588,delay_358c,letters_left_3590,letters_right_3594;
 uint16_t scroll_register,blend_control,blend_alpha;
 uint16_t saved_palette[256];
 uint8_t letter_flags_37a4[13];uint16_t letter_scale_37b2[13];
 int32_t letter_37d8[13],letter_380c[13],letter_3840[13],value_37cc,value_37d0,value_37d4,revealed_3598,remaining_359c;
 uint16_t visible_letter_colors[13];uint8_t fade_interval_1771,fade_reload_1772;
} TaxiStartupAnimation;
/* Native drawing requests and register-field observations; no instruction runtime. */
typedef struct {unsigned kind;uint32_t values[11];} TaxiStartupAnimationEvent;
typedef int (*TaxiStartupAnimationSink)(void *,const TaxiStartupAnimationEvent *);
/* Original167D0 callback flow. Drawing/fade requests require native service binding.
   Reject revealed counts above13 rather than access beyond owned original records. */
int taxi_startup_animation_step(TaxiStartupAnimation *,uint8_t fade_active,uint8_t fade_mode,uint16_t width,const uint16_t palette[256],uint8_t confirm,uint8_t start,uint16_t fade_bg,uint16_t fade_obj,TaxiStartupAnimationSink,void *);
#endif
