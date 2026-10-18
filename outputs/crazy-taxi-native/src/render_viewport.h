#ifndef TAXI_RENDER_VIEWPORT_H
#define TAXI_RENDER_VIEWPORT_H
#include <stdint.h>
/* Original2C790 shared graphics fields and two callback clip rectangles.
   Edge alignment precedes storage; centers use unaligned caller arguments. */
typedef struct {int32_t left,top,right,bottom,center_x,center_y,width,height,focal,scale_x,scale_y,near_plane,far_plane;int32_t callback_bounds[2][4];} TaxiRenderViewport;
void taxi_render_viewport_set(TaxiRenderViewport *,int32_t left,int32_t top,int32_t right,int32_t bottom);
/* Original2C784 separate near/far setter. */
void taxi_render_viewport_depth(TaxiRenderViewport *,int32_t near_plane,int32_t far_plane);
#endif
