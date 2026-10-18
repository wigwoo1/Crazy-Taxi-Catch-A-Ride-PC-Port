#include "normal_restart_owned.h"
#include "restart_resources.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int taxi_prior_normal_restart_owned(TaxiNormalRestart *,TaxiNormalRestartOwners *);
typedef struct {
 TaxiMusicTracker tracker;TaxiMusicAsset asset;TaxiPaletteRegistration palette;TaxiDisplayMode display;TaxiGraphicsMemoryBase bases;
 TaxiNormalRestartOwners services;TaxiNormalRestartPrefixOwners prefix;TaxiNormalRestartFlow flow;uint32_t rng,random_result;uint8_t loaded,flags[3];
 unsigned calls,resources,boundary;int fail_resource,adapter;uint32_t target[8],arg[8][2],value[8];
} Case;
static int resource(void *context,uint32_t target,uint32_t a,uint32_t b,uint32_t *value){
 Case *c=context;static const uint32_t targets[]={0x0800f5e8,0x08025c18,0x08007378,0x080073c0};assert(c->resources<4&&target==targets[c->resources]);
 assert(!c->flow.prefix.qualified_1691&&!c->flow.prefix.flag_153c&&!c->flow.prefix.flag_153d);
 if(c->adapter)assert(!c->flags[0]&&!c->flags[1]&&!c->flags[2]);
 unsigned index=c->resources++;if((int)index==c->fail_resource)return -1;
 TaxiRestartResources owned={&c->palette,&c->display,&c->bases};return taxi_restart_resources_call(&owned,target,a,b,value);
}
static int request(void *context,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t a[7],uint32_t *value){
 Case *c=context;assert(s==&c->flow);
 if(s->phase==TAXI_RESTART_GRAPHICS){assert(target==0x08027994&&a[0]==0x08070e50&&a[1]==1&&c->calls==8&&c->resources==4);c->boundary++;return -1;} /* Explicit unbound next-stage boundary. */
 assert(s->phase==TAXI_RESTART_PREFIX&&c->calls<8);unsigned index=c->calls;c->target[index]=target;c->arg[index][0]=a[0];c->arg[index][1]=a[1];
 int rc=taxi_normal_restart_prefix_owned(&c->prefix,s,target,a,value);c->value[index]=*value;c->calls++;if(target==0x0801e72c)c->random_result=*value;return rc;
}
static void initialize(Case *c,const uint8_t *bytes,size_t size,unsigned enabled,uint32_t seed,unsigned dirty){
 memset(c,0,sizeof *c);memset(&c->tracker,(int)dirty,sizeof c->tracker);memset(&c->asset,(int)dirty,sizeof c->asset);memset(&c->palette,(int)dirty,sizeof c->palette);memset(&c->display,(int)dirty,sizeof c->display);memset(&c->bases,(int)dirty,sizeof c->bases);
 c->rng=seed;c->loaded=165;c->flags[0]=255;c->flags[1]=128;c->flags[2]=127;c->fail_resource=-1;
 c->palette.count=3;c->palette.bg_bytes=c->palette.obj_bytes=512;c->flow.prefix=(TaxiNormalRestart){255,128,127};c->flow.graphics.retained_sl=1;
 c->services=(TaxiNormalRestartOwners){&c->tracker,&c->asset,bytes,size,&c->rng,(uint8_t)enabled,&c->loaded,resource,c};c->prefix=(TaxiNormalRestartPrefixOwners){&c->services,c->flags,c->flags+1,c->flags+2};
}
static void same(const Case *a,const Case *b){
 assert(a->rng==b->rng&&a->loaded==b->loaded&&a->resources==b->resources&&!memcmp(&a->tracker,&b->tracker,sizeof a->tracker)&&!memcmp(&a->asset,&b->asset,sizeof a->asset)&&!memcmp(&a->palette,&b->palette,sizeof a->palette)&&!memcmp(&a->display,&b->display,sizeof a->display)&&!memcmp(&a->bases,&b->bases,sizeof a->bases)&&!memcmp(&a->flow.prefix,&b->flow.prefix,sizeof a->flow.prefix));
}
static void rejected(Case *c,uint32_t target,uint32_t a,uint32_t b){
 TaxiMusicTracker tracker=c->tracker;TaxiMusicAsset asset=c->asset;TaxiPaletteRegistration palette=c->palette;TaxiDisplayMode display=c->display;TaxiGraphicsMemoryBase bases=c->bases;uint32_t rng=c->rng,value=0x13572468;uint8_t loaded=c->loaded;
 assert(taxi_normal_restart_owned_call(&c->services,target,a,b,&value)==-1&&value==0x13572468&&c->rng==rng&&c->loaded==loaded&&!memcmp(&tracker,&c->tracker,sizeof tracker)&&!memcmp(&asset,&c->asset,sizeof asset)&&!memcmp(&palette,&c->palette,sizeof palette)&&!memcmp(&display,&c->display,sizeof display)&&!memcmp(&bases,&c->bases,sizeof bases));
}
int main(int argc,char **argv){
 assert(argc==2);uint8_t *bytes=malloc(181241);FILE *file=fopen(argv[1],"rb");assert(bytes&&file&&fread(bytes,1,181241,file)==181241&&fgetc(file)==EOF&&!fclose(file));Case *current=malloc(sizeof *current),*prior=malloc(sizeof *prior);assert(current&&prior);
 for(unsigned enabled=0;enabled<2;enabled++)for(unsigned i=0;i<256;i++)for(unsigned dirty=0;dirty<2;dirty++){
  uint32_t seed=i*0x9e3779b9u;initialize(current,bytes,181241,enabled,seed,dirty?165:0);initialize(prior,bytes,181241,enabled,seed,dirty?165:0);current->adapter=1;
  assert(!taxi_prior_normal_restart_owned(&prior->flow.prefix,&prior->services));
  assert(taxi_normal_restart_flow_begin(&current->flow,request,current)==-1&&current->flow.phase==TAXI_RESTART_ERROR&&current->boundary==1);same(current,prior);
  static const uint32_t targets[]={0x08027730,0x08026ef8,0x0801e72c,0x08027110,0x0800f5e8,0x08025c18,0x08007378,0x080073c0};for(unsigned j=0;j<8;j++)assert(current->target[j]==targets[j]);
  assert(current->display.control==0x1444&&current->bases.base_152c==0x06014000&&current->bases.base_1530==0x06014000&&!current->palette.count&&!current->flags[0]&&!current->flags[1]&&!current->flags[2]);
  printf("%u %" PRIu32 " %u %" PRIu32 " %" PRIu32 " %u %u\n",enabled,seed,dirty?165u:0u,current->rng,current->random_result,current->arg[3][0],current->loaded);
 }
 for(int failed=0;failed<4;failed++){
  initialize(current,bytes,181241,1,123,165);initialize(prior,bytes,181241,1,123,165);current->adapter=1;current->fail_resource=prior->fail_resource=failed;
  assert(taxi_prior_normal_restart_owned(&prior->flow.prefix,&prior->services)==-1&&taxi_normal_restart_flow_begin(&current->flow,request,current)==-1&&!current->boundary&&current->calls==(unsigned)failed+5);same(current,prior);assert(!current->flags[0]&&!current->flags[1]&&!current->flags[2]);
 }
 initialize(current,bytes,181241,1,123,165);rejected(current,0,0,0);rejected(current,0x08026ef8,0,0);rejected(current,0x08026ef8,0x08421fc0,1);rejected(current,0x0801e72c,1,65535);rejected(current,0x08025c18,3,1);rejected(current,0x08027730,1,0);rejected(current,0x08007378,0,1);
 uint32_t args[7]={0},value=99;current->flow.phase=TAXI_RESTART_SETUP;assert(taxi_normal_restart_prefix_owned(&current->prefix,&current->flow,0x08027730,args,&value)==-1);current->flow.phase=TAXI_RESTART_PREFIX;args[6]=1;assert(taxi_normal_restart_prefix_owned(&current->prefix,&current->flow,0x08027730,args,&value)==-1);args[6]=0;assert(taxi_normal_restart_prefix_owned(&current->prefix,&current->flow,0x0800f5e8,args,&value)==-1&&current->flags[0]==255);
 assert(taxi_normal_restart_owned_call(NULL,0x08027730,0,0,&value)==-1&&taxi_normal_restart_owned_call(&current->services,0x08027730,0,0,NULL)==-1);
 /* Loader failure retains preceding stop/loaded writes but does not consume RNG. */
 initialize(current,NULL,0,1,123,165);current->adapter=1;assert(taxi_normal_restart_flow_begin(&current->flow,request,current)==-1&&current->calls==2&&!current->resources&&current->rng==123&&!current->loaded&&!current->tracker.state.playing&&!current->tracker.state.state[0]&&current->flags[0]==255);
 free(current);free(prior);free(bytes);puts("PASS1024prefix state/trace cases,4partial-resource failures, invalid requests and partial-loader retention");return 0;
}
