/* Offline comparison adapter: explicit captured inputs, no game execution. */
#include "../src/trip_start.h"
#include <inttypes.h>
#include <stdio.h>
int main(void) {
    int32_t v[18];
    for (unsigned i=0;i<18;i++) if (scanf("%" SCNd32,&v[i])!=1) return 2;
    if(v[5]<0||v[5]>65535||v[6]<0||v[6]>65535||v[7]<0||v[7]>65535) return 2;
    const unsigned byte_fields[]={8,9,10,15,16,17};
    for(unsigned i=0;i<6;i++) if(v[byte_fields[i]]<0||v[byte_fields[i]]>255) return 2;
    TaxiTripStartState s={v[0],v[1],v[2],v[3],v[4],(uint16_t)v[5],(uint16_t)v[6],(uint16_t)v[7],(uint8_t)v[8],(uint8_t)v[9],(uint8_t)v[10]};
    TaxiTripStartInput in={v[11],v[12],v[13],v[14],(uint8_t)v[15],(uint8_t)v[16],(uint8_t)v[17]};
    uint8_t palette=0;
    if(taxi_trip_start(&s,&in,&palette)) return 3;
    printf("%" PRId32 " %" PRId32 " %" PRId32 " %" PRId32 " %" PRId32 " %u %u %u %u %u %u %u\n",s.active_customer,s.direction_753c,s.time_16ac,s.initial_time_16b0,s.allowance_16e0,(unsigned)s.scale_5310,(unsigned)s.scale_5312,(unsigned)s.phase_5314,(unsigned)s.active_1694,(unsigned)s.active_1695,(unsigned)s.voice_25d6,(unsigned)palette);
    return 0;
}
