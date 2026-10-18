#include "attract_graphics.h"
#include <stdio.h>
static int service(void *c,uint32_t t,const uint32_t a[6]){(void)c;printf("%u",t);for(unsigned i=0;i<6;i++)printf(" %u",a[i]);putchar('\n');return 0;}
int main(void){return taxi_attract_graphics_prefix(service,NULL)?1:0;}
