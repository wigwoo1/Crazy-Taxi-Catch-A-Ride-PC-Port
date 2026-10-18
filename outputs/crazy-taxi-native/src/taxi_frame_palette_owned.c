#include "taxi_frame_palette_owned.h"
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int taxi_frame_palette_owned(const uint8_t *rom,size_t size,TaxiPaletteRegistration *registry,TaxiPaletteBanks *banks,TaxiPaletteTransfer *transfer,uint8_t immediate,uint8_t driver,uint8_t override){
 if(!immediate)return 0;
 if(!rom||!registry||!banks||!transfer||driver>3||size<0x4a5544)return -1;
 uint32_t identity=word(rom+0x4a5514+driver*12+8);
 if(identity<0x08000000u||(size_t)(identity-0x08000000u)>size||size-(identity-0x08000000u)<32)return -1;
 unsigned bank=override?8:4;const uint8_t *palette=rom+identity-0x08000000u;
 for(unsigned i=0;i<16;i++)banks->objects[bank*16+i]=(uint16_t)(palette[2*i]|(uint16_t)palette[2*i+1]<<8);
 *transfer=(TaxiPaletteTransfer){identity,override?0x05000300u:0x05000280u,0x84000008u};
 uint8_t *record=registry->records[bank]+16;for(unsigned i=0;i<4;i++)record[i]=(uint8_t)(identity>>(i*8));
 return 1;
}
