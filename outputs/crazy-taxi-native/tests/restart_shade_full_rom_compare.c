#include "restart_render_context.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned depth,color,value;while(scanf("%u%u",&depth,&color)==2){uint16_t palette[256];for(unsigned i=0;i<256;i++){if(scanf("%u",&value)!=1)return 2;palette[i]=(uint16_t)value;}uint8_t lookup[2560];memset(lookup,165,sizeof lookup);TaxiRestartRenderContext s={.shade_lookup=lookup,.shade_lookup_size=sizeof lookup,.engine_present=1,.palette=palette};uint32_t args[7]={depth,color};if(taxi_restart_render_call(&s,0x0802b0b0,args)!=1)return 3;printf("%u %u ",s.shade_depth_6854,s.shade_color_6858);for(unsigned i=0;i<sizeof lookup;i++)printf("%02x",lookup[i]);puts("");}return 0;}
