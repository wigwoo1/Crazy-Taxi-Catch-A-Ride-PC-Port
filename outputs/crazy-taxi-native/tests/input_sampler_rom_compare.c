#include "../src/input.h"
#include <stdio.h>
int main(void){TaxiInput s={0};unsigned raw,suppress,over,mask,delta;while(scanf("%u%u%u%u%u",&raw,&suppress,&over,&mask,&delta)==5){taxi_input_update(&s,(TaxiInputSample){(uint16_t)raw,(uint8_t)suppress,(uint8_t)over,(uint16_t)mask,delta});printf("%u %u %u",s.raw_keys,s.auxiliary_mask,s.pressed_mask);for(unsigned i=0;i<10;i++)printf(" %u %u %u %u %u",s.held[i],s.previous[i],s.repeat[i],s.pressed[i],s.countdown[i]);puts("");}return 0;}
