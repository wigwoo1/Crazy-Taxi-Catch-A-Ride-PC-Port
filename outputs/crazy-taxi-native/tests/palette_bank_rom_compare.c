#include "palette_bank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <io.h>
int main(int argc,char **argv){
 if(argc!=4)return 1;
 unsigned flags=(unsigned)strtoul(argv[1],NULL,10),mode=(unsigned)strtoul(argv[2],NULL,10);
 FILE *f=fopen(argv[3],"rb");if(!f)return 2;uint8_t raw[512];if(fread(raw,1,512,f)!=512||fgetc(f)!=EOF){fclose(f);return 3;}fclose(f);
 uint16_t palette[256];for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(raw[i*2]|raw[i*2+1]<<8);
 TaxiPaletteRegistration state={.flags=(uint8_t)flags};TaxiPaletteTransfer request;
 assert(taxi_palette_register(&state,palette,0x084a5a34,32,mode,&request)==0);
 TaxiPaletteBanks banks;memset(&banks,0xa5,sizeof banks);
 assert(!taxi_palette_transfer_apply(&banks,&request,0x084a5a34,palette));
 TaxiPaletteBanks before=banks;TaxiPaletteTransfer invalid=request;invalid.control=0;
 assert(taxi_palette_transfer_apply(&banks,&invalid,0x084a5a34,palette)==-1&&!memcmp(&banks,&before,sizeof banks));
 invalid=request;invalid.destination_identity=0x05000002;
 assert(taxi_palette_transfer_apply(&banks,&invalid,0x084a5a34,palette)==-1&&!memcmp(&banks,&before,sizeof banks));
 invalid=request;invalid.source_identity=0xdeadbeef;
 assert(taxi_palette_transfer_apply(&banks,&invalid,0x084a5a34,palette)==-1&&!memcmp(&banks,&before,sizeof banks));
 _setmode(_fileno(stdout),_O_BINARY);return fwrite(&banks,1,sizeof banks,stdout)==sizeof banks?0:4;
}
