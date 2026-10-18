#include "render_queue.h"
#include <string.h>
int taxi_world_polygon_depth(const int32_t *z,unsigned count,int32_t *depth){
    if(!z||!depth||(count!=3&&count!=4))return -1;
    uint32_t sum=0;
    for(unsigned i=0;i<count;i++)sum+=(uint32_t)z[i];
    int32_t signed_sum;memcpy(&signed_sum,&sum,sizeof signed_sum);
    /* ARM ASR rounds negative values down; source2ce8 truncates toward0. */
    *depth=count==3?signed_sum/3:(int32_t)(signed_sum>=0?
        (int64_t)signed_sum/4:-((-(int64_t)signed_sum+3)/4));
    return 0;
}
#include <stdint.h>
void taxi_render_queue_init(TaxiRenderQueue *q,TaxiRenderPacket *storage,size_t capacity){
    if(!q)return;
    for(unsigned i=0;i<128;i++)q->heads[i]=SIZE_MAX;
    q->count=0;q->capacity=capacity;q->packets=storage;
}
int taxi_render_queue_push(TaxiRenderQueue *q,unsigned bucket,unsigned kind,size_t payload){
    if(!q||!q->packets||bucket>=128||q->count>=q->capacity)return -1;
    size_t index=q->count++;
    q->packets[index]=(TaxiRenderPacket){q->heads[bucket],payload,kind};q->heads[bucket]=index;
    return 0;
}
int taxi_render_queue_consume(TaxiRenderQueue *q,TaxiRenderDispatch dispatch,void *context){
    if(!q||!q->packets||!dispatch)return -1;
    for(unsigned reverse=128;reverse;reverse--){
        unsigned bucket=reverse-1;size_t index=q->heads[bucket];q->heads[bucket]=SIZE_MAX;
        while(index!=SIZE_MAX){
            if(index>=q->count)return -1;
            TaxiRenderPacket packet=q->packets[index];
            if(dispatch(context,packet.kind,packet.payload))return -1;
            index=packet.next;
        }
    }
    q->count=0;return 0;
}
