#include "../src/shade_table.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
    uint16_t palette[256];
    struct { uint8_t before, table[2560], after; } out;
    memset(&out, 0xa7, sizeof out);
    assert(!taxi_shade_table_build(out.table, palette, 0, 0));
    assert(!taxi_shade_table_build(out.table, NULL, 1, 0));
    for (int i=0;i<2560;++i) assert(out.table[i]==0xa7);
    /* A complete red ramp repeated eight times makes nearest colors exact;
       duplicate entries check the original first-index tie rule. */
    for (int i=0;i<256;++i) palette[i]=(uint16_t)(i&31);
    for (unsigned target=0;target<32;++target) {
        assert(taxi_shade_table_build(out.table,palette,1,target<<19));
        for (int i=0;i<256;++i) {
            assert(out.table[i]==i && out.table[256+i]==i);
            for (int step=2;step<=8;++step) {
                /* Positive weighted sum gives arithmetic-floor interpolation
                   without duplicating the implementation's negative division. */
                unsigned expected=((unsigned)(i&31)*(8-step)+target*step)/8;
                assert(out.table[step*256+i]==expected);
            }
            assert(out.table[2304+i]==target);
        }
        assert(out.before==0xa7 && out.after==0xa7);
    }
    for (int file=1;file<argc;++file) {
        FILE *f=fopen(argv[file],"rb"); assert(f);
        for (int i=0;i<256;++i) {
            int lo=fgetc(f),hi=fgetc(f); assert(lo>=0 && hi>=0);
            palette[i]=(uint16_t)(lo|(hi<<8));
        }
        assert(fgetc(f)==EOF); fclose(f);
        uint8_t darkest,brightest;
        assert(taxi_palette_extrema(palette,&darkest,&brightest));
        unsigned min_sum=94,max_sum=0,min_index=1,max_index=1;
        for(int candidate=255;candidate>=1;--candidate) {
            unsigned color=palette[candidate];
            unsigned sum=(color&31)+((color>>5)&31)+((color>>10)&31);
            if(sum<=min_sum) {min_sum=sum;min_index=(unsigned)candidate;}
            if(sum>=max_sum) {max_sum=sum;max_index=(unsigned)candidate;}
        }
        assert(darkest==min_index && brightest==max_index);
        uint8_t registered_palette[20]={0};
        registered_palette[14]=darkest;registered_palette[15]=brightest;
        assert(taxi_shade_table_material(out.table,registered_palette,1,0));
        for(unsigned i=0;i<2560;++i)assert(out.table[i]==brightest);
        /* Zero-depth scene binding preserves the preceding palette override. */
        assert(!taxi_shade_table_build(out.table,palette,0,0));
        for(unsigned i=0;i<2560;++i)assert(out.table[i]==brightest);
        printf("Original registered palette override: darkest%u brightest%u, retained across zero-depth binding PASS.\n",darkest,brightest);
        const unsigned targets[][3]={{0,0,0},{31,31,31},{3,17,29},{31,0,15},{1,30,7}};
        for (unsigned t=0;t<sizeof targets/sizeof targets[0];++t) {
            const unsigned *target=targets[t];
            assert(taxi_shade_table_build(out.table,palette,123,
                (target[0]<<19)|(target[1]<<11)|(target[2]<<3)));
            for (int bank=0;bank<10;++bank) for (int i=0;i<256;++i) {
                if (bank<2) { assert(out.table[bank*256+i]==i); continue; }
                int wanted[3];
                for (int c=0;c<3;++c) wanted[c]=bank==9?(int)target[c]:
                    (((palette[i]>>(c*5))&31)*(8-bank)+(int)target[c]*bank)/8;
                int best=100,expected=-1;
                for (int candidate=255;candidate>=0;--candidate) {
                    int d=0;
                    for (int c=0;c<3;++c) {
                        int diff=((palette[candidate]>>(c*5))&31)-wanted[c];
                        d+=diff<0?-diff:diff;
                    }
                    /* Reverse search with <= independently yields first ties. */
                    if (d<=best) { best=d; expected=candidate; }
                }
                assert(out.table[bank*256+i]==expected);
            }
        }
        printf("Original scene palette checked: %s\n",argv[file]);
    }
    puts("Shade lookup: rounding, first ties, original palette references and guards PASS");
    uint8_t materials[128*20];
    memset(materials,0x55,sizeof materials);
    for (unsigned i=0;i<128;++i) materials[i*20+15]=(uint8_t)(255-i);
    for (unsigned i=0;i<256;++i) {
        memset(out.table,0xa7,sizeof out.table);
        int valid=taxi_shade_table_material(out.table,materials,128,0x12340000|i);
        assert(valid==(i<128));
        for (int j=0;j<2560;++j) assert(out.table[j]==(i<128?255-i:0xa7));
        assert(out.before==0xa7 && out.after==0xa7);
    }
    assert(!taxi_shade_table_material(out.table,materials,1,1));
    assert(!taxi_shade_table_material(out.table,NULL,128,0));
    puts("Original whole-table material override and signed-index guards PASS");
    uint16_t extrema_palette[256];uint8_t darkest,brightest;
    for(unsigned position=1;position<256;++position) {
        for(unsigned i=0;i<256;++i)extrema_palette[i]=0x4210;
        extrema_palette[0]=0x7fff;extrema_palette[position]=0;
        assert(taxi_palette_extrema(extrema_palette,&darkest,&brightest));
        assert(darkest==position && brightest==(position==1?2:1));
        extrema_palette[0]=0;extrema_palette[position]=0x7fff;
        assert(taxi_palette_extrema(extrema_palette,&darkest,&brightest));
        assert(brightest==position && darkest==(position==1?2:1));
    }
    for(unsigned i=0;i<256;++i)extrema_palette[i]=0;
    assert(taxi_palette_extrema(extrema_palette,&darkest,&brightest));
    assert(darkest==1 && brightest==1);
    extrema_palette[1]=31;extrema_palette[2]=31u<<5;
    extrema_palette[3]=31u<<10;
    assert(taxi_palette_extrema(extrema_palette,&darkest,&brightest));
    assert(darkest==4 && brightest==1);
    puts("Original palette registration extrema: all positions, excluded zero and first ties PASS");
    return 0;
}
