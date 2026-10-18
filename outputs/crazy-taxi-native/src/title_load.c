#include "title_load.h"
static unsigned half(const uint8_t *p){return p[0]|(unsigned)p[1]<<8;}
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int objects(void *context){
 TaxiTitleLoad *s=context;return taxi_startup_obj_setup(&s->queue,&s->transport,s->base_template,s->matrix_template,1);
}
int taxi_title_load(TaxiTitleLoad *s,const uint8_t *rom,size_t bytes,unsigned language){
 if(!s||!rom||language>=4)return -1;
 if(taxi_title_background_load(&s->background,&s->registry,&s->banks,rom,bytes))return -1;
 if(taxi_title_prepare_objects(&s->assets,&s->registry,&s->banks,&s->fonts,rom,bytes,language,objects,s))return -1;
 TaxiPaletteSource sources[20];uint16_t colors[20][16];size_t count=0;
 for(unsigned i=0;i<s->registry.count;i++){
  const uint8_t *record=s->registry.records[i];if(!record[6])continue;
  uint32_t identity=word(record+16);if(identity<0x08000000u||half(record)!=16)return -1;
  size_t offset=identity-0x08000000u;if(offset>bytes||bytes-offset<32)return -1;
  for(unsigned j=0;j<16;j++)colors[count][j]=(uint16_t)half(rom+offset+j*2);
  sources[count]=(TaxiPaletteSource){identity,colors[count],16};count++;
 }
 if(taxi_palette_fade_start_normal(&s->fade,1,32,0x2001,0x2001,240,160))return -1;
 return taxi_palette_fade_registered_normal(&s->banks,&s->registry,&s->fade,sources,count);
}
