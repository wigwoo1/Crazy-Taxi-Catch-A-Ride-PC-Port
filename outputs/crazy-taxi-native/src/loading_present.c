#include "loading_present.h"
static int invoke(TaxiLoadingPresent *s,TaxiLoadingPresentCall call,void *c,uint32_t target,const uint32_t a[9],uint32_t *pos,uint32_t *out){uint32_t ignored=0;return call(c,s,target,a,pos,out?out:&ignored);}
int taxi_loading_present(TaxiLoadingPresent *s,TaxiLoadingPresentCall call,void *c){
 if(!s||!call)return -1;
 uint32_t a[9]={0,0,s->gate.width_7542,s->gate.height_7544};
 if(invoke(s,call,c,0x0802c9f0,a,0,0))return -1;
 if((s->gate.settings_1eb0&0xe0)==0x80||s->gate.override_1708)return 0;
 for(unsigned i=0;i<2;i++){
  uint32_t lookup[9]={0x94+i},text=0;
  if(invoke(s,call,c,0x08028fe4,lookup,0,&text))return -1;
  uint32_t args[9]={0,s->gate.width_7542/2u,s->gate.height_7544/2u+(i?1u:0u-9u),text,0,0,4,0,256};
  if(invoke(s,call,c,0x0800f7dc,args,0,0))return -1;
 }
 uint32_t pos[3]={72,120,0};
 for(unsigned i=0;i<3;i++){
  uint32_t args[9]={i,7,0,0,1,0,0};
  if(invoke(s,call,c,0x08028288,args,pos,0))return -1;
  if(i<2)pos[0]+=32;
 }
 for(unsigned i=0;i<3;i++){
  uint32_t args[9]={0,i*60,0x00400040,0x080eb884,1};
  if(invoke(s,call,c,0x08031ef8,args,0,0))return -1;
 }
 int animation=s->animation_1684<128?s->animation_1684:(int)s->animation_1684-256;
 uint32_t sprite[9]={0x080fd5fc+(uint32_t)(animation*1024),0,0,2,1};
 if(invoke(s,call,c,0x08027fb8,sprite,0,0))return -1;
 pos[0]=120;pos[1]=24;pos[2]=0;
 uint32_t final[9]={0,2,0,0,1,0,0},zero[9]={0};
 if(invoke(s,call,c,0x08028288,final,pos,0)||invoke(s,call,c,0x08026318,zero,0,0))return -1;
 return 0;
}
