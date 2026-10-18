#include "../src/passenger_render.h"
#include <stdio.h>
int main(void){unsigned s;while(scanf("%u",&s)==1){int32_t w,h;taxi_passenger_marker_dimensions(s,&w,&h);printf("%ld %ld\n",(long)w,(long)h);}return 0;}
