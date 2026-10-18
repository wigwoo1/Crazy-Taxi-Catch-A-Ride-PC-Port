#include "ai_path.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
    assert(argc==3);FILE *asset=fopen(argv[1],"rb"),*reference=fopen(argv[2],"rb");
    TaxiAiPathAsset loaded={0};assert(!taxi_ai_path_load(&loaded,argv[1]));
    assert(asset&&reference);char magic[4];uint32_t count;
    assert(fread(magic,1,4,asset)==4&&!memcmp(magic,"CTA1",4));
    assert(fread(&count,4,1,asset)==1&&count>=2&&count<65536);
    TaxiAiPathPoint *points=malloc(count*sizeof *points);assert(points);
    assert(fread(points,sizeof *points,count,asset)==count&&fgetc(asset)==EOF);
    assert(loaded.count==count&&!memcmp(loaded.points,points,count*sizeof *points));
    assert(taxi_ai_path_load(&loaded,argv[2])==-1&&loaded.count==count&&!memcmp(loaded.points,points,count*sizeof *points));
    for(uint32_t query=0;query<65536;query++){
        int32_t expected[2],out[]={0,771,0};
        assert(fread(expected,4,2,reference)==2);
        assert(!taxi_ai_path_target(points,count,query,out));
        assert(out[0]==expected[0]&&out[2]==expected[1]&&out[1]==771);
    }
    assert(fgetc(reference)==EOF);fclose(asset);fclose(reference);free(points);taxi_ai_path_free(&loaded);assert(!loaded.points&&!loaded.count);
    puts("PASS original AI path asset: all65536 queries match independent reference; Y retained.");
}
