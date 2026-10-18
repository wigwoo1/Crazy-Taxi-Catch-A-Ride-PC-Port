#include "credits_asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int read_file(const char *dir,const char *name,void *data,size_t n){
 char path[1024];int len=snprintf(path,sizeof path,"%s/%s",dir,name);
 if(len<0||(size_t)len>=sizeof path)return -1;
 FILE *f=fopen(path,"rb");if(!f)return -1;
 int ok=fread(data,1,n,f)==n&&fgetc(f)==EOF&&!ferror(f);fclose(f);return ok?0:-1;
}
static uint16_t word(const uint8_t *p){return (uint16_t)(p[0]|(unsigned)p[1]<<8);}
int taxi_credits_asset_load(TaxiCreditsAsset *out,const char *dir){
 if(!out||!dir)return -1;
 TaxiCreditsAsset *a=calloc(1,sizeof *a);if(!a)return -1;
 uint8_t raw[2048];
 if(read_file(dir,"layer0.tiles",a->font,sizeof a->font)||read_file(dir,"layer1.tiles",a->background,sizeof a->background)||
    read_file(dir,"lines.bin",a->lines,sizeof a->lines))goto bad;
 for(unsigned k=0;k<2;k++){
  if(read_file(dir,k?"layer1.map":"layer0.map",raw,sizeof raw))goto bad;
  for(unsigned j=0;j<1024;j++){
   uint16_t e=word(raw+j*2);if((e&1023)>=(k?250:128)||(e>>12))goto bad;
   a->map[k][j]=(uint16_t)(e|(k?0x1000:0)); /* Original descriptor palette allocations0/1. */
  }
 }
 for(unsigned bank=0;bank<7;bank++){
  char name[64];if(bank<2)snprintf(name,sizeof name,"layer%u.rgb555",bank);
  else snprintf(name,sizeof name,"caller-palette%u.rgb555",bank-2);
  if(read_file(dir,name,raw,32))goto bad;
  for(unsigned j=0;j<16;j++)a->palette[bank][j]=word(raw+j*2);
 }
 for(unsigned k=0;k<104;k++){
  if(a->lines[k][0]!='0'&&a->lines[k][0]!='1')goto bad;
  for(unsigned j=1;j<31;j++)if(a->lines[k][j]<32||a->lines[k][j]>=160)goto bad;
 }
 *out=*a;free(a);return 0;
bad:free(a);return -1;
}
int taxi_credits_row_write(TaxiCreditsAsset *a,unsigned row,const TaxiCreditsFrame *f){
 if(!a||!f||row>992||(row&31))return -1;
 if(!f->write_row)return 0;
 for(unsigned j=0;j<30;j++)if((f->tiles[j]&1023)>=128||(f->tiles[j]>>12)>=7)return -1;
 memcpy(a->map[0]+row,f->tiles,sizeof f->tiles);return 0;
}
static unsigned index_at(const TaxiCreditsAsset *a,unsigned layer,unsigned x,unsigned y,unsigned *bank){
 x&=255;y&=255;uint16_t e=a->map[layer][(y/8)*32+x/8];
 unsigned tile=e&1023;*bank=e>>12;
 unsigned xx=e&1024?7-(x&7):x&7,yy=e&2048?7-(y&7):y&7;
 const uint8_t *data=layer?a->background:a->font;
 return (data[tile*32+yy*4+xx/2]>>(4*(xx&1)))&15;
}
int taxi_credits_compose(const TaxiCreditsAsset *a,const uint16_t scroll[3],uint16_t out[240*160]){
 if(!a||!scroll||!out)return -1;
 for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
  unsigned bank=0,idx=index_at(a,0,x,y+scroll[0],&bank);
  if(!idx)idx=index_at(a,1,x+scroll[1],y+scroll[2],&bank);
  out[y*240+x]=idx?a->palette[bank][idx]:a->palette[0][0];
 }
 return 0;
}
