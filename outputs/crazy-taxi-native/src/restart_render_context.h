#ifndef TAXI_RESTART_RENDER_CONTEXT_H
#define TAXI_RESTART_RENDER_CONTEXT_H
#include "view_transform.h"
#include "render_viewport.h"
/* Native ownership adapter for recovered helpers. Source addresses are
   dispatch identities, never dereferenced as host pointers. The caller supplies
   its live framebuffer/palette/transform owners. GPU mapping is separate. */
typedef struct {
 TaxiRenderViewport viewport;
 uint32_t state_2c4ec; /* Original2C8C4 code-allocation cursor;2CAE4 resets it. */
 uint8_t *frame;size_t frame_size;uint32_t fill_06900,background_pattern_6850;
 uint8_t *alternate_frame;size_t alternate_size;
 uint16_t width,height;uint8_t bitmap_mode,engine_present;
 uint8_t *shade_lookup;size_t shade_lookup_size;uint32_t shade_depth_6854,shade_color_6858;
 const uint8_t *registered_materials;size_t registered_material_count; /* Original02001790,20-byte records. */
 const uint16_t *palette;uint32_t pattern_engine_64;
 uint32_t source_transform_address;
 TaxiViewTransform *transform_owner,*selected_transform;
} TaxiRestartRenderContext;
/* 1 handled,0 other helper,-1 invalid native ownership. */
int taxi_restart_render_call(TaxiRestartRenderContext *,uint32_t target,const uint32_t args[7]);
#endif
