#include "ordinary_loading_owned.h"
int taxi_ordinary_loading_present(TaxiLoadingPresent *s,const TaxiOrdinaryLoadingOwner *o){
 if(!s||!o||!o->graphics||!o->object_mask)return -1;TaxiOrdinaryGraphicsOwners *g=o->graphics;
 if(!g->assets||!g->rom||!g->queue||!g->fonts)return -1;
 TaxiSpriteReplacementOwner sprite={g->rom,g->rom_size,0x08070e50,g->assets->bases,11,o->copies,o->immediate,g->assets->objects,sizeof g->assets->objects,g->destination};
 TaxiObjQueueOwner placement={g->rom,g->rom_size,0x08070e50,g->assets->bases,g->assets->palette_bases,11,g->queue,s->gate.width_7542,s->gate.height_7544,*o->object_mask};
 TaxiLoadingContentOwner content={{{{{g->rom,g->rom_size,o->selected_table,o->presentation,o->context},o->frame,o->frame_size,o->fill},&sprite},&placement},g->fonts};
 return taxi_loading_content_owned(s,&content);
}
