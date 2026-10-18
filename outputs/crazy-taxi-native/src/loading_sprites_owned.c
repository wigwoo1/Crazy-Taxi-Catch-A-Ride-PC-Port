#include "loading_sprites_owned.h"
static int call(void *context,TaxiLoadingPresent *s,uint32_t target,const uint32_t a[9],uint32_t position[3],uint32_t *result){
 const TaxiLoadingSpritesOwner *o=context;
 if(target==0x08027fb8){int r=taxi_sprite_replace_owned(o->sprites,a[0],a[1],a[2],a[3],a[4]);if(r<0)return -1;*result=0;return 0;}
 return o->indexed.strings.service(o->indexed.strings.context,s,target,a,position,result);
}
int taxi_loading_sprites_owned(TaxiLoadingPresent *s,const TaxiLoadingSpritesOwner *o){
 if(!s||!o||!o->sprites||o->sprites->rom!=o->indexed.strings.rom||o->sprites->rom_size!=o->indexed.strings.rom_size||!o->indexed.strings.service)return -1;
 TaxiLoadingIndexedOwner indexed=o->indexed;indexed.strings.service=call;indexed.strings.context=(void *)o;
 return taxi_loading_indexed_owned(s,&indexed);
}
