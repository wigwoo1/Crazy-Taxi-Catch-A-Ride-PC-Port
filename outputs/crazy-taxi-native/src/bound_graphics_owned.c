#include "ordinary_graphics_owned.h"
#include "palette_fade_registered.h"
#include <string.h>
int taxi_bound_graphics_start_owned(TaxiOrdinaryGraphicsOwners *o,uint32_t retained_sl,
 const uint16_t palette[256],uint32_t identity,TaxiPaletteFadeStart *fade){
 if(!o||!o->assets||!o->palette||!o->banks||!o->fonts||!o->queue||!o->transport||!o->base_template||!o->matrix_template||!o->rom||!palette||!fade)return -1;
 /* The native font view is authoritative when adapting separate host owners
    to the original shared-memory layout. Reset may touch this region when
    a larger retained registration count includes its records. */
 uint8_t *records=(uint8_t *)o->palette->records;
 memcpy(records+0x680,o->fonts,sizeof *o->fonts);
 if(taxi_palette_registration_reset(o->palette))return -1;
 memcpy(o->fonts,records+0x680,sizeof *o->fonts);
 if(taxi_obj_assets_load_eleven(o->assets,o->palette,o->banks,o->rom,o->rom_size,0x08070e50,o->destination)||
    taxi_startup_obj_setup(o->queue,o->transport,o->base_template,o->matrix_template,1)||
    taxi_font_setup(o->fonts,0,32,0,0x08415bbc,retained_sl,0))return -1;
 /* FEC8 writes alias the tail of the original palette registration owner.
    Publish only changed fields; preserve padding and other font slots. */
 memset(records+0x680,0,4);
 memcpy(records+0x684,o->fonts->descriptors[0],20);
 records[0x6a4]=o->fonts->descriptors[0][32];
 TaxiPaletteTransfer transfer;
 if(taxi_palette_register(o->palette,palette,identity,32,1,&transfer)!=16||
    taxi_palette_transfer_apply(o->banks,&transfer,identity,palette))return -1;
 /* E5A8(1,32,1,1): initialize the registered normal fade from immutable
    source colors. Retained bank bytes outside eligible records survive. */
 TaxiPaletteSource sources[17];uint16_t colors[17][256];
 if(o->palette->count!=17||taxi_palette_fade_start_normal(fade,1,32,1,1,240,160))return -1;
 for(unsigned i=0;i<17;i++){
  const uint8_t *r=o->palette->records[i];
  unsigned count=r[0]|(unsigned)r[1]<<8;
  uint32_t source=r[16]|(uint32_t)r[17]<<8|(uint32_t)r[18]<<16|(uint32_t)r[19]<<24;
  if(count>256)return -1;
  if(source==identity){memcpy(colors[i],palette,count*2);}
  else{
   if(source<0x08000000u)return -1;
   size_t offset=source-0x08000000u;
   if(offset>o->rom_size||count*2>o->rom_size-offset)return -1;
   for(unsigned j=0;j<count;j++)colors[i][j]=(uint16_t)(o->rom[offset+j*2]|(unsigned)o->rom[offset+j*2+1]<<8);
  }
  sources[i]=(TaxiPaletteSource){source,colors[i],count};
 }
 if(taxi_palette_fade_registered_normal(o->banks,o->palette,fade,sources,17))return -1;
 return 0;
}

int taxi_bound_graphics_owned(TaxiOrdinaryGraphicsOwners *o,uint32_t retained_sl,const uint16_t palette[256],uint32_t identity){
 TaxiPaletteFadeStart fade={0};
 return taxi_bound_graphics_start_owned(o,retained_sl,palette,identity,&fade);
}
