#include "world_background.h"
#include <stdio.h>
#include <inttypes.h>
int main(void){int32_t pitch;while(scanf("%" SCNd32,&pitch)==1)printf("%" PRId32 "\n",taxi_world_background_pitch(pitch));return 0;}
