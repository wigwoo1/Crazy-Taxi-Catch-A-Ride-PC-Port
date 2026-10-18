#include "restart_render_context.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned depth,color,present;while(scanf("%u%u%u",&depth,&color,&present)==3){uint8_t lookup[2560];memset(lookup,165,sizeof lookup);TaxiRestartRenderContext s={.shade_lookup=lookup,.shade_lookup_size=sizeof lookup,.engine_present=(uint8_t)present,.shade_depth_6854=123,.shade_color_6858=456};uint32_t args[7]={depth,color};if(taxi_restart_render_call(&s,0x0802b0b0,args)!=1)return 2;printf("%u %u ",s.shade_depth_6854,s.shade_color_6858);for(unsigned i=0;i<sizeof lookup;i++)printf("%02x",lookup[i]);puts("");}return 0;}
