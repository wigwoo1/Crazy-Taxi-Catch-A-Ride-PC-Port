#include "../src/frame_event.h"
#include <stdio.h>
static unsigned targets[3],args[3],n;
static int call(void *opaque,TaxiFrameEvent *s,uint32_t target,uint32_t arg){(void)opaque;(void)s;targets[n]=target;args[n++]=arg;return 0;}
int main(void){unsigned count,line,keep,settings,callback;
 while(scanf("%u %u %u %u %u",&count,&line,&keep,&settings,&callback)==5){n=0;TaxiFrameEvent s={.counter_3520=count,.scanline_4308=0xa5a5,.callback_352c=callback,.preserve_1ea7=(uint8_t)keep,.settings_1eb0=(uint8_t)settings,.acknowledge=0xa5a5};if(taxi_frame_event(&s,(uint16_t)line,call,0))return 1;
 printf("%u %u %u %u %u",s.counter_3520,s.scanline_4308,s.settings_1eb0,s.acknowledge,n);for(unsigned j=0;j<n;j++)printf(" %u %u",targets[j],args[j]);putchar('\n');}return 0;}
