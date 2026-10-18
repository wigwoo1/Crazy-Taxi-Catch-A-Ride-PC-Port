#include "material_blink.h"
#include <string.h>
static FILE *selection_trace;
void taxi_material_selection_trace(FILE *sink){selection_trace=sink;}
static void trace_selection(FILE *sink,const TaxiMaterialBlink *p){
    fprintf(sink," %ld %u %u",(long)p->destination,(unsigned)p->has_record,p->clock);
    for(unsigned i=0;i<4;i++)fprintf(sink," %u",(unsigned)p->indices[i]);
    for(unsigned i=0;i<512;i++)fprintf(sink," %u",(unsigned)p->cache[i]);
}
int taxi_material_blink_select(TaxiMaterialBlink *state,int32_t destination,
    const uint8_t *record,size_t size,uint8_t runtime_mode,
    TaxiMaterialPanel panel,void *context) {
    if(!state || destination< -1)return -1;
    uint16_t ids[4]={65535,65535,65535,65535};
    if(destination!=-1) {
        if(!record || size<48)return -1;
        for(unsigned i=0;i<4;++i) {
            ids[i]=(uint16_t)(record[40+i*2]|record[41+i*2]<<8);
            if(ids[i]!=65535 && ids[i]>=512)return -1;
        }
        if((runtime_mode&224)!=128 && !panel)return -1;
    }
    TaxiMaterialBlink prior;
    if(selection_trace)prior=*state;
    state->destination=destination;state->has_record=destination!=-1;
    memcpy(state->indices,ids,sizeof ids);
    if(destination!=-1 && (runtime_mode&224)!=128 &&
       panel(context,record,state->cache))return -1;
    memset(state->cache,0,sizeof state->cache);
    if(selection_trace){
        fprintf(selection_trace,"%ld %u",(long)destination,(unsigned)runtime_mode);
        trace_selection(selection_trace,&prior);trace_selection(selection_trace,state);
        for(unsigned i=0;i<48;i++)fprintf(selection_trace," %u",destination==-1?0u:(unsigned)record[i]);
        fputc('\n',selection_trace);
    }
    return 0;
}
int taxi_material_blink_update(TaxiMaterialBlink *state,uint32_t delta,int frozen) {
    if(!state)return -1;
    return taxi_material_blink_step(state->cache,&state->clock,delta,frozen,
        state->destination,state->has_record?state->indices:NULL);
}
int taxi_material_blink_step(uint8_t cache[512], uint32_t *clock,
    uint32_t delta, int frozen, int32_t destination, const uint16_t indices[4]) {
    if (!cache || !clock) return -1;
    int selected=destination!=-1 && indices;
    if (selected) for (unsigned i=0;i<4;++i)
        if (indices[i]!=65535 && indices[i]>=512) return -1;
    if (!frozen) *clock+=delta;
    if (!selected) return 0;
    uint8_t value=(*clock&16)?0:1;
    for (unsigned i=0;i<4;++i) if (indices[i]!=65535) cache[indices[i]]=value;
    return 1;
}
