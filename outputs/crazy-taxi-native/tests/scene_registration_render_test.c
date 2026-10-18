#include "scene_registration_render.h"
#include "shade_table.h"
#include "world_background.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
int main(int argc,char **argv){
 assert(argc==4);TaxiSceneRegistration source={0};assert(!taxi_scene_registration_load(&source,argv[1]));
 TaxiIndexedAsset textures={0};assert(!taxi_indexed_asset_load(&textures,argv[2]));
 FILE *f=fopen(argv[3],"rb");assert(f);uint8_t bytes[512];assert(fread(bytes,1,512,f)==512&&fgetc(f)==EOF);fclose(f);uint16_t palette[256];for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(bytes[2*i]|bytes[2*i+1]<<8);
 uint8_t first[38400],second[38400],lookup[2560];
 for(unsigned positive=0;positive<2;positive++){
  memset(first,165,sizeof first);memset(second,90,sizeof second);memset(lookup,77,sizeof lookup);
  TaxiSceneRegistration registered={0};TaxiRestartRenderContext s={.frame=first,.frame_size=sizeof first,.alternate_frame=second,.alternate_size=sizeof second,.width=240,.height=160,.bitmap_mode=4,.shade_lookup=lookup,.shade_lookup_size=sizeof lookup,.shade_depth_6854=positive?38400:0,.shade_color_6858=0x00f87820};
  assert(!taxi_scene_registration_render(&registered,source.descriptor,104,&textures,palette,&s));
  assert(registered.registered&&!memcmp(source.descriptor,registered.descriptor,104)&&s.palette==palette&&s.engine_present);
  uint8_t clear;assert(taxi_palette_nearest_index(palette,0,0,0,&clear));for(unsigned i=0;i<38400;i++)assert(first[i]==clear&&second[i]==clear);
  uint32_t sky;assert(!taxi_world_background_pattern(&textures,source.background,&sky));assert(s.background_pattern_6850==sky&&s.fill_06900==clear);
  assert(s.shade_depth_6854==(positive?38400u:0u)&&s.shade_color_6858==0x00f87820);
  if(!positive)for(unsigned i=0;i<2560;i++)assert(lookup[i]==77);
  else for(unsigned i=0;i<256;i++)assert(lookup[i]==i&&lookup[256+i]==i);
 }
 taxi_indexed_asset_free(&textures);puts("PASS actualcity registration composition: descriptor/palette owners, BOTH blackcleared buffers, distinctskyfill, retained shade settings and zero/nonzero lookup paths");return 0;
}
