#ifdef NDEBUG
#undef NDEBUG
#endif
#include "music_asset.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
    assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);
    size_t size=181241;uint8_t *bytes=malloc(size);assert(bytes);
    assert(fread(bytes,1,size,f)==size&&fgetc(f)==EOF&&!ferror(f));fclose(f);
    TaxiMusicAsset asset;
    assert(!taxi_music_asset_index(&asset,bytes,size));
    assert(asset.patterns==42&&asset.samples==31&&asset.channels==4);
    const uint8_t orders[]={63,88,115,34,5};
    const size_t offsets[]={0x651a,0x8821,0xb029,0x3811,0x607};
    for(unsigned i=0;i<5;i++){
        TaxiMusicRecord record;assert(!taxi_music_asset_order(&asset,orders[i],&record));
        assert(record.offset==offsets[i]&&record.length==bytes[record.offset]);
    }
    TaxiMusicAsset before=asset;
    for(unsigned domain=0;domain<2;domain++){
        unsigned count=domain?asset.samples:asset.patterns;
        const TaxiMusicRecord *records=domain?asset.sample:asset.pattern;
        for(unsigned i=0;i<count;i++){
            assert(taxi_music_asset_index(&asset,bytes,records[i].offset+records[i].size-1)==-1);
            assert(!memcmp(&asset,&before,sizeof asset));
        }
    }
    for(unsigned order=0;order<256;order++){
        TaxiMusicRecord out={1,2,3};unsigned index=bytes[6+order];
        int status=taxi_music_asset_order(&asset,(uint8_t)order,&out);
        if(index<asset.patterns){assert(!status);assert(!memcmp(&out,asset.pattern+index,sizeof out));}
        else assert(status==-1&&out.offset==1&&out.size==2&&out.length==3);
    }
    size_t bad=asset.sample[0].offset;uint8_t saved[4];memcpy(saved,bytes+bad,4);memset(bytes+bad,255,4);
    assert(taxi_music_asset_index(&asset,bytes,size)==-1&&!memcmp(&asset,&before,sizeof asset));memcpy(bytes+bad,saved,4);
    free(bytes);
    puts("PASS native original music loader: 42 patterns/31 samples, five source start orders, every record truncation rollback and malformed lengths.");
    return 0;
}
