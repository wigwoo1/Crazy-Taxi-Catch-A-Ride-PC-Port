#include "restart_render_context.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
int main(int argc,char **argv){
 if(argc!=3)return 1;
 uint32_t index=(uint32_t)strtoul(argv[1],NULL,16),seed=(uint32_t)strtoul(argv[2],NULL,16);
 uint8_t table[2560],lookup[2560];
 for(unsigned i=0;i<128;i++)for(unsigned j=0;j<20;j++)table[20*i+j]=(uint8_t)(i*37+j+seed);
 TaxiRestartRenderContext s={.registered_materials=table,.registered_material_count=128,.shade_lookup=lookup,.shade_lookup_size=2560};
 uint32_t args[7]={index};if(taxi_restart_render_call(&s,0x0802ca94,args)!=1)return 2;
 _setmode(_fileno(stdout),_O_BINARY);return fwrite(lookup,1,2560,stdout)==2560?0:3;
}
