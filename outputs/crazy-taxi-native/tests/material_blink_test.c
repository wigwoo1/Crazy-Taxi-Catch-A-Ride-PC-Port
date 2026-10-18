#include "../src/material_blink.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int panel(void *context,const uint8_t record[48],const uint8_t cache[512]) {
    TaxiMaterialBlink *state=context;assert(record && state->has_record);
    for(unsigned i=0;i<512;++i)assert(cache[i]==93);
    return 0;
}
int main(int argc, char **argv) {
    uint8_t cache[512];
    const uint16_t indices[4]={0,511,7,65535};
    for (unsigned phase=0;phase<64;++phase) for (int frozen=0;frozen<2;++frozen) {
        memset(cache,93,sizeof cache);uint32_t clock=phase;
        assert(taxi_material_blink_step(cache,&clock,1,frozen,0,indices)==1);
        assert(clock==phase+(unsigned)!frozen);
        for (unsigned i=0;i<512;++i)
            assert(cache[i]==((i==0||i==511||i==7)?((clock&16)?0:1):93));
    }
    uint32_t clock=UINT32_MAX;memset(cache,93,sizeof cache);
    assert(!taxi_material_blink_step(cache,&clock,1,0,-1,indices));assert(clock==0);
    for (unsigned i=0;i<512;++i) assert(cache[i]==93);
    const uint16_t invalid[4]={0,512,65535,65535};
    assert(taxi_material_blink_step(cache,&clock,1,0,0,invalid)==-1);
    assert(clock==0 && cache[0]==93);
    unsigned records=0;
    for (int asset=1;asset<argc;++asset) {
        FILE *file=fopen(argv[asset],"rb");assert(file);
        uint32_t header[4];assert(fread(header,4,4,file)==4 && !memcmp(header,"CTP1",4));
        assert(fseek(file,(long)header[1]*32,SEEK_CUR)==0);
        for (unsigned destination=0;destination<header[3];++destination) {
            uint8_t record[48];assert(fread(record,1,48,file)==48);
            uint16_t ids[4];
            for (unsigned j=0;j<4;++j) {
                ids[j]=(uint16_t)(record[40+2*j]|(record[41+2*j]<<8));
                assert(ids[j]==65535 || ids[j]<512);
            }
            for (unsigned phase=0;phase<32;++phase) {
                memset(cache,93,sizeof cache);clock=phase;
                assert(taxi_material_blink_step(cache,&clock,1,0,(int32_t)destination,ids)==1);
                for (unsigned index=0;index<512;++index) {
                    int selected=0;
                    for (unsigned j=0;j<4;++j) if (ids[j]==index) selected=1;
                    assert(cache[index]==(selected?((clock&16)?0:1):93));
                }
            }
            for(unsigned mode=0;mode<256;++mode) {
                TaxiMaterialBlink state={.clock=37,.destination=-1};
                memset(state.cache,93,sizeof state.cache);
                assert(!taxi_material_blink_select(&state,(int32_t)destination,
                    record,sizeof record,(uint8_t)mode,panel,&state));
                assert(state.clock==37 && state.has_record && state.destination==(int32_t)destination);
                for(unsigned index=0;index<512;++index)assert(!state.cache[index]);
                assert(taxi_material_blink_update(&state,11,0)==1);
                assert(state.clock==48);
                assert(!taxi_material_blink_select(&state,-1,NULL,0,(uint8_t)mode,NULL,NULL));
                assert(state.clock==48 && !state.has_record && state.destination==-1);
                assert(!taxi_material_blink_update(&state,1,1) && state.clock==48);
            }
            ++records;
        }
        fclose(file);
        printf("Original material blink records: %s (%u) PASS\n",argv[asset],header[3]);
    }
    if (argc>1) assert(records==60);
    puts("Original destination material blink: clock/freeze/wrap, all cache bytes and guards PASS");
}
