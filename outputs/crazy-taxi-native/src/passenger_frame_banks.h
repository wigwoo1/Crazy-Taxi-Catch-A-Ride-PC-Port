#ifndef TAXI_PASSENGER_FRAME_BANKS_H
#define TAXI_PASSENGER_FRAME_BANKS_H
#include <stdint.h>
/* Original ROM087EC51C, sourcequalified frame selector2318E..231AA. */
static const uint32_t taxi_passenger_frame_bases[32]={0x84507c9u,0x84511d1u,0x844e5c1u,0x84533d9u,0x8459fe9u,0x845a9f1u,0x8457de1u,0x845cbf9u,0x8463809u,0x8464211u,0x8461601u,0x8466419u,0x846d029u,0x846da31u,0x846ae21u,0x846fc39u,0x8476849u,0x8477251u,0x8474641u,0x8479459u,0x8480069u,0x8480a71u,0x847de61u,0x8482c79u,0x8489889u,0x848a291u,0x8487681u,0x848c499u,0x84930a9u,0x8493ab1u,0x8490ea1u,0x8495cb9u};
static inline int taxi_passenger_texture_from_pointer(uint32_t address,unsigned *out){
    static const unsigned counts[4]={1,4,4,9},starts[4]={0,1,5,9};
    if(!out)return -1;
    for(unsigned k=0;k<8;k++)for(unsigned s=0;s<4;s++){
        uint32_t offset=address-taxi_passenger_frame_bases[k*4+s];
        if(offset%2048u==0&&offset/2048u<counts[s]){
            *out=k*18+starts[s]+offset/2048u;return 0;
        }
    }
    return -1;
}
#endif
