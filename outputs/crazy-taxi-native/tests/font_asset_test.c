#include "../src/font_asset.h"
#include "../src/text_layout.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv) {
    assert(argc==7||argc==15||argc==19);
    const unsigned counts[3]={224,224,170},palettes[3]={3,3,1};
    for(unsigned slot=0;slot<3;slot++) {
        TaxiFontAsset f={0};assert(!taxi_font_asset_load(&f,argv[1+slot*2],argv[2+slot*2],palettes[slot],32,slot==1));
        assert(f.count==counts[slot]&&!!f.pairs==(slot==1));
        for(unsigned b=0;b<f.palettes;b++)for(unsigned g=0;g<f.count;g++)assert(taxi_font_asset_glyph(&f,b,g));
        assert(!taxi_font_asset_glyph(&f,f.palettes,0)&&!taxi_font_asset_glyph(&f,0,f.count));
        if(slot==0)assert(f.metrics[160*9+1]==4); /* Source32px tall glyph. */
        if(slot==2)assert(f.metrics[96*9]==4); /* Source32px wide glyph. */
        assert(taxi_font_asset_load(&f,argv[1],argv[2],3,32,0)==-1);
        taxi_font_asset_free(&f);assert(!f.rgba&&!f.metrics&&!f.pairs&&!f.count);
    }
    if(argc>=15) {
        const unsigned extra_counts[4]={224,224,96,96},extra_palettes[4]={3,3,1,1};
        const int spacing[4]={3,1,0,-2};
        for(unsigned i=0;i<4;i++) {
            TaxiFontAsset f={0};assert(!taxi_font_asset_load(&f,argv[7+i*2],argv[8+i*2],extra_palettes[i],32,i<2));
            assert(f.count==extra_counts[i]&&f.spacing==spacing[i]&&!!f.pairs==(i<2));
            for(unsigned bank=0;bank<f.palettes;bank++)for(unsigned g=0;g<f.count;g++)assert(taxi_font_asset_glyph(&f,bank,g));
            taxi_font_asset_free(&f);
        }
        puts("Native name-entry/ranking fonts: all1536 frames loaded; original pair ownership and signed negative ranking spacing preserved.");
    }
    if(argc==19) {
        for(unsigned slot=0;slot<2;slot++) {
            TaxiFontAsset f={0};assert(!taxi_font_asset_load(&f,argv[15+slot*2],argv[16+slot*2],slot?3:4,32,1));
            assert(f.count==224&&f.pairs&&f.spacing==(slot?1:3));
            for(unsigned bank=0;bank<f.palettes;bank++)for(unsigned g=0;g<224;g++)assert(taxi_font_asset_glyph(&f,bank,g));
            taxi_font_asset_free(&f);
        }
        puts("Native profile fonts: all1568 frames, four-palette font0 and original pair tables preserved.");
    }
    puts("Native results fonts: all1514 glyph/palette frames loaded; tall/wide glyphs and null pair ownership preserved.");
    return 0;
}
