#include "palette_fade_setup.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
int main(void){uint32_t mode,cap,bg,obj,w,h;
 while(scanf("%"SCNu32" %"SCNu32" %"SCNu32" %"SCNu32" %"SCNu32" %"SCNu32,&mode,&cap,&bg,&obj,&w,&h)==6){
 TaxiPaletteFadeStart s;memset(&s,0xa5,sizeof s);s.bldcnt=0x1234;s.bldy=0x2345;
 if(taxi_palette_fade_start_normal(&s,mode,cap,bg,obj,(uint16_t)w,(uint16_t)h))return 1;
 printf("%u %u %u %u %u",s.masks.mode,s.masks.cap,s.masks.endpoint_count,s.masks.background_mask,s.masks.object_mask);
 for(unsigned i=0;i<3;i++)printf(" %u",s.masks.background_channels[i]);
 for(unsigned i=0;i<3;i++)printf(" %u",s.masks.object_channels[i]);
 printf(" %d %u %u %u %u %u %u %u %u %u",s.fade.progress_178a,s.fade.mode_1773,s.fade.active_177b,s.fade.endpoint_count_1774,s.fade.interval_1771,s.fade.reload_1772,s.scale_x_177c,s.scale_y_177e,s.bldcnt,s.bldy);
 for(unsigned i=0;i<3;i++)printf(" %"PRId32,s.motion.position[i]);
 for(unsigned i=0;i<3;i++)printf(" %"PRId32,s.motion.velocity[i]);puts("");
 }return 0;}
