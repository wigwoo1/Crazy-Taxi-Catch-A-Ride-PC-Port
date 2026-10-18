#include "title_background.h"
#include <string.h>
static uint32_t word(const uint8_t *r,size_t n){return (uint32_t)r[n]|(uint32_t)r[n+1]<<8|(uint32_t)r[n+2]<<16|(uint32_t)r[n+3]<<24;}
static uint16_t half(const uint8_t *r,size_t n){return (uint16_t)(r[n]|r[n+1]<<8);}
int taxi_title_background_load(TaxiTitleBackground *a,TaxiPaletteRegistration *s,TaxiPaletteBanks *b,const uint8_t *r,size_t size){
 const size_t d=0x3415c;uint8_t tiles[8032];uint16_t colors[16];TaxiPaletteTransfer transfer;
 if(!a||!s||!b||!r||size<d+132||s->count||s->bg_bytes||s->obj_bytes)return -1;
 /*5778 writes retained globals before processing layers; hardware alpha below
    remains its separately executed source publication. */
 if(taxi_presentation_blend_load(&a->retained_blend,r+d,size-d))return -1;
 if(half(r,d+56)!=32||half(r,d+64)!=8032||half(r,d+72)!=64||half(r,d+80)!=32||r[d+124]!=1)return -1;
 uint32_t map=word(r,d),pal=word(r,d+16),compressed=word(r,d+32);
 if(map<0x08000000u||pal<0x08000000u||compressed<0x08000000u)return -1;
 size_t m=map-0x08000000u,p=pal-0x08000000u,c=compressed-0x08000000u;
 if(m>size||size-m<4096||p>size||size-p<32||c>size)return -1;
 for(unsigned tile=0;tile<251;tile++){
  unsigned n=0;while(n<32){
   if(c>=size)return -1;unsigned command=r[c++],count=command&127u;if(!count||count>32-n)return -1;
   size_t used=(command&128u)?count:1;if(used>size-c)return -1;
   if(command&128u)memcpy(tiles+tile*32+n,r+c,count);else memset(tiles+tile*32+n,r[c],count);
   c+=used;n+=count;
  }
 }
 for(unsigned i=0;i<16;i++)colors[i]=half(r,p+i*2);
 if(taxi_palette_register_small(s,colors,pal,0,1,&transfer)<0)return -1;
 if(taxi_palette_transfer_small_apply(b,&transfer,pal,colors))return -1;
 memcpy(a->graphics,tiles,sizeof tiles);
 for(unsigned y=0;y<32;y++)for(unsigned x=0;x<64;x++){
  size_t dst=8192+(x/32)*2048+(y*32+x%32)*2,src=m+(y*64+x)*2;
  a->graphics[dst]=r[src];a->graphics[dst+1]=r[src+1];
 }
 a->control=0x4440;a->blend=0x1f40;a->alpha=0x0808;
 s->count=4;s->bg_bytes=128;return 0;
}
