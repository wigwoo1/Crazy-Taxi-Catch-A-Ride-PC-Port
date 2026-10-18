#include "normal_restart_graphics.h"
static int request(TaxiRestartGraphicsCall call,void *context,uint32_t target,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g){
 const uint32_t args[7]={a,b,c,d,e,f,g};return call(context,target,args);
}
int taxi_restart_graphics_begin(TaxiRestartGraphics *s,TaxiRestartGraphicsCall call,void *context){
 if(!s||!call||s->phase||s->city>1)return -1;
 if(request(call,context,0x08027994,0x08070e50,1,0,0,0,0,0)||
    request(call,context,0x0800fec8,0,32,0,0x08415bbc,s->retained_sl,0,0)||
    request(call,context,0x0800fec8,1,32,3,0,3,0,0))return -1;
 return taxi_restart_graphics_common_begin(s,call,context);
}
int taxi_restart_graphics_common_begin(TaxiRestartGraphics *s,TaxiRestartGraphicsCall call,void *context){
 if(!s||!call||s->phase||s->city>1)return -1;
 if(request(call,context,0x0800e154,0x084a5a34,32,1,0,0,0,0)||
    request(call,context,0x0800e5a8,1,32,1,1,0,0,0))return -1;
 if(s->city>1)return -1;
 uint32_t descriptor=0x087556cc+104u*s->city;
 if(request(call,context,0x0802ac6c,descriptor,0,0,240,160,256,38400)||
    request(call,context,0x0802c834,s->context_base+0xe18,0,0,0,0,0,0))return -1;
 unsigned byte=(s->viewport_1780-1u)&255u;int index=byte<128?(int)byte:(int)byte-256;
 if(request(call,context,0x0802ca94,(uint32_t)index,0,0,0,0,0,0)||
    request(call,context,0x0802c688,31,0,0,0,0,0,0)||
    request(call,context,0x080203c4,0,0,0,0,0,0,0))return -1;
 s->driver_graphics_187c=0x08290b88;
 for(unsigned i=0;i<3;i++)if(request(call,context,0x08027fb8,0x080fcff8,i,i,7,s->retained_sl,0,0))return -1;
 s->intro_graphics_1804=0x08290bac;
 if(request(call,context,0x08027fb8,0x080fd5fc,0,0,2,s->retained_sl,0,0)||
    request(call,context,0x08026440,0,0,0,0,0,0,0))return -1;
 s->phase=s->fade_177b?1:2;return s->phase==2;
}
int taxi_restart_graphics_step(TaxiRestartGraphics *s,TaxiRestartGraphicsCall call,void *context){
 if(!s||!call||(s->phase!=1&&s->phase!=3))return -1;
 /* A native child may clear the shared fade before the caller resumes. */
 if(s->phase==3||s->fade_177b){int status=request(call,context,0x0800d764,0,0,0,0,0,0,0);if(status==1){s->phase=3;return 0;}if(status)return -1;s->phase=1;}
 if(s->fade_177b)return 0;
 s->phase=2;return 1;
}
