#include "../src/title_load.h"
#include "../src/title_entry.h"
#include <stdio.h>
#include <string.h>
static unsigned char rom[8388608];static TaxiTitleLoad s;
static unsigned half(const unsigned char *p){return p[0]|(unsigned)p[1]<<8;}
static unsigned word(const unsigned char *p){return p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;}
int main(int argc,char **argv){
 if(argc!=2)return 2;FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 unsigned cases=0;
 for(unsigned language=0;language<4;language++)for(unsigned interval=0;interval<4;interval++){
  memset(&s,0,sizeof s);memset(&s.banks,165,sizeof s.banks);memset(&s.fonts,165,sizeof s.fonts);
  memset(s.matrix_template,165,sizeof s.matrix_template);
  s.fade.fade.interval_1771=(uint8_t)interval;s.fade.fade.reload_1772=3;
  if(taxi_title_load(&s,rom,sizeof rom,language))return 4;
  if(s.queue.total||s.queue.count[0]||s.queue.count[1]||s.queue.count[2]||s.transport.matrix_count||memcmp(s.transport.matrices,s.matrix_template,sizeof s.matrix_template))return 13;
  for(unsigned i=0;i<128;i++){
   if(s.base_template[i][0]!=0x2a1||s.base_template[i][1]!=0xf1||s.base_template[i][2]||s.base_template[i][3]||memcmp(s.queue.attributes[0][i],s.base_template[i],8))return 14;
   if(s.transport.output[i][0]!=0x20a1||s.transport.output[i][1]!=0xf1||s.transport.output[i][2]!=0x200||s.transport.output[i][3])return 15;
  }
  if(s.registry.count!=20||s.registry.bg_bytes!=128||s.registry.obj_bytes!=512||s.fade.fade.interval_1771!=interval||s.fade.fade.reload_1772!=3||s.fade.bldy!=16||s.fade.bldcnt!=255)return 5;
  for(unsigned i=0;i<20;i++){
   unsigned char *record=s.registry.records[i];if(!record[6])continue;
   unsigned offset=half(record+2)/2,source=word(record+16)-0x8000000;
   unsigned short *bank=record[7]?s.banks.background:s.banks.objects;
   for(unsigned j=0;j<16;j++)if(bank[offset+j]!=half(rom+source+j*2))return 6;
  }
  for(unsigned i=16;i<256;i++)if(s.banks.background[i]!=0xa5a5)return 7;
  if(s.fonts.cursor||s.fonts.descriptors[2][32]!=165||s.assets.bases[10]!=0x60163a0)return 8;
  TaxiTitleRuntime runtime;memset(&runtime,165,sizeof runtime);TaxiMenuInput retained=runtime.input;
  if(taxi_title_entry(&runtime,&s)||memcmp(&runtime.input,&retained,sizeof retained)||runtime.clock||runtime.finished||runtime.callback.sound_done||runtime.callback.scroll.timer||runtime.callback.scroll.x_q8||runtime.flow.remaining!=900||runtime.fade.interval_1771!=interval||runtime.fade.reload_1772!=3||runtime.bldy!=16)return 12;
  uint16_t mosaic=123;unsigned frames=0;
  while(s.fade.fade.active_177b){if(++frames>200)return 9;if(taxi_palette_fade_service_normal(&s.banks,&s.registry,&s.fade,NULL,0,1,240,160,&mosaic))return 10;}
  if(s.fade.bldy||mosaic)return 11;
  printf("language=%u interval=%u reload=3 fade_frames=%u PASS\n",language,interval,frames);cases++;
 }
 printf("PASS %u title load integrations\n",cases);return 0;
}
