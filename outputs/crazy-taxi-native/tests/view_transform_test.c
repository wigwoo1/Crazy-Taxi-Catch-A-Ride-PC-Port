#include "../src/view_transform.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv) {
    int32_t sine[4096],cosine[4096];
    for(int i=0;i<4096;i++){sine[i]=i*17-35000;cosine[i]=i*31-60000;}
    TaxiViewTransform view={0};
    taxi_view_transform_position(&view,INT32_MIN,17,INT32_MAX);
    for(int i=-32768;i<=32767;i++) {
        taxi_view_transform_angles(&view,i,i+123,INT32_MIN+(i+32768),sine,cosine);
        const int32_t inputs[3]={i,i+123,INT32_MIN+(i+32768)};
        for(unsigned axis=0;axis<3;axis++) {
            unsigned a=(uint32_t)inputs[axis]%4096;
            unsigned b=a?4096-a:0;
            assert(view.words[3+axis]==(int32_t)a);
            assert(view.words[6+axis]==sine[a] && view.words[9+axis]==cosine[a]);
            assert(view.words[12+axis]==sine[b] && view.words[15+axis]==cosine[b]);
        }
        assert(view.words[0]==INT32_MIN && view.words[1]==17 && view.words[2]==INT32_MAX);
    }
    if(argc>=2) {
        uint8_t bytes[16388];FILE *f=fopen(argv[1],"rb");assert(f);
        assert(fread(bytes,1,sizeof bytes,f)==sizeof bytes);fclose(f);
        assert(!memcmp(bytes,"CTR1",4));
        for(unsigned i=0;i<4096;i++) {
            unsigned a=bytes[4+2*i]|((unsigned)bytes[5+2*i]<<8);
            unsigned b=bytes[8196+2*i]|((unsigned)bytes[8197+2*i]<<8);
            int32_t sa=a<32768?(int32_t)a:(int32_t)a-65536;
            int32_t sb=b<32768?(int32_t)b:(int32_t)b-65536;
            sine[i]=sa>=0?sa/16:-((-sa+15)/16);
            cosine[i]=sb>=0?sb/16:-((-sb+15)/16);
        }
        assert(sine[0]==0 && cosine[0]==256 && sine[1024]==255);
        for(unsigned i=0;i<4096;i++) {
            taxi_view_transform_angles(&view,(int32_t)i,-(int32_t)i,(int32_t)i+4096,sine,cosine);
            assert(view.words[6]==sine[i] && view.words[9]==cosine[i]);
            assert(view.words[13]==sine[i] && view.words[16]==cosine[i]);
        }
        puts("PASS: original extracted CTR1 trig data, forward/reverse caches and non-ideal quarter turn.");
        TaxiTrig trig;memcpy(trig.sine,sine,sizeof sine);memcpy(trig.cosine,cosine,sizeof cosine);
        for(int asset=2;asset<argc;asset++) {
            FILE *file=fopen(argv[asset],"rb");assert(file);
            uint32_t header[4];assert(fread(header,4,4,file)==4 && !memcmp(header,"CTP1",4));
            assert(fseek(file,(long)header[1]*32,SEEK_CUR)==0);
            for(unsigned destination=0;destination<header[3];destination++) {
                uint8_t record[48];assert(fread(record,1,48,file)==48);
                int32_t fields[12];memcpy(fields,record,48);
                assert(!taxi_view_transform_destination(&view,record,48,&trig));
                for(unsigned axis=0;axis<3;axis++) {
                    unsigned angle=(uint32_t)fields[axis+3]&4095;
                    assert(view.words[axis]==fields[axis]);
                    assert(view.words[axis+3]==(int32_t)angle);
                    assert(view.words[axis+6]==sine[angle]);
                    assert(view.words[axis+9]==cosine[angle]);
                }
                TaxiViewTransform before=view;
                assert(taxi_view_transform_destination(&view,record,23,&trig)==-1);
                assert(!memcmp(&view,&before,sizeof view));
            }
            fclose(file);
            printf("PASS: destination camera transforms from %s (%u records).\n",argv[asset],header[3]);
        }
    }
    puts("PASS: original18-word view transform layout, full signed-halfword angles and preserved position.");
}

