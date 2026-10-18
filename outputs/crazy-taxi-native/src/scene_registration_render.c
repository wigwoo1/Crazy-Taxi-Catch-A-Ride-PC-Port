#include "scene_registration_render.h"
#include "world_background.h"
int taxi_scene_registration_render(TaxiSceneRegistration *registration,const uint8_t *descriptor,size_t size,
    const TaxiIndexedAsset *textures,const uint16_t palette[256],TaxiRestartRenderContext *render){
 if(!registration||!render||!palette||!textures)return -1;
 TaxiSceneRegistration next;
 if(taxi_scene_registration_set(&next,descriptor,size))return -1;
 /* Source1E7F0 copy/engine publish precedes2C558, and the shade helper retains
    its two global arguments rather than initializing them to new defaults. */
 *registration=next;render->palette=palette;render->engine_present=1;
 const uint32_t black[7]={0};
 if(taxi_restart_render_call(render,0x0802c558,black)!=1)return -1;
 if(taxi_world_background_pattern(textures,registration->background,&render->background_pattern_6850))return -1;
 uint32_t shade[7]={render->shade_depth_6854,render->shade_color_6858,0,0,0,0,0};
 return taxi_restart_render_call(render,0x0802b0b0,shade)==1?0:-1;
}
