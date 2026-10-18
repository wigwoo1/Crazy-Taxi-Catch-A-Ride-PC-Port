#include "graphics_bootstrap_tail.h"
#include "render_frame_owned.h"
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int taxi_graphics_bootstrap_tail(TaxiGraphicsBootstrapTail *out,TaxiRestartRenderContext *render,TaxiSceneRegistration *registration,const TaxiGraphicsBootstrapAssets *assets,const int32_t bounds[4],int32_t near_plane,int32_t far_plane){
 if(!out||!render||!registration||!assets||!bounds||!assets->descriptor||assets->descriptor_size!=104||!assets->sine||!assets->cosine||assets->sine_size!=8192||assets->cosine_size!=8192||!assets->textures||!assets->palette)return -1;
 uint32_t zero[7]={0};
 if(taxi_graphics_storage_reset(&out->storage))return -1;
 render->background_pattern_6850=out->storage.background_6850;
 if(taxi_restart_render_call(render,0x0802cae4,zero)!=1)return -1;
 taxi_trig_decode(&out->trig,assets->sine,assets->cosine);
 if(taxi_scene_registration_render(registration,assets->descriptor,assets->descriptor_size,assets->textures,assets->palette,render))return -1;
 if(taxi_restart_render_call(render,0x0802c834,zero)!=1)return -1;
 taxi_render_viewport_set(&render->viewport,bounds[0],bounds[1],bounds[2],bounds[3]);
 taxi_render_viewport_depth(&render->viewport,near_plane,far_plane);
 if(taxi_render_frame_owned(&out->frame,&render->viewport,word(registration->descriptor+0x40),word(registration->descriptor+0x44),0x03006940))return -1;
 return taxi_restart_render_call(render,0x0802c558,zero)==1?0:-1;
}
