#include "palette_bank.h"
#include <string.h>
int taxi_palette_transfer_apply(TaxiPaletteBanks *banks,const TaxiPaletteTransfer *request,uint32_t caller_identity,const uint16_t caller_palette[256]){
 if(!banks||!request||request->control!=0x84000080u)return -1;
 uint16_t *destination=request->destination_identity==0x05000000u?banks->background:
  request->destination_identity==0x05000200u?banks->objects:NULL;
 if(!destination)return -1;
 if(request->source_identity==0x084a50b0u||request->source_identity==0x084a52b0u){
  /* Both complete source banks verified from the supplied ROM. */
  uint16_t color=request->source_identity==0x084a50b0u?0:0x07ff;
  for(unsigned i=0;i<256;i++)destination[i]=color;
 }else{
  if(request->source_identity!=caller_identity||!caller_palette)return -1;
  memmove(destination,caller_palette,512);
 }
 return 0;
}
