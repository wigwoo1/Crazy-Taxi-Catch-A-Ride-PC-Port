#include "../src/world_render.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {unsigned count,refs[16];int fail;} Requests;
static int consume(void *ctx,unsigned ref){Requests *r=ctx;r->refs[r->count++]=ref;return r->fail?-1:0;}
int main(void){uint8_t a[36]={4},b[34]={3};unsigned ra[]={0,1,0x8000,0},rb[]={1,2,0x8000};for(unsigned j=0;j<4;j++){a[28+j*2]=(uint8_t)ra[j];a[29+j*2]=(uint8_t)(ra[j]>>8);}for(unsigned j=0;j<3;j++){b[28+j*2]=(uint8_t)rb[j];b[29+j*2]=(uint8_t)(rb[j]>>8);}const uint8_t *cells[]={a,b};TaxiWorldAsset w={.nx=2,.nz=1,.np=3,.cells=cells};uint32_t stamps[3]={0};Requests r={0};
 assert(!taxi_world_render_visit_stamped(&w,0,0,1,0,stamps,3,1,NULL,consume,&r));assert(r.count==5&&r.refs[0]==0&&r.refs[1]==1&&r.refs[2]==0x8000&&r.refs[3]==2&&r.refs[4]==0x8000);
 r=(Requests){0};assert(!taxi_world_render_visit_stamped(&w,0,0,1,0,stamps,3,1,NULL,consume,&r));assert(r.count==2&&r.refs[0]==0x8000&&r.refs[1]==0x8000);
 r=(Requests){0};assert(!taxi_world_render_visit_stamped(&w,0,0,1,0,stamps,3,2,NULL,consume,&r));assert(r.count==5);
 memset(stamps,0,sizeof stamps);r=(Requests){0};assert(!taxi_world_render_visit_stamped(&w,0,0,1,0,stamps,3,0,NULL,consume,&r));assert(r.count==2);
 r=(Requests){.fail=1};assert(taxi_world_render_visit_stamped(&w,0,0,1,0,stamps,3,7,NULL,consume,&r)==-1);assert(stamps[0]==7&&stamps[1]==0);
 uint8_t seen[3];r=(Requests){0};assert(!taxi_world_render_visit(&w,0,0,1,0,seen,3,NULL,consume,&r));assert(r.count==5);r=(Requests){0};assert(!taxi_world_render_visit(&w,0,0,1,0,seen,3,NULL,consume,&r));assert(r.count==5);
 uint32_t normal_cache[3]={0},special_cache[1]={0};w.special_count=1;r=(Requests){0};
 assert(!taxi_world_render_visit_cached(&w,0,0,1,0,normal_cache,3,special_cache,1,1,NULL,consume,&r));assert(r.count==4&&special_cache[0]==1);
 r=(Requests){0};assert(!taxi_world_render_visit_cached(&w,0,0,1,0,normal_cache,3,special_cache,1,1,NULL,consume,&r));assert(r.count==0);
 r=(Requests){0};assert(!taxi_world_render_visit_cached(&w,0,0,1,0,normal_cache,3,special_cache,1,2,NULL,consume,&r));assert(r.count==4&&special_cache[0]==2);
 r=(Requests){0};assert(taxi_world_render_visit_cached(&w,0,0,1,0,normal_cache,3,special_cache,0,3,NULL,consume,&r)==-1);assert(r.count==0&&special_cache[0]==2);
 puts("PASS persistent/repeated/zero generation, retained cache, special references, callback failure and legacy traversal.");return 0;}
