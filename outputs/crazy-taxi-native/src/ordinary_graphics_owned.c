#include "ordinary_graphics_owned.h"
int taxi_ordinary_graphics_owned(TaxiOrdinaryGraphicsOwners *o,uint32_t retained_sl){
 if(!o||!o->assets||!o->palette||!o->banks||!o->fonts||!o->queue||!o->transport||!o->base_template||!o->matrix_template||!o->rom)return -1;
 if(taxi_obj_assets_load_eleven(o->assets,o->palette,o->banks,o->rom,o->rom_size,0x08070e50,o->destination)||taxi_startup_obj_setup(o->queue,o->transport,o->base_template,o->matrix_template,1))return -1;
 if(taxi_font_setup(o->fonts,0,32,0,0x08415bbc,retained_sl,0)||taxi_font_setup(o->fonts,1,32,3,0,3,0))return -1;
 return 0;
}

int taxi_ordinary_objects_publish(TaxiOrdinaryGraphicsOwners *o){
 if(!o)return -1;
 return taxi_startup_obj_publish(o->queue,o->transport,o->base_template,o->matrix_template);
}


int taxi_ordinary_scene_palette_owned(TaxiOrdinaryGraphicsOwners *o,
 const TaxiSceneRegistration *scene,const uint16_t expected[256],TaxiPaletteTransfer *request){
 if(!o||!o->palette||!o->banks||!o->rom||!scene||!scene->registered||!expected||!request)return -1;
 const uint8_t *p=scene->descriptor+24;
 uint32_t identity=p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
 if(identity<0x08000000u)return -1;
 size_t offset=identity-0x08000000u;
 if(offset>o->rom_size||512>o->rom_size-offset)return -1;
 uint16_t palette[256];
 for(unsigned i=0;i<256;i++){
  palette[i]=(uint16_t)(o->rom[offset+i*2]|(unsigned)o->rom[offset+i*2+1]<<8);
  /* Sparse graphics views must contain the actual scene source, not zeros
     from an omitted range that happens to be inside their address space. */
  if(palette[i]!=expected[i])return -1;
 }
 TaxiPaletteRegistration next=*o->palette;TaxiPaletteBanks banks=*o->banks;TaxiPaletteTransfer transfer;
 int index=taxi_palette_register(&next,palette,identity,32,1,&transfer);
 if(index<0||taxi_palette_transfer_apply(&banks,&transfer,identity,palette))return -1;
 *o->palette=next;*o->banks=banks;*request=transfer;return index;
}