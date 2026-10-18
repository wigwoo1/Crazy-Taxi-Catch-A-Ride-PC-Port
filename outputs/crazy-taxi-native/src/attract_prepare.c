#include "attract_prepare.h"
void taxi_attract_prepare(TaxiAttractPrepare *s){
 if(!s)return;
 s->recorder[0]=1;s->recorder[1]=s->recorder[2]=0;
 for(unsigned i=8;i<12;i++)s->recorder[i]=0;
 s->recorder[16]=s->recorder[17]=255;s->flag_1ea8=s->flag_3895=0;
}
