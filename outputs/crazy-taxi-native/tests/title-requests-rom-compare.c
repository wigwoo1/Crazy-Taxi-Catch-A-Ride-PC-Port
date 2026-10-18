#include "../src/title_requests.h"
#include <stdio.h>
#ifdef TAXI_TITLE_PUBLISH
#include "../src/title_publish.h"
#endif
#include <string.h>
static unsigned char rom[8388608];static TaxiTitleLoad s;
int main(int argc,char **argv){if(argc!=2)return 2;FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 unsigned clock,fade;
 while(scanf("%u %u",&clock,&fade)==2){
  memset(&s,0,sizeof s);s.assets.bases[10]=0x60163a0;s.assets.palette_bases[10]=15;
  for(unsigned b=0;b<3;b++)for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)s.queue.attributes[b][i][j]=(uint16_t)(165+b*1000+i*37+j*53);
  for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)s.queue.depth[i][j]=(uint16_t)(165+i*37+j*53);
  s.queue.last_width=99;s.queue.last_height=165;
  TaxiTitleCallback callback={.sound_done=1,.scroll={34,69632}};TaxiTitleCallbackOutput out;uint32_t random=12345;
  if(taxi_title_callback(&callback,&random,0,1,clock,(uint8_t)fade,240,160,&out)||taxi_title_requests_queue(&s,&out,rom,sizeof rom,0x2001))return 4;
  #ifdef TAXI_TITLE_PUBLISH
  for(unsigned i=0;i<128;i++){s.base_template[i][0]=0x2a1;s.base_template[i][1]=0xf1;}
  memset(s.matrix_template,165,sizeof s.matrix_template);
  if(taxi_title_objects_publish(&s))return 5;
#endif
  printf("%u %u %u %u %u %u ",s.queue.total,s.queue.count[0],s.queue.count[1],s.queue.count[2],s.queue.last_width,s.queue.last_height);
  const unsigned char *data=(const unsigned char *)s.queue.attributes;for(unsigned i=0;i<sizeof s.queue.attributes;i++)printf("%02x",data[i]);data=(const unsigned char *)s.queue.depth;for(unsigned i=0;i<sizeof s.queue.depth;i++)printf("%02x",data[i]);
#ifdef TAXI_TITLE_PUBLISH
  data=(const unsigned char *)s.transport.output;for(unsigned i=0;i<sizeof s.transport.output;i++)printf("%02x",data[i]);
  data=(const unsigned char *)s.transport.matrices;for(unsigned i=0;i<sizeof s.transport.matrices;i++)printf("%02x",data[i]);
#endif
  puts("");
 }
 return 0;}
