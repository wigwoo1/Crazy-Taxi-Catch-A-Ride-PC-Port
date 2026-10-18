#include "../src/startup_obj.h"
#include <stdio.h>
int main(void){
 unsigned char bytes[8+33*18];FILE *f=fopen("outputs/crazy-taxi-native/assets/startup-animation/sprites.crs","rb");if(!f)return 1;
 size_t n=fread(bytes,1,sizeof bytes,f);fclose(f);if(n!=sizeof bytes||bytes[4]!=33)return 1;
 unsigned frame,scale,x,y;
 while(scanf("%u %u %u %u",&frame,&scale,&x,&y)==4){
  if(frame>=33)return 1;uint16_t m[9];for(unsigned i=0;i<9;i++){unsigned a=8+frame*18+i*2;m[i]=(uint16_t)(bytes[a]|bytes[a+1]<<8);}
  TaxiStartupObjPlacement p;if(taxi_startup_obj_placement(m,(int32_t)x,(int32_t)y,(int16_t)scale,&p))return 1;
  printf("%u %u %u %u %u %u\n",(uint32_t)p.field.left,(uint32_t)p.field.top,p.field.width,p.field.height,p.field.double_size,p.affine);
 }return 0;
}
