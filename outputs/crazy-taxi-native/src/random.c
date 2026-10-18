#include "random.h"
#include <string.h>
static FILE *random_trace;
static const char *random_label="other";
void taxi_random_trace(FILE *sink){random_trace=sink;random_label="other";}
void taxi_random_trace_label(const char *label){random_label=label?label:"other";}
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
int32_t taxi_random_range(uint32_t *state,int32_t minimum,int32_t maximum){
    uint32_t s=*state;
    uint32_t before=s;
    if(!s)s=0x4652414e;
    s<<=1;
    if(s&0x80000000u)s^=0x1d872b41;
    s=((s&0x7fffffffu)^0x06000000u)&0x7fffffffu;
    *state=s;
    int32_t span=bits((uint32_t)maximum-(uint32_t)minimum+1u);
    /* Original signed remainder helper returns zero for a zero divisor. */
    int32_t remainder=span?(int32_t)s%span:0;
    int32_t result=bits((uint32_t)minimum+(uint32_t)remainder);
    if(random_trace)fprintf(random_trace,"%s %u %ld %ld %u %ld\n",random_label,before,
        (long)minimum,(long)maximum,*state,(long)result);
    return result;
}
int32_t taxi_random_range_tagged(uint32_t *state,int32_t minimum,int32_t maximum,const char *label){
    const char *previous=random_label;
    taxi_random_trace_label(label);
    int32_t result=taxi_random_range(state,minimum,maximum);
    random_label=previous;
    return result;
}
void taxi_random_fold(uint32_t *state){
    uint32_t s=*state^0x06000000u;
    if(s&0x80000000u)s^=0x1d872b41;
    *state=s&0x7fffffffu;
}
