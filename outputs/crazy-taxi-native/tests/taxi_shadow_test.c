#ifdef NDEBUG
#undef NDEBUG
#endif
#include "taxi_shadow.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
    assert(argc==2);TaxiShadow *s=malloc(sizeof *s),*saved=malloc(sizeof *saved);assert(s&&saved);
    assert(!taxi_shadow_load(s,argv[1]));*saved=*s;
    assert(taxi_shadow_load(s,"missing-shadow-bank.ctx")==-1&&!memcmp(s,saved,sizeof *s));
    uint16_t palette[256];for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(i*127);
    uint8_t rgba[8192],retained[8192];unsigned transparent=0;
    for(unsigned f=0;f<64;f++){
        assert(!taxi_shadow_rgba(s,f,palette,rgba));
        for(unsigned i=0;i<2048;i++){
            unsigned index=s->pixels[f*2048+i];assert(rgba[4*i+3]==(index?255:0));
            transparent+=index==0;
            for(unsigned k=0;k<3;k++){
                unsigned value=(palette[index]>>(5*k))&31;
                assert(rgba[4*i+k]==((value<<3)|(value>>2)));
            }
        }
    }
    assert(transparent==103777);memcpy(retained,rgba,sizeof rgba);
    assert(taxi_shadow_rgba(s,64,palette,rgba)==-1&&!memcmp(retained,rgba,sizeof rgba));
    free(saved);free(s);puts("Original64 taxi shadow frames: all source indices, caller palette, alpha and failed-load retention PASS");
    return 0;
}
