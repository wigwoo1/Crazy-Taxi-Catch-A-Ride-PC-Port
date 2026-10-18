#include "ordinary_presentation_owned.h"
int taxi_ordinary_presentation_helper(void *context,TaxiPresentationService *s,uint32_t target,uint32_t arg){
 TaxiOrdinaryPresentationOwner *o=context;TaxiOrdinaryGraphicsOwners *g=o->graphics;
 if(target==0x08022508)return taxi_city_palette_gameplay_transfer(o->palette,arg);
 if(target==0x0800ee10)return taxi_city_palette_frame(o->palette,(int32_t)s->delta_3b00,o->mosaic);
 if(target==0x08027cd0)return taxi_startup_obj_publish(g->queue,g->transport,g->base_template,g->matrix_template);
 if(target==0x08028ee0){TaxiSpriteCopyFlushOwner copy={g->rom,g->rom_size,g->assets->objects,sizeof g->assets->objects,g->destination,o->copies,o->immediate};return taxi_sprite_copy_flush_owned(&copy);}
 return o->platform(o->context,s,target,arg);
}
int taxi_ordinary_presentation_owner_validate(TaxiOrdinaryPresentationOwner *o){
 if(!o||!o->graphics||!o->palette||!o->copies||!o->immediate||!o->mosaic||!o->platform)return -1;
 TaxiOrdinaryGraphicsOwners *g=o->graphics;
 if(!g->assets||!g->queue||!g->transport||!g->base_template||!g->matrix_template||!g->rom||o->palette->registry!=g->palette||o->palette->banks!=g->banks||o->palette->rom!=g->rom||o->palette->rom_size!=g->rom_size)return -1;
 return 0;
}
int taxi_ordinary_presentation_owned(TaxiPresentationService *s,TaxiOrdinaryPresentationOwner *o){
 if(!s||taxi_ordinary_presentation_owner_validate(o))return -1;
 return taxi_presentation_service(s,taxi_ordinary_presentation_helper,o);
}
