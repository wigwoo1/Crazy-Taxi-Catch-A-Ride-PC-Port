#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/city_background.h"
static void read(const char *root,const char *name,void *out,size_t bytes) {
    char path[1024];snprintf(path,sizeof path,"%s/%s",root,name);FILE *f=fopen(path,"rb");assert(f);
    assert(fread(out,1,bytes,f)==bytes&&fgetc(f)==EOF&&!ferror(f));fclose(f);
}
int main(int argc,char **argv) {
    assert(argc==2);uint8_t *bg=malloc(262144),*phases=malloc(4915200),*out=malloc(153600),strip[640];uint16_t bp[256],op[16];assert(bg&&phases&&out);
    read(argv[1],"background.indices",bg,262144);read(argv[1],"background.rgb555",bp,512);read(argv[1],"footer.rgb555",op,32);read(argv[1],"footer.indices",strip,640);
    for(unsigned driver=0;driver<4;driver++) {
        char name[64];snprintf(name,sizeof name,"panel-fade-%u.rgba",driver);read(argv[1],name,phases,4915200);
        for(unsigned cap=0;cap<32;cap++) {
            assert(!taxi_city_background(out,bg,bp,strip,op,(driver&1)*256,(driver/2)*256,cap));
            assert(!memcmp(out,phases+cap*153600,153600));
            assert(!taxi_city_background(out,bg,bp,strip,op,(driver&1)*256+512,(driver/2)*256+512,cap));
            assert(!memcmp(out,phases+cap*153600,153600));
        }
    }
    memset(out,173,153600);strip[0]=16;assert(taxi_city_background(out,bg,bp,strip,op,0,0,31)==-1);
    for(unsigned i=0;i<153600;i++)assert(out[i]==173);
    puts("Native city composition:128 source phase comparisons,512 scroll wrap, palette-before-blend and invalid index rollback passed; viewer integration pending.");
    free(bg);free(phases);free(out);
}
