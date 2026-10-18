#include "render_viewport.h"
#include <string.h>
static int32_t bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
void taxi_render_viewport_set(TaxiRenderViewport *s,int32_t l,int32_t t,int32_t r,int32_t b){
 if(!s)return;
 s->left=bits((uint32_t)l&~3u);s->top=bits((uint32_t)t&~3u);
 s->right=bits((uint32_t)r&~3u);s->bottom=bits((uint32_t)b&~3u);
 for(unsigned i=0;i<2;i++){s->callback_bounds[i][0]=s->left;s->callback_bounds[i][1]=s->top;s->callback_bounds[i][2]=s->right;s->callback_bounds[i][3]=s->bottom;}
 s->center_x=bits((uint32_t)l+(uint32_t)r)/2;s->center_y=bits((uint32_t)t+(uint32_t)b)/2;
 s->width=bits((uint32_t)r-(uint32_t)l);s->height=bits((uint32_t)b-(uint32_t)t);s->focal=192;
 s->scale_x=bits((uint32_t)s->width<<7)/192;s->scale_y=bits((uint32_t)s->height<<7)/192;
}
void taxi_render_viewport_depth(TaxiRenderViewport *s,int32_t near_plane,int32_t far_plane){if(s){s->near_plane=near_plane;s->far_plane=far_plane;}}
