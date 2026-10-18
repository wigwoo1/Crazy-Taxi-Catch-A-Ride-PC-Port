#include "../src/graphics_storage.h"
#include "../src/graphics_generation.h"
#include "../src/world_polygon_stamp.h"
#include <stdio.h>
#include <stdlib.h>
int main(void){unsigned base,prior;while(scanf("%u %u",&base,&prior)==2){TaxiGraphicsStorage *s=calloc(1,sizeof *s);if(!s)return 2;s->state_6844=base;s->pool_1b6ec[0]=prior;s->pool_1b6ec[1]=0xa5a5a5a5;s->heads_2b6ec[0]=0x12345678;uint32_t saved=s->state_6844;
 if(taxi_graphics_generation_advance(&s->state_6844,1,1)!=1)return 3;
 int preview=taxi_world_polygon_stamp(s->pool_1b6ec,12288,s->state_6844,0);unsigned preview_generation=s->state_6844;
 s->state_6844=saved; /* Only the qualified source counter slice, not full248 restoration. */
 if(taxi_graphics_generation_advance(&s->state_6844,1,1)!=1)return 3;
 int next=taxi_world_polygon_stamp(s->pool_1b6ec,12288,s->state_6844,0);
 if(taxi_graphics_generation_advance(&s->state_6844,1,1)!=1)return 3;
 int following=taxi_world_polygon_stamp(s->pool_1b6ec,12288,s->state_6844,0);
 printf("%u %d %d %u %d %u %u %u\n",preview_generation,preview,next,s->state_6844,following,s->pool_1b6ec[0],s->pool_1b6ec[1],s->heads_2b6ec[0]);free(s);}return 0;}
