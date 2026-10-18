#include "title_entry.h"
int taxi_title_entry(TaxiTitleRuntime *runtime,const TaxiTitleLoad *loaded){
 if(!runtime||!loaded||loaded->registry.count!=20||loaded->registry.bg_bytes!=128||loaded->registry.obj_bytes!=512||loaded->fade.masks.mode!=1||loaded->fade.masks.background_mask!=0x2001||loaded->fade.masks.object_mask!=0x2001)return -1;
 runtime->callback=(TaxiTitleCallback){0};taxi_title_flow_init(&runtime->flow);
 runtime->fade=loaded->fade.fade;runtime->bldy=loaded->fade.bldy;
 runtime->bg_palette_mask=loaded->fade.masks.background_mask;runtime->obj_palette_mask=loaded->fade.masks.object_mask;
 runtime->clock=0;runtime->finished=0;return 0;
}
