#include "startup_init_owned.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {unsigned screen,save;} Children;
static int child(void *pointer,TaxiStartupInit *s,uint32_t target,uint32_t a,uint32_t b){
 Children *c=pointer;
 if(target==0x08016250){if(a||b)return -1;s->retained_1ea5=(uint8_t)c->screen;return 0;}
 if(target==0x08010298){if(a!=1||b!=1)return -1;s->retained_1ea5=(uint8_t)c->save;return 0;}
 return -1;
}
static int write_state(FILE *out,const TaxiStartupInit *s,const TaxiStartupInitOwners *owners){
 const TaxiInput *input=owners->input;
 unsigned char header[32]={s->retained_1ea5,s->initialized_1ea4,s->child_signal_3892,s->selected_font};
 for(unsigned i=0;i<4;i++)header[4+i]=(unsigned char)(s->dispatcher_3878>>(i*8));
 for(unsigned j=0;j<6;j++)for(unsigned i=0;i<4;i++)header[8+j*4+i]=(unsigned char)(s->font_assets[j]>>(i*8));
 unsigned char tail[3]={*owners->music_volume,*owners->effects_volume,*owners->language};
 return fwrite(header,1,32,out)==32&&fwrite(owners->settings,1,512,out)==512&&fwrite(owners->runtime_rows,1,800,out)==800&&sizeof *input==66&&fwrite(input,1,66,out)==66&&fwrite(tail,1,3,out)==3;
}
int main(int argc,char **argv){
 if(argc!=7&&argc!=8)return 1;int resumable=argc==8&&!strcmp(argv[7],"--resumable");if(argc==8&&!resumable)return 1;unsigned seed=(unsigned)strtoul(argv[2],NULL,10);Children children={(unsigned)strtoul(argv[4],NULL,10),(unsigned)strtoul(argv[5],NULL,10)};
 FILE *rom=fopen(argv[1],"rb");if(!rom)return 2;unsigned char table[80],settings[512],rows[800];
 if(fseek(rom,0x4a54c4,SEEK_SET)||fread(table,1,80,rom)!=80){fclose(rom);return 3;}fclose(rom);
 TaxiInput input;TaxiStartupInit s;memset(&input,(int)seed,sizeof input);memset(&s,(int)seed,sizeof s);memset(settings,(int)seed,512);memset(rows,(int)seed,800);
 s.retained_1ea5=(uint8_t)strtoul(argv[3],NULL,10);uint8_t music=(uint8_t)seed,effects=(uint8_t)seed,language=(uint8_t)seed;
 TaxiStartupInitOwners owners={settings,rows,table,&input,&music,&effects,&language};
 if(!resumable){if(taxi_startup_init_owned(&s,&owners,child,&children))return 4;}
 else {
  char pending_path[1024];if(snprintf(pending_path,sizeof pending_path,"%s.pending",argv[6])<0)return 7;
  FILE *pending=fopen(pending_path,"wb");if(!pending)return 8;
  TaxiStartupInitContinuation c={0};int stage=taxi_startup_init_owned_begin(&c,&s,&owners);unsigned steps=0;
  while(stage>0){
   if(++steps>2||fputc(stage,pending)==EOF||!write_state(pending,&s,&owners))return 9;
   if(child(&children,&s,stage==TAXI_STARTUP_INIT_SCREEN?0x08016250:0x08010298,stage==TAXI_STARTUP_INIT_SAVE,stage==TAXI_STARTUP_INIT_SAVE))return 10;
   stage=taxi_startup_init_owned_resume(&c);
  }
  if(fclose(pending)||stage!=0||taxi_startup_init_owned_resume(&c)!=-1)return 11;
 }
 FILE *out=fopen(argv[6],"wb");if(!out)return 5;
 int ok=write_state(out,&s,&owners);
 if(fclose(out))ok=0;return ok?0:6;
}
