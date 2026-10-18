#include "impact.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static int32_t magnitude(int32_t a) {return a<0?sub(0,a):a;}
static int32_t clamp(int32_t a,int32_t lo,int32_t hi) {return a<lo?lo:a>hi?hi:a;}
void taxi_trig_decode(TaxiTrig *out,const uint8_t *sine,const uint8_t *cosine) {
    for(unsigned i=0;i<4096;i++) {
        uint32_t a=sine[2*i]|((uint32_t)sine[2*i+1]<<8);
        uint32_t b=cosine[2*i]|((uint32_t)cosine[2*i+1]<<8);
        out->sine[i]=asr(bits(a<<16),20);
        out->cosine[i]=asr(bits(b<<16),20);
    }
}
/* Column-major 4x4 arithmetic of 08000738. Shift only after each sum. */
static void product(const int32_t a[16],const int32_t b[16],int32_t out[16]) {
    for(unsigned j=0;j<4;j++) for(unsigned i=0;i<4;i++) {
        int32_t sum=0;
        for(unsigned k=0;k<4;k++) sum=add(sum,mul(a[k*4+i],b[j*4+k]));
        out[j*4+i]=asr(sum,8);
    }
}
static void rotation(int32_t out[16],unsigned axis,int32_t angle,const TaxiTrig *t) {
    memset(out,0,16*sizeof *out);out[0]=out[5]=out[10]=out[15]=256;
    int32_t s=t->sine[(uint32_t)angle&4095],c=t->cosine[(uint32_t)angle&4095];
    if(axis==0) {out[5]=out[10]=c;out[9]=s;out[6]=sub(0,s);}
    if(axis==1) {out[0]=out[10]=c;out[2]=s;out[8]=sub(0,s);}
    if(axis==2) {out[0]=out[5]=c;out[1]=s;out[4]=sub(0,s);}
}
static int32_t projection(const int32_t m[16],unsigned axis,TaxiPoint normal) {
    int32_t v[3];
    for(unsigned i=0;i<3;i++) v[i]=add(asr(mul(m[axis*4+i],-4096),8),m[12+i]);
    return asr(add(add(mul(v[0],normal.x),mul(v[1],normal.y)),mul(v[2],normal.z)),7);
}
TaxiPoint taxi_rotate_y(TaxiPoint p,int32_t angle,const TaxiTrig *t) {
    int32_t s=t->sine[(uint32_t)angle&4095],c=t->cosine[(uint32_t)angle&4095];
    return (TaxiPoint){asr(sub(mul(p.x,c),mul(p.z,s)),8),
        asr(mul(p.y,256),8),asr(add(mul(p.x,s),mul(p.z,c)),8)};
}
TaxiPoint taxi_rotate_point(TaxiPoint p,int32_t x,int32_t y,int32_t z,const TaxiTrig *t) {
    int32_t rx[16],ry[16],rz[16],tmp[16],m[16],v[3];
    rotation(rx,0,x,t);rotation(ry,1,y,t);rotation(rz,2,z,t);
    product(ry,rx,tmp);product(tmp,rz,m);
    for(unsigned i=0;i<3;i++) v[i]=add(asr(add(add(mul(p.x,m[i]),mul(p.y,m[4+i])),mul(p.z,m[8+i])),8),m[12+i]);
    return (TaxiPoint){v[0],v[1],v[2]};
}
TaxiPoint taxi_rotate_camera(TaxiPoint p,int32_t x,int32_t y,int32_t z,const TaxiTrig *t) {
    int32_t rx[16],ry[16],rz[16],tmp[16],m[16],v[3];
    rotation(rx,0,x,t);rotation(ry,1,y,t);rotation(rz,2,z,t);
    product(rz,rx,tmp);product(tmp,ry,m);
    for(unsigned i=0;i<3;i++) v[i]=add(asr(add(add(mul(p.x,m[i]),mul(p.y,m[4+i])),mul(p.z,m[8+i])),8),m[12+i]);
    return (TaxiPoint){v[0],v[1],v[2]};
}
void taxi_vehicle_impact(TaxiImpactState *v,TaxiPoint n,uint8_t settings,
    uint8_t submode,const TaxiTrig *trig,TaxiImpactEffects *effects) {
    int32_t rx[16],ry[16],rz[16],tmp[16],matrix[16];
    rotation(rx,0,v->angle_18,trig);
    rotation(ry,1,add(v->angle_1c,v->angle_69c)/4096,trig);
    rotation(rz,2,v->angle_1a,trig);
    product(ry,rx,tmp);product(tmp,rz,matrix);
    int32_t px=projection(matrix,0,n),pz=projection(matrix,2,n);
    int special=(settings&0xe0)==0x80 && submode==0;
    int32_t strength=clamp(magnitude(v->speed_544)/1138,special?16:32,special?32:64);
    v->state_748=mul(px,strength)/(special?8192:4096);
    v->state_73c=mul(pz,strength)/(special?16384:8192);
    v->state_74c=mul(n.x,special?1024:2048);
    v->state_750=mul(n.z,special?1024:2048);
    int32_t loss=clamp(magnitude(pz),1024,3584);
    v->speed_544=sub(v->speed_544,mul(v->speed_544,loss)/4096);
    unsigned level=(unsigned)((loss-1024)/854);
    if(level>2) level=2;
    unsigned slot;
    if(px<0) {
        /* 0802ac0a CMN uses signed comparison without overflowing its sum. */
        if((int64_t)magnitude(pz)+(int64_t)px<=0) slot=0;
        else slot=pz<0?4:5;
    }else if(magnitude(pz)<=px) slot=1;
    else slot=pz<0?4:5;
    v->state_34=((px<0)!=(pz<0))?-0x9c71c:0x9c71c;
    *effects=(TaxiImpactEffects){px,pz,41+level,slot};
}
