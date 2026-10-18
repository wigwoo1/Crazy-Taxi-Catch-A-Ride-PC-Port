#ifndef TAXI_GRAPHICS_BOOTSTRAP_TAIL_H
#define TAXI_GRAPHICS_BOOTSTRAP_TAIL_H
#include "graphics_storage.h"
#include "render_frame_init.h"
#include "scene_registration_render.h"
/* Native2AC6C AE5A..AF26 owner/helper sequence. Original executable-copy
   relocation and global callback bindings before AE5A are not implemented. */
typedef struct {TaxiGraphicsStorage storage;TaxiTrig trig;TaxiRenderFrameInit frame;} TaxiGraphicsBootstrapTail;
typedef struct {
 const uint8_t *descriptor;size_t descriptor_size;
 const uint8_t *sine,*cosine;size_t sine_size,cosine_size;
 TaxiIndexedAsset *textures;const uint16_t *palette;
} TaxiGraphicsBootstrapAssets;
int taxi_graphics_bootstrap_tail(TaxiGraphicsBootstrapTail *,TaxiRestartRenderContext *,TaxiSceneRegistration *,const TaxiGraphicsBootstrapAssets *,const int32_t bounds[4],int32_t near_plane,int32_t far_plane);
#endif
