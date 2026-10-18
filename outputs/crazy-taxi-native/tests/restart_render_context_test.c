#include "restart_render_context.h"
#include "shade_table.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
 uint8_t frame[38400];uint16_t palette[256];TaxiViewTransform transform;
 memset(frame,165,sizeof frame);memset(&transform,90,sizeof transform);
 for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)((i*131)&32767);
 TaxiViewTransform before=transform;
 TaxiRestartRenderContext s={.frame=frame,.frame_size=sizeof frame,.fill_06900=0x78563412,.palette=palette,.source_transform_address=0x0200344c,.transform_owner=&transform};
 uint32_t args[7]={0,0,240,160,0,0,0};
 s.state_2c4ec=0xffffffff;TaxiRestartRenderContext expected_reset;memcpy(&expected_reset,&s,sizeof s);expected_reset.state_2c4ec=0;
 assert(taxi_restart_render_call(&s,0x0802cae4,args)==1&&!memcmp(&s,&expected_reset,sizeof s));
 assert(taxi_restart_render_call(&s,0x0802c9f0,args)==1);
 for(unsigned i=0;i<sizeof frame;i++)assert(frame[i]==18);
 args[0]=0x0200344c;assert(taxi_restart_render_call(&s,0x0802c834,args)==1);
 assert(s.selected_transform==&transform&&!memcmp(&before,&transform,sizeof transform));
 args[0]=31;args[1]=args[2]=0;uint32_t expected=0;
 assert(taxi_palette_nearest_pair(palette,31,0,0,&expected));
 assert(taxi_restart_render_call(&s,0x0802c688,args)==1&&s.pattern_engine_64==expected);
 assert(s.fill_06900==0x78563412&&s.selected_transform==&transform);
 TaxiRestartRenderContext saved=s;args[0]=32;
 assert(taxi_restart_render_call(&s,0x0802c688,args)==-1&&!memcmp(&s,&saved,sizeof s));
 args[0]=1;assert(taxi_restart_render_call(&s,0x0802c834,args)==-1&&!memcmp(&s,&saved,sizeof s));
 assert(taxi_restart_render_call(NULL,0x08026318,NULL)==0);
 args[0]=0;assert(taxi_restart_render_call(&s,0x0802c834,args)==1&&s.selected_transform==NULL);
 assert(!memcmp(&before,&transform,sizeof transform));
 args[0]=0x0200344c;assert(taxi_restart_render_call(&s,0x0802c834,args)==1&&s.selected_transform==&transform);
 s.frame_size=1;assert(taxi_restart_render_call(&s,0x0802c9f0,args)==-1);
 uint8_t second[38400];memset(frame,165,sizeof frame);memset(second,165,sizeof second);
 s.frame_size=sizeof frame;s.alternate_frame=second;s.alternate_size=sizeof second;
 s.width=7;s.height=5;s.bitmap_mode=3;s.engine_present=1;args[0]=args[1]=args[2]=0;
 assert(taxi_restart_render_call(&s,0x0802c558,args)==1&&frame[0]==165&&second[0]==165);
 s.bitmap_mode=4;s.engine_present=0;assert(taxi_restart_render_call(&s,0x0802c558,args)==1&&frame[0]==165);
 s.engine_present=1;uint8_t chosen;assert(taxi_palette_nearest_index(palette,0,0,0,&chosen));
 assert(taxi_restart_render_call(&s,0x0802c558,args)==1&&s.fill_06900==chosen);
 for(unsigned i=0;i<38400;i++)assert(frame[i]==(i<34?chosen:165)&&second[i]==frame[i]);
 puts("PASS native restart render adapter: actual frame clear, selected pointer retains transform bytes, separate palette/fill owners, invalid ownership preserves state");return 0;
}
