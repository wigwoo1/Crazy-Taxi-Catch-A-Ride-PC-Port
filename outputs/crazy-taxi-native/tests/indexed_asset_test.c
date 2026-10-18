#include "../src/indexed_asset.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv) {
    assert(argc==4);
    for(int file=1;file<argc;++file) {
        TaxiIndexedAsset a={0};assert(!taxi_indexed_asset_load(&a,argv[file]));
        assert(a.count==(file==3?45:149));
        size_t total=0;
        for(unsigned i=0;i<a.count;++i) {
            TaxiIndexedTexture t;assert(!taxi_indexed_asset_texture(&a,i,&t));
            assert(t.size==t.width*t.height);total+=t.size;
            if(!i) assert(t.width==240 && t.height==160);
            else assert(t.width==(1u<<t.log_width) && t.height==(1u<<t.log_height));
        }
        assert(total==(file==3?177216:597952));
        uint8_t *before=a.storage;
        assert(taxi_indexed_asset_load(&a,"missing-indexed-fixture.cti")==-1 && a.storage==before);
        TaxiIndexedTexture t;assert(taxi_indexed_asset_texture(&a,a.count,&t)==-1);
        printf("Original native indexed bundle: %s (%u textures) PASS\n",argv[file],a.count);
        taxi_indexed_asset_free(&a);assert(!a.storage && !a.count);
    }
}
