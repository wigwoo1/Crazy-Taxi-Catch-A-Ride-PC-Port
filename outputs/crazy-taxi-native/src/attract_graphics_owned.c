#include "attract_graphics_owned.h"
static int service(void *context,uint32_t target,const uint32_t a[6]){
 TaxiAttractGraphicsOwners *o=context;
 if(target==0x08027994){
  if(a[0]!=0x0805a338||a[1]!=1)return -1;
  if(taxi_obj_assets_load_eleven(o->assets,o->palette,o->banks,o->rom,o->rom_size,a[0],o->destination))return -1;
  return taxi_startup_obj_setup(o->queue,o->transport,o->base_template,o->matrix_template,1);
 }
 if(target==0x0800fec8)return taxi_font_setup(o->fonts,a[0],a[1],a[2],a[3],a[4],a[5]);
 return -1;
}
int taxi_attract_graphics_owned(TaxiAttractGraphicsOwners *o){
 if(!o||!o->assets||!o->palette||!o->banks||!o->fonts||!o->queue||!o->transport||!o->base_template||!o->matrix_template||!o->rom)return -1;
 return taxi_attract_graphics_prefix(service,o);
}
