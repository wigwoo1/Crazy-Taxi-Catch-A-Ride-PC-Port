#include "effect_asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t u32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint16_t u16(const uint8_t *p){return (uint16_t)(p[0]|(uint16_t)p[1]<<8);}
void taxi_effect_asset_free(TaxiEffectAsset *a){if(a){free(a->storage);memset(a,0,sizeof *a);}}
int taxi_effect_asset_load(TaxiEffectAsset *out,const char *path){
 if(!out||out->storage||!path)return -1;
 FILE *f=fopen(path,"rb");if(!f)return -1;
 if(fseek(f,0,SEEK_END)){fclose(f);return -1;}
 long length=ftell(f);
 if(length<12||length>1024*1024||fseek(f,0,SEEK_SET)){fclose(f);return -1;}
 TaxiEffectAsset a={.size=(size_t)length};a.storage=malloc(a.size);
 if(!a.storage){fclose(f);return -1;}
 int valid=fread(a.storage,1,a.size,f)==a.size&&fgetc(f)==EOF;fclose(f);
 if(!valid||memcmp(a.storage,"CTE1",4))goto bad;
 a.count=u32(a.storage+4);a.pixel_bytes=u32(a.storage+8);
 if(!a.count||a.count>256||12u+36u*(size_t)a.count+a.pixel_bytes!=a.size)goto bad;
 size_t cursor=0;
 for(uint32_t j=0;j<a.count;j++){
  const uint8_t *p=a.storage+12+36*j;
  unsigned w=u16(p+22),h=u16(p+24),tx=u16(p+4),ty=u16(p+6);
  if(p[0]>1||p[1]<8||p[1]>10||p[2]>=(p[1]==8?4:p[1]==9?3:9)||
   p[3]>=(p[1]==9?2:1)||!tx||tx>8||!ty||ty>8||w!=tx*8||h!=ty*8||
   u16(p+16)>w||u16(p+18)>h||p[26]||p[27]||u32(p+28)!=cursor||
   u32(p+32)!=(uint32_t)w*h*4||cursor+(size_t)w*h*4>a.pixel_bytes)goto bad;
  for(uint32_t k=0;k<j;k++)if(!memcmp(p,a.storage+12+36*k,4))goto bad;
  cursor+=(size_t)w*h*4;
 }
 if(cursor!=a.pixel_bytes)goto bad;
 *out=a;return 0;
bad:taxi_effect_asset_free(&a);return -1;
}
int taxi_effect_asset_frame(const TaxiEffectAsset *a,unsigned container,unsigned group,
 unsigned frame,unsigned bank,TaxiEffectFrame *out){
 if(!a||!a->storage||!out||container>1||group<8||group>10||frame>255||bank>255)return 0;
 for(uint32_t j=0;j<a->count;j++){
  const uint8_t *p=a->storage+12+36*j;
  if(p[0]!=container||p[1]!=group||p[2]!=frame||p[3]!=bank)continue;
  TaxiEffectFrame f={.width=u16(p+22),.height=u16(p+24),.bytes=u32(p+32)};
  for(unsigned k=0;k<9;k++)f.metrics[k]=u16(p+4+2*k);
  f.rgba=a->storage+12+36*a->count+u32(p+28);*out=f;return 1;
 }
 return 0;
}
