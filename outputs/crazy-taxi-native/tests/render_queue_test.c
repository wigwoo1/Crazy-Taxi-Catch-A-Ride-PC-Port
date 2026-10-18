#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "render_queue.h"
typedef struct {unsigned count;size_t payload[5];unsigned kind[5];} Log;
static int dispatch(void *ctx,unsigned kind,size_t payload){Log *l=ctx;l->payload[l->count]=payload;l->kind[l->count++]=kind;return 0;}
int main(void){
    int32_t depth=123;
    const int32_t quad[4]={-1,0,0,0},tri[3]={-1,0,0};
    assert(!taxi_world_polygon_depth(quad,4,&depth)&&depth==-1);
    assert(!taxi_world_polygon_depth(tri,3,&depth)&&depth==0);
    const int32_t overflow[4]={INT32_MAX,INT32_MAX,2,0};
    assert(!taxi_world_polygon_depth(overflow,4,&depth)&&depth==0);
    assert(taxi_world_polygon_depth(quad,2,&depth)==-1&&depth==0);
    for(int32_t sum=-100000;sum<=100000;sum++){
        int32_t z[4]={sum,0,0,0};
        assert(!taxi_world_polygon_depth(z,3,&depth)&&depth==sum/3);
        int32_t floor_div=sum/4-(sum<0&&sum%4!=0);
        assert(!taxi_world_polygon_depth(z,4,&depth)&&depth==floor_div);
    }
    TaxiRenderPacket packets[5];TaxiRenderQueue q;Log l={0};taxi_render_queue_init(&q,packets,5);
    /* Mixed world(kind0)/traffic(kind1) fixture demonstrates interleaving. */
    assert(!taxi_render_queue_push(&q,100,0,10));assert(!taxi_render_queue_push(&q,30,0,20));
    assert(!taxi_render_queue_push(&q,80,1,30));assert(!taxi_render_queue_push(&q,80,0,40));
    assert(!taxi_render_queue_push(&q,0,1,50));
    assert(taxi_render_queue_push(&q,10,1,60)==-1);
    assert(!taxi_render_queue_consume(&q,dispatch,&l));
    const size_t expected[5]={10,40,30,20,50};
    for(unsigned i=0;i<5;i++)assert(l.payload[i]==expected[i]);
    assert(l.kind[0]==0&&l.kind[1]==0&&l.kind[2]==1&&l.kind[3]==0&&l.kind[4]==1);
    assert(!q.count);l.count=0;assert(!taxi_render_queue_consume(&q,dispatch,&l)&&!l.count);
    puts("Original polygon depth rounding, mixed world/traffic queue ordering and reset PASS");
}
