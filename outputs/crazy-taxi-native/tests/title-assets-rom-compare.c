#include "../src/title_assets.h"
#include <stdio.h>
#include <string.h>
static unsigned char rom[8388608];
int main(int argc,char **argv){
 if(argc!=3)return 2;FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 TaxiTitleAssets a={0};TaxiPaletteRegistration s={0};TaxiPaletteBanks b;memset(&b,165,sizeof b);
 s.count=4;s.bg_bytes=128;s.records[0][0]=16;s.records[0][6]=1;s.records[0][7]=1;
 s.records[0][16]=0xf8;s.records[0][17]=0x70;s.records[0][18]=0x0b;s.records[0][19]=8;
 if(taxi_title_assets_load(&a,&s,&b,rom,sizeof rom))return 4;
 const int32_t palette_bases[11]={0,2,5,6,8,10,11,12,13,14,15};
 if(memcmp(a.palette_bases,palette_bases,sizeof palette_bases))return 6;
 f=fopen(argv[2],"wb");if(!f)return 5;
 for(unsigned g=0;g<11;g++){uint32_t v=a.bases[g];for(unsigned k=0;k<4;k++)fputc((int)((v>>(8*k))&255),f);}
 fwrite(a.objects,1,sizeof a.objects,f);fwrite(s.records,1,sizeof s.records,f);fwrite(&b,1,sizeof b,f);
 fprintf(stdout,"count=%u bg=%u obj=%u\n",s.count,s.bg_bytes,s.obj_bytes);return fclose(f)!=0;
}
