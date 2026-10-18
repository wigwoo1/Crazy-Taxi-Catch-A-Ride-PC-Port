#include "../src/pair_column_setup.h"
#include <stdio.h>
int main(void){uint32_t d[8],a,b;while(scanf("%u %u %u %u %u %u %u %u %u %u",d,d+1,d+2,d+3,d+4,d+5,d+6,d+7,&a,&b)==10){TaxiPairColumnSetup o={0};int r=taxi_pair_column_setup(d,a,b,&o);printf("%d %u %u %u %u %u %u\n",r,o.row,o.rows,o.x,o.y,o.dx,o.dy);}return 0;}
