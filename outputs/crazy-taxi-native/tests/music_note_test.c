#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "music_note.h"
static unsigned read16(const unsigned char *p){return p[0]|(unsigned)p[1]<<8;}
static uint32_t read32(const unsigned char *p){return read16(p)|(uint32_t)read16(p+2)<<16;}
int main(int argc,char **argv){
    assert(argc==3);unsigned char table[1536];FILE *f=fopen(argv[1],"rb");assert(f);
    assert(fread(table,1,sizeof table,f)==sizeof table&&fgetc(f)==EOF);fclose(f);
    f=fopen(argv[2],"rb");assert(f);unsigned count=0;unsigned char r[14];
    while(fread(r,1,sizeof r,f)==sizeof r){
        TaxiMusicNote n;assert(taxi_music_note(&n,r[0],r[1],r[2],table,21024));
        assert(n.note==r[3]&&n.period==read16(r+4)&&n.frequency==read32(r+6)&&n.step==read32(r+10));count++;
    }
    assert(feof(f)&&!ferror(f)&&count==31*254);fclose(f);
    TaxiMusicNote n;memset(&n,0xa5,sizeof n);TaxiMusicNote old=n;
    assert(!taxi_music_note(&n,0,0,0,table,21024));assert(!memcmp(&n,&old,sizeof n));
    assert(!taxi_music_note(&n,128,0,0,table,21024));assert(!memcmp(&n,&old,sizeof n));
    assert(!taxi_music_note(&n,1,0,0,table,0));assert(!memcmp(&n,&old,sizeof n));
    puts("PASS original music pitch: 7874 source sample/note cases and atomic guards");return 0;
}
