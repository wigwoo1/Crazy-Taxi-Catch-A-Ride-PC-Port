#include "../src/palette_registration_small.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned flags,mode,cap,count,bg,obj,seed;
 while(scanf("%u %u %u %u %u %u %u",&flags,&mode,&cap,&count,&bg,&obj,&seed)==7){
  TaxiPaletteRegistration s={0};TaxiPaletteBanks banks;uint16_t palette[16];TaxiPaletteTransfer transfer={0x11111111,0x22222222,0x33333333};
  s.flags=(uint8_t)flags;s.count=(uint16_t)count;s.bg_bytes=(uint16_t)bg;s.obj_bytes=(uint16_t)obj;memset(s.records,165,sizeof s.records);memset(&banks,37,sizeof banks);
  for(unsigned i=0;i<16;i++)palette[i]=(uint16_t)(seed+i*131);
  int result=taxi_palette_register_small(&s,palette,0x10001000,cap,mode,&transfer);
  if(result>=0&&taxi_palette_transfer_small_apply(&banks,&transfer,0x10001000,palette))return 2;
  printf("%d %u %u %u %u %u %u ",result,s.count,s.bg_bytes,s.obj_bytes,transfer.source_identity,transfer.destination_identity,transfer.control);
  const unsigned char *p=(const unsigned char *)s.records;for(unsigned i=0;i<sizeof s.records;i++)printf("%02x",p[i]);putchar(' ');
  p=(const unsigned char *)&banks;for(unsigned i=0;i<sizeof banks;i++)printf("%02x",p[i]);puts("");
 }return 0;}
