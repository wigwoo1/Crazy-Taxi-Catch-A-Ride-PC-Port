#include "../src/graphics_generation.h"
#include <stdio.h>
int main(void){unsigned n,s,t;while(scanf("%u %u %u",&n,&s,&t)==3){int rc=taxi_graphics_generation_advance(&n,s,t);printf("%d %u\n",rc,n);}return 0;}
