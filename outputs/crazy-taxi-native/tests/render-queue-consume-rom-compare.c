#include "../src/render_queue.h"
#include <stdio.h>
#include <stdint.h>
typedef struct {unsigned ids[256],n;} Calls;
static int draw(void *ctx,unsigned kind,size_t payload){Calls *c=ctx;if(kind!=payload%3)return -1;c->ids[c->n++]=(unsigned)payload;return 0;}
int main(void){unsigned seed,n;while(scanf("%u %u",&seed,&n)==2){if(n>256)return 2;TaxiRenderPacket packets[256];TaxiRenderQueue q;taxi_render_queue_init(&q,packets,256);for(unsigned i=0;i<n;i++)if(taxi_render_queue_push(&q,(seed+i*17u)%128,(i+1)%3,i+1))return 3;Calls c={0};if(taxi_render_queue_consume(&q,draw,&c))return 4;printf("%u ",c.n);for(unsigned i=0;i<c.n;i++)printf("%u ",c.ids[i]);for(unsigned i=0;i<128;i++)printf("%u ",q.heads[i]==SIZE_MAX?0u:1u);printf("%u\n",(unsigned)q.count);}return 0;}
