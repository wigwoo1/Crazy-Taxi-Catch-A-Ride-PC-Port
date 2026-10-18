#include "taxi_render_tail_owned.h"
static int signed_byte(uint8_t v){return v&128?(int)v-256:v;}
int taxi_render_tail_owned(TaxiPaletteRegistration *registry,TaxiPaletteBanks *banks,
 TaxiPaletteTransfer *transfer,const uint8_t *rom,size_t size,uint8_t shared[3],
 uint8_t fade,uint32_t delta,const uint32_t first[3],uint32_t anchor[3]){
 if(!shared||!first||!anchor)return -1;
 int flash=shared[0]&&signed_byte(shared[1])>=0&&!fade;
 if(flash&&(!registry||!banks||!transfer||!rom||size<0x20fce0u+32u))return -1;
 uint32_t position[3]={first[0],first[1],first[2]};
 if(flash){
  const uint32_t identity=0x0820fce0u;
  for(unsigned i=0;i<16;i++)banks->objects[64+i]=(uint16_t)(rom[0x20fce0u+i*2]|(uint16_t)rom[0x20fce1u+i*2]<<8);
  *transfer=(TaxiPaletteTransfer){identity,0x05000280u,0x84000008u};
  for(unsigned i=0;i<4;i++)registry->records[4][16+i]=(uint8_t)(identity>>(i*8));
 }
 for(unsigned i=0;i<3;i++)anchor[i]=position[i];
 if(shared[0]){
  shared[2]=(uint8_t)((uint32_t)shared[2]+delta);
  if(signed_byte(shared[2])>7){
   shared[2]=0;shared[1]=(uint8_t)(shared[1]+1u);
   if(signed_byte(shared[1])==10)shared[1]=7;
  }
 }
 return flash;
}