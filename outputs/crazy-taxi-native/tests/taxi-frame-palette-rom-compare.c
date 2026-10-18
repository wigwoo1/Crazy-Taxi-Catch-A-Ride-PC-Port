#include "taxi_frame_palette_owned.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){FILE *f=fopen("Crazy Taxi - Catch a Ride (USA).gba","rb");uint8_t *rom=malloc(8388608);assert(f&&rom&&fread(rom,1,8388608,f)==8388608);fclose(f);unsigned immediate,driver,override;while(scanf("%u%u%u",&immediate,&driver,&override)==3){TaxiPaletteRegistration registry;TaxiPaletteBanks banks;memset(&registry,165,sizeof registry);memset(&banks,165,sizeof banks);TaxiPaletteTransfer transfer={0x11111111,0x22222222,0x33333333};int changed=taxi_frame_palette_owned(rom,8388608,&registry,&banks,&transfer,(uint8_t)immediate,(uint8_t)driver,(uint8_t)override);printf("%d %u %u %u ",changed,transfer.source_identity,transfer.destination_identity,transfer.control);for(unsigned i=0;i<256;i++)printf("%02x",((uint8_t*)registry.records)[i]);printf(" ");for(unsigned i=0;i<256;i++)printf("%04x",banks.objects[i]);puts("");}free(rom);return 0;}
