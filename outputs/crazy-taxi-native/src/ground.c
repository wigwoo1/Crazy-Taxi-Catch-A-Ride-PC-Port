#include "ground.h"
#include <limits.h>
#include <string.h>
static int32_t bits(uint32_t u) { int32_t s; memcpy(&s,&u,4); return s; }
static int32_t add(int32_t a,int32_t b) { return bits((uint32_t)a+(uint32_t)b); }
static int32_t sub(int32_t a,int32_t b) { return bits((uint32_t)a-(uint32_t)b); }
static int32_t mul(int32_t a,int32_t b) { return bits((uint32_t)a*(uint32_t)b); }
static int32_t asr(int32_t a,unsigned n) {
    return bits(((uint32_t)a>>n)|(a<0 ? (UINT32_MAX<<(32-n)):0));
}
static uint16_t u16(const uint8_t *p) { return (uint16_t)(p[0]|(p[1]<<8)); }
static int32_t i32(const uint8_t *p) {
    return bits((uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24));
}
static int32_t s8(uint8_t u) { return u<128 ? u : (int32_t)u-256; }
static TaxiPoint vertex(const uint8_t *p) { return (TaxiPoint){i32(p),i32(p+4),i32(p+8)}; }
int taxi_world_contact(TaxiWorldContact *out,const TaxiGroundHit *hit) {
    if(!out||!hit||hit->height==INT32_MIN) return 0;
    out->material=hit->material;out->height=hit->height;out->normal=hit->normal;
    out->flags|=1;out->red=out->green=out->blue=0;
    if(hit->contact_valid) out->contact_value=hit->contact_value;
    return hit->normal.x!=0||hit->normal.y!=0||hit->normal.z!=0;
}
/* 080051cc: midpoint-rounded edge tests, not floating barycentrics.
   Zero joins the nonpositive side; winding changes boundary inclusion. */
static int32_t edge(TaxiPoint p,TaxiPoint a,TaxiPoint b) {
    return add(mul(sub(p.x,asr(add(a.x,b.x),1)),sub(a.z,b.z)),
               mul(sub(p.z,asr(add(a.z,b.z),1)),sub(b.x,a.x)));
}
static int contains(TaxiPoint p,TaxiPoint a,TaxiPoint b,TaxiPoint c) {
    int n=(edge(p,a,b)>0)+(edge(p,b,c)>0)+(edge(p,c,a)>0);
    return n==0 || n==3;
}
static int32_t div32(int32_t a,int32_t b) {
    /* 080029fc returns the dividend for zero divisor; avoid host UB. */
    return b ? bits((uint32_t)((int64_t)a/b)) : a;
}
static void unwrap(int32_t *a,int32_t *b) {
    int32_t d=sub(*a,*b);
    if(d<0) d=sub(*b,*a);
    if(d>32768) {
        if(*a<*b) *a=add(*a,65536);else *b=add(*b,65536);
    }
}
static int32_t lerp(int32_t a,int32_t b,int32_t t) {
    return add(a,asr(mul(sub(b,a),t),12));
}
int taxi_ground_contact(int32_t x,int32_t z,const uint8_t *a,
    const uint8_t *b,const uint8_t *c,uint32_t *output) {
    if(!output || !a || !b || !c) return 0;
    const uint8_t *ends[3]={a,b,c},*previous=c;
    const uint8_t *down_high=NULL,*down_low=NULL,*up_high=NULL,*up_low=NULL;
    for(int j=0;j<3;j++) {
        const uint8_t *next=ends[j];
        int32_t pz=i32(previous+8),nz=i32(next+8);
        if(!down_high && pz>nz && z>=nz && z<=pz) { down_high=previous;down_low=next; }
        if(!up_high && pz<nz && z>=pz && z<=nz) { up_high=next;up_low=previous; }
        previous=next;
    }
    if(!down_high || !up_high) return 0;
    int32_t d0=i32(down_high+12),d1=i32(down_low+12);
    unwrap(&d0,&d1);
    int32_t t=div32(mul(sub(z,i32(down_high+8)),4096),sub(i32(down_low+8),i32(down_high+8)));
    int32_t dx=lerp(i32(down_high),i32(down_low),t),dv=lerp(d0,d1,t);
    int32_t u0=i32(up_high+12),u1=i32(up_low+12);
    unwrap(&u0,&u1);
    t=div32(mul(sub(z,i32(up_high+8)),4096),sub(i32(up_low+8),i32(up_high+8)));
    int32_t ux=lerp(i32(up_high),i32(up_low),t),uv=lerp(u0,u1,t);
    t=ux==dx ? 0 : div32(mul(sub(x,dx),4096),sub(ux,dx));
    dv=(int32_t)((uint32_t)dv&65535);uv=(int32_t)((uint32_t)uv&65535);
    unwrap(&dv,&uv);
    *output=(uint32_t)add(mul(dv,256),asr(mul(sub(uv,dv),t),4))&0xffffff;
    return 1;
}
static int ground_query(const uint8_t *vertices,size_t nv,
    const uint8_t *polygons,size_t np,const uint8_t *refs,size_t count,
    TaxiPoint point,TaxiGroundHit *hit,int contact) {
    if (!hit || (!vertices && nv) || (!polygons && np) || (!refs && count)
        || nv>65535 || np>32768 || count>SIZE_MAX/2) return -1;
    *hit=(TaxiGroundHit){.height=INT32_MIN,.polygon=65535,.material=65535};
    int32_t best=INT32_MAX;
    int found=0;
    for (size_t k=0;k<count;k++) {
        uint16_t index=u16(refs+k*2);
        if (index&0x8000) continue;
        if (index>=np) return -1;
        const uint8_t *f=polygons+(size_t)index*24;
        uint16_t ids[4]={u16(f+8),u16(f+10),u16(f+12),u16(f+14)};
        int quad=ids[3]!=65535;
        for (int j=0;j<(quad?4:3);j++) if (ids[j]>=nv) return -1;
        TaxiPoint a=vertex(vertices+(size_t)ids[0]*16);
        TaxiPoint c=vertex(vertices+(size_t)ids[2]*16);
        int second=quad && edge(point,a,c)<0;
        TaxiPoint b=vertex(vertices+(size_t)ids[second?2:1]*16);
        if (second) c=vertex(vertices+(size_t)ids[3]*16);
        const uint8_t *nb=f+(second?5:2);
        TaxiPoint n={s8(nb[0]),s8(nb[1]),s8(nb[2])};
        int32_t ax=n.x<0?-n.x:n.x, ay=n.y<0?-n.y:n.y, az=n.z<0?-n.z:n.z;
        if (ay<=ax || ay<=az || !contains(point,a,b,c)) continue;
        int32_t plane=add(add(mul(a.x,n.x),mul(a.y,n.y)),mul(a.z,n.z));
        /* Original ASR 8 then LSL 8 discards low bits before solving Y. */
        plane=bits((uint32_t)plane&0xffffff00u);
        int32_t numerator=sub(sub(plane,mul(point.x,n.x)),mul(point.z,n.z));
        int32_t height=bits((uint32_t)((int64_t)numerator/n.y));
        int32_t distance=sub(point.y,height);
        if (distance<0) distance=sub(height,point.y);
        if (point.y!=INT32_MIN && distance>=best) continue;
        best=distance;
        hit->height=height;hit->normal=(TaxiPoint){n.x*32,n.y*32,n.z*32};
        hit->polygon=index;hit->material=point.y==INT32_MIN?65535:f[0];
        found=1;
        if (point.y==INT32_MIN) return 1;
        if(contact&&taxi_ground_contact(point.x,point.z,vertices+(size_t)ids[0]*16,
            vertices+(size_t)ids[second?2:1]*16,vertices+(size_t)ids[second?3:2]*16,
            &hit->contact_value)) hit->contact_valid=1;
    }
    return found;
}
int taxi_ground_candidates(const uint8_t *vertices,size_t nv,
    const uint8_t *polygons,size_t np,const uint8_t *refs,size_t count,
    TaxiPoint point,TaxiGroundHit *hit) {
    return ground_query(vertices,nv,polygons,np,refs,count,point,hit,1);
}
int taxi_ground_height_candidates(const uint8_t *vertices,size_t nv,
    const uint8_t *polygons,size_t np,const uint8_t *refs,size_t count,
    TaxiPoint point,int32_t *height) {
    if(!height) return -1;
    TaxiGroundHit hit;
    int status=ground_query(vertices,nv,polygons,np,refs,count,point,&hit,0);
    if(status>=0) *height=hit.height;
    return status;
}
int taxi_obstacle_candidates(const uint8_t *vertices,size_t nv,
    const uint8_t *polygons,size_t np,const uint8_t *materials,size_t nm,
    const uint8_t *refs,size_t count,TaxiPoint point,TaxiObstacleHit *hit) {
    if(!hit || (!vertices && nv) || (!polygons && np) || (!materials && nm)
        || (!refs && count) || nv>65535 || np>32768 || nm>256 || count>SIZE_MAX/2) return -1;
    *hit=(TaxiObstacleHit){.height=INT32_MIN,.polygon=65535};
    int32_t best=INT32_MAX;
    int found=0;
    for(size_t k=0;k<count;k++) {
        uint16_t index=u16(refs+k*2);if(index&0x8000) continue;
        if(index>=np) return -1;
        const uint8_t *f=polygons+(size_t)index*24;
        if(f[0]>=nm) return -1;
        uint16_t ids[4]={u16(f+8),u16(f+10),u16(f+12),u16(f+14)};
        unsigned corners=ids[3]==65535?3:4;
        TaxiPoint pts[4]={0};
        for(unsigned j=0;j<corners;j++) {
            if(ids[j]>=nv) return -1;
            pts[j]=vertex(vertices+(size_t)ids[j]*16);
        }
        int second=corners==4 && edge(point,pts[0],pts[2])<0;
        TaxiPoint a=pts[0],b=pts[second?2:1],c=pts[second?3:2];
        if(materials[(size_t)f[0]*28+20]&1) {
            if(!contains(point,a,b,c)) continue;
            hit->height=INT32_MAX;hit->polygon=index;hit->edge_count=corners;
            memcpy(hit->vertices,pts,sizeof pts);return 2;
        }
        const uint8_t *nb=f+(second?5:2);
        TaxiPoint n={s8(nb[0]),s8(nb[1]),s8(nb[2])};
        int32_t ax=n.x<0?-n.x:n.x,az=n.z<0?-n.z:n.z;
        if(n.y<=ax || n.y<=az || !contains(point,a,b,c)) continue;
        int32_t plane=add(add(mul(a.x,n.x),mul(a.y,n.y)),mul(a.z,n.z));
        plane=bits((uint32_t)plane&0xffffff00u);
        int32_t height=div32(sub(sub(plane,mul(point.x,n.x)),mul(point.z,n.z)),n.y);
        int32_t distance=sub(point.y,height);
        if(distance<0) distance=sub(height,point.y);
        if(point.y!=INT32_MIN && distance>=best) continue;
        best=distance;hit->height=height;found=1;
        /* Original does not assign the polygon pointer on ordinary heights. */
        if(point.y==INT32_MIN) return 1;
    }
    return found;
}
