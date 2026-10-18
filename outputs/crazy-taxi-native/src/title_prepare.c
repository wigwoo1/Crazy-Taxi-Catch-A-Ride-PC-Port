#include "title_prepare.h"
#include <string.h>
static uint32_t word(const uint8_t *r,size_t n){return (uint32_t)r[n]|(uint32_t)r[n+1]<<8|(uint32_t)r[n+2]<<16|(uint32_t)r[n+3]<<24;}
static uint16_t half(const uint8_t *r,size_t n){return (uint16_t)(r[n]|r[n+1]<<8);}
int taxi_title_prepare_objects(TaxiTitleAssets *a,TaxiPaletteRegistration *s,TaxiPaletteBanks *b,TaxiFontSetup *fonts,const uint8_t *r,size_t size,unsigned language,TaxiTitleObjectSetup setup,void *context){
 const size_t resource=0x34634;TaxiSpriteCopyAsset frames[4];uint32_t languages[4];TaxiTitlePromptSetup copies;
 if(!a||!s||!b||!fonts||!r||language>=4||size<0x7ebf58||s->count!=4||s->bg_bytes!=128||s->obj_bytes!=0)return -1;
 size_t group=resource+(size_t)word(r,resource+44);if(group>size||size-group<36)return -1;
 if(word(r,group)!=4||word(r,group+4)!=16)return -1;
 for(unsigned i=0;i<4;i++){
  size_t m=resource+(size_t)word(r,group+20+i*4);if(m>size||size-m<18)return -1;
  uint32_t src=word(r,0x7ebf48+i*4);if(src<0x08000000u||(size_t)(src-0x08000000u)>size||size-(src-0x08000000u)<1024)return -1;
  languages[i]=src;
  frames[i]=(TaxiSpriteCopyAsset){11,4,16,0x060163a0u,half(r,m),half(r,m+2),half(r,m+16)};
  /* Source title metrics are four consecutive32x16 pieces. */
  if(frames[i].width_tiles!=4||frames[i].height_tiles!=2||frames[i].tile_index>1016)return -1;
 }
 if(taxi_title_assets_load(a,s,b,r,size))return -1;
 if(setup&&setup(context))return -1;
 for(unsigned i=0;i<4;i++)frames[i].destination_base=a->bases[10];
 if(taxi_title_font_setup(fonts)||taxi_title_prompt_setup(s,languages,language,frames,&copies))return -1;
 for(unsigned i=0;i<4;i++){
  uint32_t src=copies.frames[i].source-0x08000000u,dst=copies.frames[i].destination-0x06010000u;
  size_t n=(copies.frames[i].control&65535u)*4u;
  if(src>size||n>size-src||dst>sizeof a->objects||n>sizeof a->objects-dst)return -1;
  memcpy(a->objects+dst,r+src,n);
 }
 for(unsigned i=0;i<16;i++)b->objects[160+i]=half(r,0x290ad4+i*2);
 return 0;
}

int taxi_title_prepare(TaxiTitleAssets *a,TaxiPaletteRegistration *s,TaxiPaletteBanks *b,TaxiFontSetup *fonts,const uint8_t *r,size_t size,unsigned language){return taxi_title_prepare_objects(a,s,b,fonts,r,size,language,0,0);}
