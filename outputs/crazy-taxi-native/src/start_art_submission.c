#include "start_art_submission.h"
#include <string.h>
int taxi_start_art_submission(TaxiStartArtSubmission *p,uint8_t active,int16_t phase,uint16_t width,int16_t offset,int16_t scale){
 if(!p)return -1;memset(p,0,sizeof *p);if(!active)return 0;
 int32_t center=width>>1;p->scale=scale;
 if(phase<=1){p->count=6;p->y=64;for(unsigned j=0;j<6;j++){p->frame[j]=(uint8_t)(j%3);p->x[j]=center+(j<3?-(int32_t)offset:(int32_t)offset)-48+32*(int32_t)(j%3);}}
 else if(phase==2&&scale<=512){p->count=3;p->affine=1;p->y=80;for(unsigned j=0;j<3;j++){p->frame[j]=(uint8_t)j;p->x[j]=center+((int32_t)j-1)*(scale/8);}}
 return 0;
}
