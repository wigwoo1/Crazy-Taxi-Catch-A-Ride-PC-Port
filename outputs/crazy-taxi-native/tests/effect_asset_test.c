#include "effect_asset.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){
 assert(argc==3);TaxiEffectAsset a={0};assert(!taxi_effect_asset_load(&a,argv[1]));
 assert(a.count==38&&a.pixel_bytes==39168);unsigned checked=0;
 for(unsigned c=0;c<2;c++)for(unsigned g=8;g<=10;g++)
  for(unsigned f=0;f<(g==8?4:g==9?3:9);f++)for(unsigned b=0;b<(g==9?2:1);b++){
   TaxiEffectFrame frame;assert(taxi_effect_asset_frame(&a,c,g,f,b,&frame));
   if(g==10&&f==4){
    assert(frame.width==(c?8:16)&&frame.height==(c?8:32)&&frame.bytes==(c?256:2048));
   }else assert(frame.width==16&&frame.height==16&&frame.bytes==1024);
   for(size_t i=3;i<frame.bytes;i+=4)assert(frame.rgba[i]==0||frame.rgba[i]==255);
   checked++;
  }
 assert(checked==38);TaxiEffectFrame one,two;
 assert(taxi_effect_asset_frame(&a,0,9,0,0,&one));
 assert(taxi_effect_asset_frame(&a,0,9,0,1,&two));
 assert(memcmp(one.rgba,two.rgba,1024));
 TaxiEffectFrame kept=one;assert(!taxi_effect_asset_frame(&a,0,10,9,0,&one));
 assert(!memcmp(&kept,&one,sizeof one));
 /* A corrupt pixel offset must be rejected without accepting partial data. */
 unsigned char original=a.storage[40];a.storage[40]^=1;
 FILE *file=fopen(argv[2],"wb");assert(file);
 assert(fwrite(a.storage,1,a.size,file)==a.size);assert(!fclose(file));a.storage[40]=original;
 TaxiEffectAsset bad={0};assert(taxi_effect_asset_load(&bad,argv[2])==-1&&bad.storage==NULL);
 assert(taxi_effect_asset_load(&a,argv[2])==-1&&a.storage);
 assert(!remove(argv[2]));taxi_effect_asset_free(&a);assert(!a.storage);
 puts("Effect assets: all38 original frames/banks, distinct tire palettes, transparency and corrupt offset rejection PASS");
 return 0;
}
