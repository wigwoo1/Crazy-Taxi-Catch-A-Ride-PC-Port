#include "../src/random.h"
#include <stdio.h>
int main(void){unsigned seed;int lo,hi;while(scanf("%u %d %d",&seed,&lo,&hi)==3){uint32_t state=seed;int32_t v=taxi_random_range(&state,lo,hi);printf("%d %u\n",v,state);}return 0;}
