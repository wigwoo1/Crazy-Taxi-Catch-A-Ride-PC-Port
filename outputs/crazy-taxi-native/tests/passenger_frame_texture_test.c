#include "../src/passenger_frame_banks.h"
#include <stdio.h>
int main(void){unsigned a,t;while(scanf("%u",&a)==1){if(taxi_passenger_texture_from_pointer(a,&t))return 1;printf("%u\n",t);}return 0;}
