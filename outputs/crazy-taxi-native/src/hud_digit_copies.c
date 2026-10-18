#include "hud_digit_copies.h"
int taxi_hud_digit_copy_requests(TaxiHudDigitCopyRequest out[15],const int16_t first[10],const int16_t second[9]){
 if(!out||!first||!second)return -1;unsigned count=0;
 for(unsigned group=5;group<=6;group++){
  const int16_t *digits=group==5?first:second;unsigned limit=group==5?10:9,skip=group==5?3:2;
  for(unsigned slot=0;slot<limit;slot++){if(slot==skip||slot==skip+4)continue;int blank=digits[slot]==-1;out[count++]=(TaxiHudDigitCopyRequest){blank?0x03005400u:(group==5?0x080c6e8cu:0x080c7d8cu),blank?0u:(uint16_t)digits[slot],slot,group,0};}
 }return (int)count;
}
