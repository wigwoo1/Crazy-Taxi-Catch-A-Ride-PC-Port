#include "loading_indexed_owned.h"
#include "loading_image.h"
#include "world_background.h"
#include <string.h>
static int32_t bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
static int call(void *context,TaxiLoadingPresent *s,uint32_t target,const uint32_t a[9],uint32_t position[3],uint32_t *result){
 const TaxiLoadingIndexedOwner *o=context;
 if(target==0x0802c9f0){if(!*o->frame)return -1;*result=0;return taxi_world_clear_rect(*o->frame,bits(a[0]),bits(a[1]),bits(a[2]),bits(a[3]),*o->fill);}
 if(target==0x08031ef8){
  if(!*o->frame||a[0]||a[2]!=0x00400040||a[3]!=0x080eb884||a[4]!=1||o->strings.rom_size<0xec884)return -1;
  *result=0;return taxi_loading_image_placement(*o->frame,o->strings.rom+0xeb884,a[1]);
 }
 return o->strings.service(o->strings.context,s,target,a,position,result);
}
int taxi_loading_indexed_owned(TaxiLoadingPresent *s,const TaxiLoadingIndexedOwner *o){
 if(!s||!o||!o->frame||o->frame_size<38400||!o->fill||!o->strings.service)return -1;
 TaxiLoadingStringsOwner strings=o->strings;strings.service=call;strings.context=(void *)o;
 return taxi_loading_strings_owned(s,&strings);
}
