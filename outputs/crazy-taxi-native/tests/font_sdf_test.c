#ifdef NDEBUG
#undef NDEBUG
#endif
#include "font_sdf.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void synthetic(void){
    uint8_t source[7*7*4]={0};
    for(unsigned y=1;y<6;y++)for(unsigned x=1;x<6;x++){
        uint8_t *p=source+(y*7+x)*4;p[0]=231;p[1]=199;p[2]=147;p[3]=255;
    }
    uint8_t output[35*35*4];unsigned w=0,h=0;
    assert(!taxi_font_sdf_expand_rgba(source,7,7,5,64,output,sizeof output,&w,&h));
    assert(w==35&&h==35);
    unsigned partial=0;
    for(size_t i=0;i<(size_t)w*h;i++)if(output[i*4+3]&&output[i*4+3]<255)partial++;
    assert(partial>0);
    assert(output[(17*35+17)*4+3]==255);
    assert(output[(0*35+0)*4+3]==0);
    assert(output[(17*35+17)*4]==231&&output[(17*35+17)*4+1]==199);
    for(unsigned y=0;y<7;y++)for(unsigned x=0;x<7;x++){
        unsigned sx=x*5+2,sy=y*5+2;uint8_t alpha=output[(sy*w+sx)*4+3];
        assert((alpha>=128)==(source[(y*7+x)*4+3]>=128));
    }
    uint8_t empty[7*7*4]={0};assert(!taxi_font_sdf_expand_rgba(empty,7,7,5,64,output,sizeof output,&w,&h));
    for(size_t i=0;i<(size_t)w*h*4;i++)assert(output[i]==0);
    assert(taxi_font_sdf_expand_rgba(source,7,7,1,64,output,sizeof output,&w,&h)<0);
    assert(taxi_font_sdf_expand_rgba(source,7,7,5,64,output,1,&w,&h)<0);
}
static void source_glyph(const char *input,const char *output_path){
    FILE *f=fopen(input,"rb");if(!f){perror(input);exit(1);}
    const size_t bytes=(size_t)2*224*32*32*4;uint8_t *all=malloc(bytes),*out=malloc((size_t)128*128*4),*sampled=malloc((size_t)160*160*4);
    if(!all||!out||!sampled||fread(all,1,bytes,f)!=bytes||fgetc(f)!=EOF){fprintf(stderr,"font source has invalid length\n");exit(1);}fclose(f);
    /* The exact ROM-derived 'A' glyph (ASCII 65, descriptor starts at 32),
       using the white menu-font palette bank. */
    const uint8_t *glyph=all+((size_t)224+65-32)*32*32*4;unsigned w=0,h=0;
    if(taxi_font_sdf_expand_rgba(glyph,32,32,4,64,out,(size_t)128*128*4,&w,&h)||w!=128||h!=128){fprintf(stderr,"font SDF expansion failed\n");exit(1);}
    unsigned preview_width=w,preview_height=h;
    if(taxi_font_sdf_expand_rgba(glyph,32,32,5,64,sampled,(size_t)160*160*4,&w,&h)||w!=160||h!=160){fprintf(stderr,"font SDF source-center check failed\n");exit(1);}
    unsigned ink=0,partial=0;
    for(unsigned y=0;y<32;y++)for(unsigned x=0;x<32;x++){
        const uint8_t *original=glyph+(y*32+x)*4,*expanded=sampled+((y*5+2)*160+x*5+2)*4;
        if((expanded[3]>=128)!=(original[3]>=128)||((original[3]>=128)&&memcmp(original,expanded,3))){fprintf(stderr,"font SDF changed source glyph center at%u,%u\n",x,y);exit(1);}
        if(original[3])ink++;
    }
    for(size_t i=0;i<(size_t)128*128;i++)if(out[i*4+3]&&out[i*4+3]<255)partial++;
    if(!ink||!partial){fprintf(stderr,"font SDF produced no source ink or antialiased edge\n");exit(1);}
    f=fopen(output_path,"wb");if(!f||fwrite(out,1,(size_t)preview_width*preview_height*4,f)!=(size_t)preview_width*preview_height*4||fclose(f)){fprintf(stderr,"font SDF output write failed\n");exit(1);}
    free(all);free(out);free(sampled);
}
static void source_hud_bank(const char *input){
    FILE *f=fopen(input,"rb");if(!f){perror(input);exit(1);}
    uint8_t source[16*16*4],expanded[64*64*4];unsigned tested=0,partial=0;
    for(unsigned glyph=0;glyph<896;glyph++){
        if(fread(source,1,sizeof source,f)!=sizeof source){fprintf(stderr,"HUD font bank truncated at%u\n",glyph);exit(1);}
        unsigned w=0,h=0;
        if(taxi_font_sdf_expand_rgba(source,16,16,4,64,expanded,sizeof expanded,&w,&h)||w!=64||h!=64){fprintf(stderr,"HUD font SDF failed at%u\n",glyph);exit(1);}
        for(unsigned y=0;y<16;y++)for(unsigned x=0;x<16;x++){
            const uint8_t *a=source+(y*16+x)*4,*b=expanded+((y*4+2)*64+x*4+2)*4;
            if(a[3]!=b[3]||(a[3]&&memcmp(a,b,3))){fprintf(stderr,"HUD font visible source center changed at glyph%u pixel%u,%u\n",glyph,x,y);exit(1);}
            tested++;
        }
        for(size_t i=0;i<(size_t)64*64;i++)if(expanded[i*4+3]&&expanded[i*4+3]<255)partial++;
    }
    if(fgetc(f)!=EOF||ferror(f)||fclose(f)){fprintf(stderr,"HUD font bank has invalid length\n");exit(1);}
    if(tested!=896u*256u||!partial){fprintf(stderr,"HUD font SDF coverage incomplete\n");exit(1);}
    printf("PASS HUD font SDF: %u source pixels across all896 glyph/palette cells retain exact RGBA centers; fractional edge pixels=%u.\n",tested,partial);
}
int main(int argc,char **argv){
    synthetic();
    if(argc==3&&!strcmp(argv[1],"--hud-bank"))source_hud_bank(argv[2]);
    else if(argc==3)source_glyph(argv[1],argv[2]);
    else if(argc!=1){fprintf(stderr,"usage: font-sdf-test [font0.rgba preview.rgba] | --hud-bank hud-font.rgba\n");return 2;}
    if(argc==3&&!strcmp(argv[1],"--hud-bank"))return 0;
    puts(argc==3?"PASS source-preserving font SDF: exact synthetic and ROM glyph center masks/RGB, plus antialiased coverage." : "PASS source-preserving font SDF: synthetic bounds/coverage, exact source-center mask, transparent glyph and invalid-input checks.");
    return 0;
}
