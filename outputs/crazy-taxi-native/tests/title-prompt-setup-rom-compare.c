#include "../src/title_prompt_setup.h"
#include <stdio.h>
#include <string.h>
static unsigned char rom[8388608];
static uint32_t word(unsigned n){return (uint32_t)rom[n]|(uint32_t)rom[n+1]<<8|(uint32_t)rom[n+2]<<16|(uint32_t)rom[n+3]<<24;}
static uint16_t half(unsigned n){return (uint16_t)(rom[n]|rom[n+1]<<8);}
int main(int argc,char **argv){if(argc!=2)return 2;FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 unsigned resource=0x34634,g=resource+word(resource+44);TaxiSpriteCopyAsset assets[4];uint32_t languages[4];
 for(unsigned i=0;i<4;i++){unsigned m=resource+word(g+20+4*i);assets[i]=(TaxiSpriteCopyAsset){word(resource),word(g),word(g+4),0x060163a0,half(m),half(m+2),half(m+16)};languages[i]=word(0x7ebf48+4*i);}
 for(unsigned language=0;language<4;language++){TaxiPaletteRegistration registry={0};registry.count=11;memset(registry.records,165,sizeof registry.records);TaxiTitlePromptSetup out;
  if(taxi_title_prompt_setup(&registry,languages,language,assets,&out))return 4;
  printf("%u",language);for(unsigned i=0;i<4;i++)printf(" %u %u %u",out.frames[i].source,out.frames[i].destination,out.frames[i].control);
  printf(" %u %u %u ",out.palette.source,out.palette.destination,out.palette.control);
  for(unsigned i=0;i<sizeof registry.records;i++)printf("%02x",((unsigned char *)registry.records)[i]);puts("");
 }return 0;}
