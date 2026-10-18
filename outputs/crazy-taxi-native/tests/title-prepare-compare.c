#include "../src/title_prepare.h"
#include <stdio.h>
#include <string.h>
static unsigned char rom[8388608];
int main(int argc,char **argv){
 if(argc!=4)return 2;unsigned language=(unsigned)(argv[2][0]-'0');FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 TaxiTitleAssets a={0};TaxiPaletteRegistration s={0};TaxiPaletteBanks b;TaxiFontSetup fonts;memset(&b,165,sizeof b);memset(&fonts,165,sizeof fonts);
 s.count=4;s.bg_bytes=128;s.records[0][0]=16;s.records[0][6]=1;s.records[0][7]=1;
 s.records[0][16]=0xf8;s.records[0][17]=0x70;s.records[0][18]=0x0b;s.records[0][19]=8;
 if(taxi_title_prepare(&a,&s,&b,&fonts,rom,sizeof rom,language))return 4;
 f=fopen(argv[3],"wb");if(!f)return 5;
 fwrite(a.objects,1,sizeof a.objects,f);fwrite(s.records,1,sizeof s.records,f);fwrite(&b,1,sizeof b,f);fwrite(fonts.descriptors,1,sizeof fonts.descriptors,f);fwrite(&fonts.cursor,1,4,f);
 return fclose(f)!=0;
}
