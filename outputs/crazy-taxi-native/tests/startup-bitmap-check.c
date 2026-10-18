#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/startup_bitmap.h"
int main(int argc,char **argv){
 if(argc!=4)return 2;
 FILE *f=fopen(argv[1],"rb");if(!f)return 3;
 unsigned long address=strtoul(argv[2],NULL,0),offset=address-0x08000000;
 if(fseek(f,0,SEEK_END))return 4;long length=ftell(f);
 if(length<0||offset>=(unsigned long)length)return 4;
 size_t n=(size_t)length-offset;uint8_t *image=malloc(n);if(!image)return 5;
 if(fseek(f,(long)offset,SEEK_SET)||fread(image,1,n,f)!=n)return 6;fclose(f);
 TaxiDisplayMode display={0};uint16_t palette[256];uint8_t a[40960],b[40960];
 memset(a,123,sizeof a);memset(b,77,sizeof b);
 TaxiPaletteRegistration registry={0};TaxiPaletteBanks banks;TaxiPaletteTransfer transfer;
 memset(&banks,165,sizeof banks);
 int result=taxi_startup_bitmap_load_registered(&display,&registry,&banks,image,n,(uint32_t)address,palette,a,b,&transfer);
 if(registry.count!=1||registry.bg_bytes!=512||registry.obj_bytes||transfer.source_identity!=0x084a50b0||transfer.destination_identity!=0x05000000||transfer.control!=0x84000080)return 10;
 for(unsigned i=0;i<256;i++)if(banks.background[i]!=0||banks.objects[i]!=0xa5a5)return 11;free(image);
 if(result!=1)return 7;
 f=fopen(argv[3],"wb");if(!f)return 8;
 if(fwrite(a,1,sizeof a,f)!=sizeof a||fwrite(b,1,sizeof b,f)!=sizeof b||fwrite(palette,1,sizeof palette,f)!=sizeof palette)return 9;
 fclose(f);printf("PASS native bitmap %lu control %u palette-pointer %u\n",address,display.control,(unsigned)(display.globals[20]|(display.globals[21]<<8)|(display.globals[22]<<16)|(display.globals[23]<<24)));return 0;
}
