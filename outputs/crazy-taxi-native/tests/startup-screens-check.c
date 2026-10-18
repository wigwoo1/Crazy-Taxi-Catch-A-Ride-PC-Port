#include "../src/startup_screens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned position,stops,frames;
static int platform(void *unused,TaxiStartupScreens *s,uint32_t target,uint32_t arg){
 (void)unused;static const uint32_t order[]={0x08016208,0x08015c3c,0x08027cd0,0x08028ee0};
 if(target==0x0802763c){stops++;return 0;}
 if(position>=4||target!=order[position++])return -1;
 if(target==0x08016208){if(arg!=1)return -1;s->presentation.current_3520++;}
 return 0;
}
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|p[1]<<8);}
static uint32_t word(const uint8_t *p){return (uint32_t)half(p)|(uint32_t)half(p+2)<<16;}
int main(int argc,char **argv){
 if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;
 uint8_t *rom=malloc(8388608);if(!rom)return 3;
 if(fread(rom,1,8388608,f)!=8388608)return 4;fclose(f);
 for(unsigned input=0;input<2;input++){
  TaxiStartupScreens *s=calloc(1,sizeof *s);if(!s)return 5;s->presentation.delta_3b00=1;
  for(unsigned state=0;state<3;state++){
   const uint8_t *raw=rom+0x49d560+state*28;
   TaxiScreenDescriptor d={0};d.image=word(raw);d.timer=word(raw+4);d.fade_in_bg=half(raw+8);d.fade_in_obj=half(raw+10);d.fade_out_bg=half(raw+12);d.fade_out_obj=half(raw+14);d.type=(int8_t)raw[24];d.flags=raw[25];
   if(taxi_startup_screens_load(s,&d,rom+d.image-0x08000000,8388608-(d.image-0x08000000)))return 6;
   if(!s->fade.fade.active_177b||s->fade.bldy!=16||memcmp(s->banks.background,s->source_palette,512))return 7;
   frames=0;unsigned in_end=0,out_start=0;int result=0;
   for(;frames<500;){position=0;unsigned was=s->fade.fade.mode_1773;
    result=taxi_startup_screens_frame(s,(uint8_t)input,0,platform,NULL);frames++;
    if(result<0||position!=4)return 8;
    if(!in_end&&!s->fade.fade.active_177b){in_end=frames;if(s->fade.bldy)return 9;}
    if(was==1&&s->fade.fade.mode_1773==0)out_start=frames;
    if(result==1)break;
   }
   unsigned expected=input&&state?125:(state?365:245);
   if(result!=1||frames!=expected||in_end!=33||s->fade.bldy!=16||s->sequence.flow.remaining_359c)return 10;
   TaxiScreenDescriptor next={0};next.type=state==2?7:-1;next.image=state==2?0:1;
   unsigned old_stops=stops;if(taxi_startup_screens_advance(s,&next,platform,NULL)!=1||stops!=old_stops+1||s->sequence.index!=state+1)return 11;
   printf("PASS screen%u held-input%u frames%u fade-in-end%u fade-out-start%u\n",state,input,frames,in_end,out_start);
  }free(s);
 }free(rom);return 0;
}
