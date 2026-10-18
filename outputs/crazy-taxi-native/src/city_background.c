#include "city_background.h"
int taxi_city_background(uint8_t *out,const uint8_t *bg,const uint16_t *bp,
    const uint8_t *strip,const uint16_t *op,unsigned sx,unsigned sy,unsigned cap) {
    if(!out||!bg||!bp||!strip||!op||cap>31)return -1;
    for(unsigned i=0;i<640;i++)if(strip[i]>15)return -1;
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++) {
        unsigned bi=bg[((sy+y)&511)*512+((sx+x)&511)],oi=0;
        if(y>=144&&x<168)oi=strip[x<160?(y-144)*32+x%32:512+(y-144)*8+x-160];
        uint16_t a=oi?op[oi]:bp[bi],b=bp[bi];int mix=oi&&bi;
        for(unsigned c=0;c<3;c++) {
            unsigned av=(a>>(5*c))&31,bv=(b>>(5*c))&31;
            if(av>cap)av=cap;if(bv>cap)bv=cap;
            out[(y*240+x)*4+c]=(uint8_t)((mix?(av+bv)/2:av)*255/31);
        }
        out[(y*240+x)*4+3]=255;
    }
    return 0;
}
