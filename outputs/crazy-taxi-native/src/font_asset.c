#include "font_asset.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static uint16_t half(const uint8_t *p) {return (uint16_t)(p[0]|((unsigned)p[1]<<8));}
void taxi_font_asset_free(TaxiFontAsset *a) {
    if(!a)return;
    free(a->metrics);free(a->pairs);free(a->rgba);memset(a,0,sizeof *a);
}
const uint8_t *taxi_font_asset_glyph(const TaxiFontAsset *a,unsigned bank,unsigned glyph) {
    if(!a||!a->rgba||bank>=a->palettes||glyph>=a->count)return NULL;
    return a->rgba+((size_t)bank*a->count+glyph)*a->cell*a->cell*4;
}
int taxi_font_asset_load(TaxiFontAsset *out,const char *mp,const char *rp,
    unsigned palettes,unsigned cell,int has_pairs) {
    if(!out||!mp||!rp||out->metrics||out->pairs||out->rgba||out->count||out->palettes||
       !palettes||palettes>16||(cell!=16&&cell!=32))return -1;
    TaxiFontAsset a={0};FILE *f=fopen(mp,"rb");if(!f)return -1;
    uint8_t header[8],row[18];
    if(fread(header,1,8,f)!=8||memcmp(header,"CTF1",4))goto bad;
    a.count=half(header+4);a.spacing=(int16_t)half(header+6);a.palettes=palettes;a.cell=cell;
    if(!a.count||a.count>224)goto bad;
    a.metrics=malloc((size_t)a.count*18);if(!a.metrics)goto bad;
    for(unsigned i=0;i<a.count;i++) {
        if(fread(row,1,18,f)!=18)goto bad;
        uint16_t *m=a.metrics+i*9;
        for(unsigned j=0;j<9;j++)m[j]=half(row+j*2);
        if(m[0]>cell/8||m[1]>cell/8)goto bad;
    }
    size_t pairs=(size_t)a.count*a.count;
    if(has_pairs) {
        a.pairs=malloc(pairs);if(!a.pairs||fread(a.pairs,1,pairs,f)!=pairs)goto bad;
    } else {
        /* Exported absent table must be entirely zero; do not retain it. */
        for(size_t i=0;i<pairs;i++)if(fgetc(f)!=0)goto bad;
    }
    if(fgetc(f)!=EOF||ferror(f))goto bad;
    fclose(f);f=fopen(rp,"rb");if(!f)goto bad;
    size_t bytes=(size_t)a.count*palettes*cell*cell*4;
    a.rgba=malloc(bytes);if(!a.rgba||fread(a.rgba,1,bytes,f)!=bytes||fgetc(f)!=EOF||ferror(f))goto bad;
    fclose(f);*out=a;return 0;
bad:
    if(f)fclose(f);
    taxi_font_asset_free(&a);return -1;
}
