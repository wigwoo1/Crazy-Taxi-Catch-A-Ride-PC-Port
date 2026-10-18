#include "../src/startup_intro_fade.h"
#include <stdio.h>
int main(void){TaxiStartupIntro s={0};TaxiStartupIntroFade f={0};unsigned char metrics[602];FILE *file=fopen("outputs/crazy-taxi-native/assets/startup-animation/sprites.crs","rb");if(!file)return 1;if(fread(metrics,1,sizeof metrics,file)!=sizeof metrics)return 2;fclose(file);
 for(unsigned i=0;i<33;i++)for(unsigned j=0;j<9;j++){unsigned n=8+i*18+j*2;s.metrics[i][j]=(uint16_t)(metrics[n]|metrics[n+1]<<8);}
 s.width=240;s.height=160;s.animation.phase_3574=255;s.animation.remaining_359c=18000;
 if(taxi_startup_obj_setup(&s.queue,&s.transport,s.base_template,s.matrix_template,1)||taxi_startup_intro_fade_begin(&s,&f,1))return 3;
 unsigned frame=0,in_end=0,out_start=0;
 for(;frame<700;){if(taxi_startup_intro_fade_frame(&s,&f,1,0,0))return 4;frame++;if(!in_end&&!f.state.fade.active_177b){in_end=frame;if(s.brightness!=0)return 5;}if(s.fades&&!out_start)out_start=frame;if(s.fades&&!f.state.fade.active_177b)break;}
 if(frame>=700||s.fades!=1||s.brightness!=16||s.animation.remaining_359c||s.queue.total)return 6;
 printf("PASS native fade composition: frames%u fadeINend%u fadeOUTstart%u brightness%u accepted%u\n",frame,in_end,out_start,s.brightness,s.accepted);return 0;}
