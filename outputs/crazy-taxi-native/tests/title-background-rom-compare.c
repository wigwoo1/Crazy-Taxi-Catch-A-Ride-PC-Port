#include "../src/title_background.h"
#include <stdio.h>
#include <string.h>
static unsigned char rom[8388608];
int main(int argc,char **argv){
 if(argc!=3)return 2;FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 TaxiTitleBackground a={0};TaxiPaletteRegistration s={0};TaxiPaletteBanks b;memset(&b,165,sizeof b);
 if(taxi_title_background_load(&a,&s,&b,rom,sizeof rom))return 4;
 f=fopen(argv[2],"wb");if(!f)return 5;fwrite(a.graphics,1,sizeof a.graphics,f);fwrite(s.records,1,sizeof s.records,f);fwrite(&b,1,sizeof b,f);
 printf("%u %u %u %u %u %u\n",a.control,a.blend,a.alpha,s.count,s.bg_bytes,s.obj_bytes);return fclose(f)!=0;
}
