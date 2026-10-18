#include "../src/traffic_init.h"
#include <stdio.h>
int main(void){for(unsigned i=0;i<256;i++)printf("%u %u %u\n",i,taxi_traffic_density((uint8_t)i),taxi_traffic_spacing((uint8_t)i));return 0;}
