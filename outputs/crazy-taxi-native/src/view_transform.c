#include "view_transform.h"
#include <string.h>
int taxi_view_transform_destination(TaxiViewTransform *out,const uint8_t *record,
    size_t size,const TaxiTrig *trig) {
    if(!out||!record||!trig||size<24)return -1;
    int32_t fields[6];
    for(unsigned i=0;i<6;i++) {
        const uint8_t *p=record+i*4;
        uint32_t bits=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
        memcpy(&fields[i],&bits,4);
    }
    TaxiViewTransform result;
    taxi_view_transform_position(&result,fields[0],fields[1],fields[2]);
    taxi_view_transform_angles(&result,fields[3],fields[4],fields[5],trig->sine,trig->cosine);
    *out=result;return 0;
}
void taxi_view_transform_position(TaxiViewTransform *out,int32_t x,int32_t y,int32_t z) {
    out->words[0]=x;out->words[1]=y;out->words[2]=z;
}
void taxi_view_transform_angles(TaxiViewTransform *out,int32_t x,int32_t y,int32_t z,
    const int32_t sine[4096],const int32_t cosine[4096]) {
    const int32_t angles[3]={x,y,z};
    for(unsigned axis=0;axis<3;axis++) {
        unsigned forward=(uint32_t)angles[axis]&4095;
        unsigned reverse=(0u-(uint32_t)angles[axis])&4095;
        out->words[3+axis]=(int32_t)forward;
        out->words[6+axis]=sine[forward];out->words[9+axis]=cosine[forward];
        out->words[12+axis]=sine[reverse];out->words[15+axis]=cosine[reverse];
    }
}
