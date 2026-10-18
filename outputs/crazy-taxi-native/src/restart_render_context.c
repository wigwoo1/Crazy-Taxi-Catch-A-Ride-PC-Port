#include "restart_render_context.h"
#include "world_background.h"
#include "shade_table.h"
#include <string.h>
static int32_t signed_bits(uint32_t value){int32_t out;memcpy(&out,&value,4);return out;}
int taxi_restart_render_call(TaxiRestartRenderContext *s,uint32_t target,const uint32_t args[7]){
 if(target!=0x0802ca94&&target!=0x0802cae4&&target!=0x0802c9f0&&target!=0x0802c834&&target!=0x0802c688&&target!=0x0802c558&&target!=0x0802c790&&target!=0x0802c784&&target!=0x0802b0b0)return 0;
 if(!s||!args)return -1;
 if(target==0x0802ca94){
  if(!s->shade_lookup||s->shade_lookup_size<2560||!s->registered_materials)return -1;
  return taxi_shade_table_material(s->shade_lookup,s->registered_materials,s->registered_material_count,args[0])?1:-1;
 }
 if(target==0x0802cae4){s->state_2c4ec=0;return 1;}
 if(target==0x0802b0b0){
  if(args[0]&&s->engine_present&&(!s->palette||!s->shade_lookup||s->shade_lookup_size<2560))return -1;
  s->shade_depth_6854=args[0];s->shade_color_6858=args[1];
  if(args[0]&&s->engine_present&&!taxi_shade_table_build(s->shade_lookup,s->palette,args[0],args[1]))return -1;
  return 1;
 }
 if(target==0x0802c790){taxi_render_viewport_set(&s->viewport,signed_bits(args[0]),signed_bits(args[1]),signed_bits(args[2]),signed_bits(args[3]));return 1;}
 if(target==0x0802c784){taxi_render_viewport_depth(&s->viewport,signed_bits(args[0]),signed_bits(args[1]));return 1;}
 if(target==0x0802c558){
  if(s->bitmap_mode!=4||!s->engine_present)return 1;
  size_t bytes=((size_t)s->width*s->height/2)*2;
  uint8_t index=0;
  if(!s->palette||!s->frame||!s->alternate_frame||bytes>s->frame_size||bytes>s->alternate_size||
     !taxi_palette_nearest_index(s->palette,args[0],args[1],args[2],&index))return -1;
  s->fill_06900=index;
  memset(s->frame,index,bytes);memset(s->alternate_frame,index,bytes);return 1;
 }
 if(target==0x0802c9f0){
  if(!s->frame||s->frame_size<38400)return -1;
  return taxi_world_clear_rect(s->frame,signed_bits(args[0]),signed_bits(args[1]),signed_bits(args[2]),signed_bits(args[3]),s->fill_06900)?-1:1;
 }
 if(target==0x0802c834){
  if(args[0]==0){s->selected_transform=NULL;return 1;}
  if(!s->transform_owner||args[0]!=s->source_transform_address)return -1;
  s->selected_transform=s->transform_owner;return 1;
 }
 if(!s->palette||!taxi_palette_nearest_pair(s->palette,args[0],args[1],args[2],&s->pattern_engine_64))return -1;
 return 1;
}
