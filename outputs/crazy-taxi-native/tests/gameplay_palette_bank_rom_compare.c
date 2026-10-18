#include "palette_fade.h"
#include <stdio.h>
int main(void){unsigned mode,index,prior,source;
 while(scanf("%u%u%u%u",&mode,&index,&prior,&source)==4){
  if(prior>65535||source>65535)return 2;
  uint16_t out=(uint16_t)prior,color=(uint16_t)source;
  int rc=taxi_palette_gameplay_bank_transfer(&out,&color,1,index,mode);if(rc<0)return 3;
  printf("%d %u\n",rc,out);
 }return 0;}
