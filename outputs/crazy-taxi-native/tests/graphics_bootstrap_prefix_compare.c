#include "graphics_bootstrap_prefix.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){unsigned cursor;while(scanf("%u",&cursor)==1){TaxiGraphicsBootstrapPrefix out={0};TaxiRestartRenderContext render={.state_2c4ec=cursor};assert(!taxi_graphics_bootstrap_prefix(&out,&render));printf("%u %u %u %u %u",render.state_2c4ec,out.image_source,out.image_identity,out.image_bytes,out.initialized);for(unsigned i=0;i<42;i++)printf(" %u %u",out.callbacks[i].owner,out.callbacks[i].entry);for(unsigned i=0;i<3;i++)printf(" %u %u",out.internal[i].source_site,out.internal[i].entry);puts("");assert(!taxi_graphics_bootstrap_binding(&out,0));for(unsigned i=0;i<42;i++)assert(taxi_graphics_bootstrap_binding(&out,out.callbacks[i].owner)==out.callbacks[i].entry);}
 const unsigned invalid[]={1,2,3,677,680,0xffffffff};for(unsigned i=0;i<sizeof invalid/sizeof invalid[0];i++){TaxiGraphicsBootstrapPrefix out;memset(&out,0xa5,sizeof out);TaxiGraphicsBootstrapPrefix before=out;TaxiRestartRenderContext r={.state_2c4ec=invalid[i]};assert(taxi_graphics_bootstrap_prefix(&out,&r)==-1&&r.state_2c4ec==invalid[i]&&!memcmp(&out,&before,sizeof out));}return 0;}
