#include "presentation_service.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
int main(void){
 if(_setmode(_fileno(stdin),_O_BINARY)==-1)return 6;
 uint8_t h[132];memset(h,0x91,sizeof h);TaxiPresentationBlend b={0xa5a5,0x5a5a},before=b;
 for(size_t n=0;n<132;n++)if(taxi_presentation_blend_load(&b,h,n)!=-1||memcmp(&b,&before,sizeof b))return 2;
 if(taxi_presentation_blend_load(NULL,h,132)!=-1||taxi_presentation_blend_load(&b,NULL,132)!=-1||memcmp(&b,&before,sizeof b))return 3;
 uint8_t record[136];
 while(fread(record,1,sizeof record,stdin)==sizeof record){
  uint32_t current=(uint32_t)record[132]|((uint32_t)record[133]<<8)|((uint32_t)record[134]<<16)|((uint32_t)record[135]<<24);
  b=before;TaxiPresentationService s;memset(&s,0xa5,sizeof s);s.current_3520=current;
  TaxiPresentationService expected;memcpy(&expected,&s,sizeof s);expected.blend_control=0x1f4f;expected.blend_alpha=(uint16_t)(((uint16_t)record[128]<<8)|record[130]|((uint16_t)record[131]<<8));expected.previous_3524=current;expected.delta_3b00=1;
  if(taxi_presentation_blend_load(&b,record,132)||taxi_presentation_reset(&s,b.alpha_00e0,b.alpha_00e2)||memcmp(&s,&expected,sizeof s))return 4;
  printf("%u %u %u %u %u %u %u\n",(unsigned)b.alpha_00e0,(unsigned)b.alpha_00e2,(unsigned)s.blend_control,(unsigned)s.blend_alpha,(unsigned)s.current_3520,(unsigned)s.previous_3524,(unsigned)s.delta_3b00);
 }
 return ferror(stdin)?5:0;
}
