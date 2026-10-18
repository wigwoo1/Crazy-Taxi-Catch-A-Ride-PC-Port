#include "presentation_service.h"
#include <stdio.h>
int main(void){unsigned a,b,current,previous,delta;while(scanf("%u %u %u %u %u",&a,&b,&current,&previous,&delta)==5){
 TaxiPresentationService s={0};s.current_3520=current;s.previous_3524=previous;s.delta_3b00=delta;
 if(taxi_presentation_reset(&s,(uint16_t)a,(uint16_t)b))return 1;
 printf("%u %u %u %u %u\n",s.blend_control,s.blend_alpha,s.current_3520,s.previous_3524,s.delta_3b00);
 }return 0;}
