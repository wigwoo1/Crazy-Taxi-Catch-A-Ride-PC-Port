#include "restart_resources.h"
int taxi_restart_resources_call(void *context,uint32_t target,uint32_t a,uint32_t b,uint32_t *result){
 TaxiRestartResources *o=context;if(!o||!o->palette||!o->display||!o->bases||!result)return -1;*result=0;
 switch(target){
 case 0x0800f5e8:return a||b?-1:taxi_palette_registration_reset(o->palette);
 case 0x08025c18:return taxi_display_mode_four(o->display,a,b);
 case 0x08007378:return a||b?-1:taxi_graphics_memory_base_first(o->bases,o->display->globals[1]);
 case 0x080073c0:return a||b?-1:taxi_graphics_memory_base_second(o->bases,o->display->globals[1]);
 default:return -1;
 }
}
