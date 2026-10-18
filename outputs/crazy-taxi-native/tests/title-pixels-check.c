#include "../src/title_pixels.h"
#include "../src/title_requests.h"
#include "../src/title_publish.h"
#include <stdio.h>
#include <stdlib.h>
static unsigned char rom[8388608];static TaxiTitleLoad s;static uint16_t pixels[240*160];
int main(int argc,char **argv){if(argc!=7)return 2;FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 if(taxi_title_load(&s,rom,sizeof rom,(unsigned)atoi(argv[2])))return 4;
 TaxiTitleCallback callback={.sound_done=1,.scroll={34,69632}};TaxiTitleCallbackOutput requests;uint32_t random=0;
 if(taxi_title_callback(&callback,&random,0,1,atoi(argv[5])?0:112,0,240,160,&requests)||taxi_title_requests_queue(&s,&requests,rom,sizeof rom,0x2001)||taxi_title_objects_publish(&s))return 5;
 if(taxi_title_pixels(&s,(unsigned)atoi(argv[3]),(uint16_t)atoi(argv[4]),pixels))return 6;
 f=fopen(argv[6],"wb");if(!f)return 7;fwrite(pixels,1,sizeof pixels,f);return fclose(f)!=0;}
