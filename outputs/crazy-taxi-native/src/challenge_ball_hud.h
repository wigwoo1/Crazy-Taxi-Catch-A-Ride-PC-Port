#ifndef TAXI_CHALLENGE_BALL_HUD_H
#define TAXI_CHALLENGE_BALL_HUD_H
#include "challenge_flag_hud.h"
typedef struct {uint8_t glyph;int32_t x,y,z;} TaxiBallCountGlyph;
typedef struct {TaxiFlagHudPlan clock;TaxiBallCountGlyph counter[5];unsigned counter_count;uint32_t label_id;int32_t label_x,label_y;uint8_t label_alignment,label_style;uint16_t label_scale;} TaxiBallHudPlan;
/* Original8430 timer/indicator/counter and F7DC text request metadata.
   positions are original60-byte087EC12C..167 records. Clock OBJ flags1,
   counter flags0; label fontgroup0, alignment2, style1 paused/2 otherwise. */
int taxi_challenge_ball_hud_plan(TaxiBallHudPlan *out,int16_t remaining,int16_t popped,uint16_t width,uint16_t height,uint8_t vehicle_flags,int paused,const uint8_t positions[60],size_t size);
/* Timer and counter group3 OBJ pixels at logical240x160. Label/indicator
   composition and live GPU upload remain caller-owned. */
int taxi_challenge_ball_hud_pixels(uint8_t out[153600],const TaxiBallHudPlan *plan,const TaxiFontAsset *font);
/* Original F7DC label font0, full-size right alignment and bank1/2.
   Appends OBJ pixels, preserving earlier timer/counter priority. */
int taxi_challenge_ball_label_pixels(uint8_t out[153600],const TaxiBallHudPlan *plan,const TaxiFontAsset *font,const char *label);
#endif
