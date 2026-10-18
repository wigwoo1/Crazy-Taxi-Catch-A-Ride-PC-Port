#include "traffic_init.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void write32(uint8_t *p, uint32_t x) {
    p[0]=(uint8_t)x; p[1]=(uint8_t)(x>>8);
    p[2]=(uint8_t)(x>>16); p[3]=(uint8_t)(x>>24);
}
int main(void) {
    uint8_t actors[255*0x5c], expected[sizeof actors];
    uint8_t sprites[576], reference[576];
    const uint32_t bases[3]={0x08230110,0x08250318,0x08270520};
    for (unsigned density=0;density<256;density++) {
        memset(actors,0xa5,sizeof actors); memcpy(expected,actors,sizeof actors);
        memset(sprites,0x5a,sizeof sprites); memcpy(reference,sprites,sizeof sprites);
        uint32_t parameters[3]={0,123,0};
        unsigned count=density?density:1;
        for(unsigned i=0;i<count;i++) write32(expected+i*92+16,180*(i+1));
        for(unsigned type=0;type<3;type++)for(unsigned frame=0;frame<16;frame++){
            uint8_t *p=reference+type*192+frame*12;
            p[0]=128;p[1]=64;p[2]=7;p[3]=6;
            write32(p+4,bases[type]+frame*8192);
        }
        assert(taxi_traffic_initialize(actors,sizeof actors,density,parameters,sprites)==0);
        assert(memcmp(actors,expected,sizeof actors)==0);
        assert(memcmp(sprites,reference,sizeof sprites)==0);
        assert(parameters[0]==0x8e800&&parameters[1]==123&&parameters[2]==0x6b8cc);
        memcpy(expected,actors,sizeof actors); memcpy(reference,sprites,sizeof sprites);
        uint32_t old[3];memcpy(old,parameters,sizeof old);
        assert(taxi_traffic_initialize(actors,count*92-1,density,parameters,sprites)==-1);
        assert(memcmp(actors,expected,sizeof actors)==0);
        assert(memcmp(sprites,reference,sizeof sprites)==0);
        assert(memcmp(parameters,old,sizeof old)==0);
    }
    puts("Original traffic initialization: all byte-density inputs, retained bytes and bounds PASS");
    return 0;
}
