#include "../src/title_parent.h"
#include <stdio.h>
#include <string.h>
typedef struct {unsigned seed,limit,count;int snapshots;} Fixture;
static int service(void *context,TaxiTitleParent *s,uint32_t target,uint32_t arg){
 Fixture *f=context;if(f->count>=64)return -1;
 printf("%u:%u,",target,arg);if(f->snapshots)printf("[%u:%u:%u:%u]",s->active_3898,s->flags_3899,s->menu_3897,s->callback_3878);f->count++;
 s->flags_3899=(uint8_t)(f->seed+f->count*13);
 s->active_3898=f->count>=f->limit?0:(uint8_t)(f->count%3==0?255:f->count%3==1?128:1);
 return 0;
}
int main(int argc,char **argv){int snapshots=argc==2&&!strcmp(argv[1],"--resumable-snapshots");int resumable=snapshots||(argc==2&&!strcmp(argv[1],"--resumable"));unsigned seed,limit,active,flags,menu,callback;
 while(scanf("%u %u %u %u %u %u",&seed,&limit,&active,&flags,&menu,&callback)==6){
  Fixture f={seed,limit,0,snapshots};TaxiTitleParent s={(uint8_t)active,(uint8_t)flags,(uint8_t)menu,callback};
  if(resumable){
   TaxiTitleParentContinuation c={0};int pending=taxi_title_parent_begin(&c,&s);
   while(pending>0){uint32_t target=0,arg=0;
    if(pending==TAXI_TITLE_PARENT_TITLE){target=0x08016250;arg=5;}
    else if(pending==TAXI_TITLE_PARENT_INTRO)target=0x08016250;
    else if(pending==TAXI_TITLE_PARENT_PREPARE_ATTRACT)target=0x0800de64;
    else if(pending==TAXI_TITLE_PARENT_ATTRACT)target=0x0800abec;
    else return 3;
    if(service(&f,&s,target,arg))return 4;
    pending=taxi_title_parent_resume(&c);
   }
   if(pending||taxi_title_parent_resume(&c)!=-1)return 5;
  }else if(taxi_title_parent_run(&s,service,&f))return 2;
  printf(" %u %u %u %u\n",s.active_3898,s.flags_3899,s.menu_3897,s.callback_3878);
 }return 0;}
