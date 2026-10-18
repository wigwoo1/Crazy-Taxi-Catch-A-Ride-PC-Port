#include "../src/obj_order.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
int main(void){
 TaxiObjField original={10,20,64,128,1},f;int32_t offsets[]={-1,4095,-4097};
 for(unsigned mode=0;mode<256;mode++){
  f=original;unsigned char rows[2048]={0};assert(!taxi_obj_screen_origin(&f,(uint8_t)mode,offsets,rows,sizeof rows));
  assert(f.left==(mode==1?9:mode==3?8:10)&&f.top==20&&f.width==64&&f.height==128&&f.double_size==1);
 }
 unsigned char rows[2048]={0};
 for(unsigned row=0;row<128;row++){
  int32_t value=(int32_t)row*256-16385;uint32_t v=(uint32_t)value;
  for(unsigned b=0;b<4;b++)rows[row*16+4+b]=(uint8_t)(v>>(b*8));
 }
 for(unsigned x=0;x<256;x++){
  f=original;f.left=(int32_t)x;assert(!taxi_obj_screen_origin(&f,4,NULL,rows,sizeof rows));
  assert(f.top==20+(int32_t)(x/2)-65&&f.left==(int32_t)x);
 }
 f=original;f.left=-1;assert(!taxi_obj_screen_origin(&f,4,NULL,NULL,0)&&f.top==20);
 f.left=256;assert(!taxi_obj_screen_origin(&f,4,NULL,NULL,0)&&f.top==20);
 f=original;assert(taxi_obj_screen_origin(&f,4,NULL,rows,2047)==-1&&!memcmp(&f,&original,sizeof f));
 assert(taxi_obj_screen_origin(&f,1,NULL,NULL,0)==-1&&!memcmp(&f,&original,sizeof f));
 f=original;f.top=INT32_MAX;offsets[1]=4096;
 assert(!taxi_obj_screen_origin(&f,2,offsets,NULL,0)&&f.top==INT32_MIN);
 assert(taxi_obj_screen_origin(NULL,0,NULL,NULL,0)==-1);
 puts("PASS original OBJ origin modes,256 signed/unsigned row boundaries, metadata retention and invalid atomicity; live owner unqualified.");
 return 0;
}
