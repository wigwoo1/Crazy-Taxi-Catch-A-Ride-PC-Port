#include "indexed_asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t word(const uint8_t *p) { return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24); }
void taxi_indexed_asset_free(TaxiIndexedAsset *a) { if(a){free(a->storage);memset(a,0,sizeof *a);} }
int taxi_indexed_asset_texture(const TaxiIndexedAsset *a,unsigned index,TaxiIndexedTexture *out) {
    if(!a || !out || !a->storage || index>=a->count) return -1;
    const uint8_t *d=a->storage+12+index*12;
    *out=(TaxiIndexedTexture){d[0],d[1],d[2],d[3],a->storage+12+a->count*12+word(d+4),word(d+8)};
    return 0;
}
int taxi_indexed_asset_load(TaxiIndexedAsset *out,const char *path) {
    if(!out || !path) return -1;
    FILE *f=fopen(path,"rb");if(!f)return -1;
    if(fseek(f,0,SEEK_END)){fclose(f);return -1;}
    long length=ftell(f);
    if(length<12 || length>16777216 || fseek(f,0,SEEK_SET)){fclose(f);return -1;}
    uint8_t *p=malloc((size_t)length);if(!p){fclose(f);return -1;}
    int valid=fread(p,1,(size_t)length,f)==(size_t)length;fclose(f);
    if(!valid){free(p);return -1;}
    unsigned count=word(p+4);uint32_t bytes=word(p+8);
    if(memcmp(p,"CTI1",4) || !count || count>512 ||
        12u+(size_t)count*12+bytes!=(size_t)length){free(p);return -1;}
    size_t end=0;
    for(unsigned i=0;i<count;++i) {
        const uint8_t *d=p+12+i*12;
        uint32_t offset=word(d+4),size=word(d+8);
        if(!d[0] || !d[1] || d[2]>8 || d[3]>8 || offset!=end ||
            size!=(unsigned)d[0]*d[1] || size>bytes-end){free(p);return -1;}
        end+=size;
    }
    if(end!=bytes){free(p);return -1;}
    taxi_indexed_asset_free(out);*out=(TaxiIndexedAsset){p,(size_t)length,count};return 0;
}
