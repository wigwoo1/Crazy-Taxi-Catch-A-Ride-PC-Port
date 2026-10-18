#ifndef TAXI_OBJ_SHADER_H
#define TAXI_OBJ_SHADER_H
#include <stdint.h>
/* Source Q8 field mapping for the taxi128x64 texture halves. */
int taxi_obj_shader_begin(const int16_t matrix[4],int doubled,unsigned half,int snapped);
/* General original8..64 OBJ dimensions; unscaled source texture offsets. */
int taxi_obj_shader_frame_begin(const int16_t matrix[4],int doubled,unsigned width,
 unsigned height,unsigned texture_width,unsigned texture_height,
 unsigned offset_x,unsigned offset_y,int snapped);
/* Forced semi-OBJ against immutable resolved BG2/backdrop RGBA capture.
   Caller owns topmost OBJ, raw555 colors, coverage and disables GL blending.
   Begin resets off; one setup per active frame, end restores texture unit1. */
int taxi_obj_shader_background(unsigned texture,unsigned width,unsigned height,uint16_t bldcnt,uint16_t bldalpha);
void taxi_obj_shader_end(void);
void taxi_obj_shader_free(void);
#endif