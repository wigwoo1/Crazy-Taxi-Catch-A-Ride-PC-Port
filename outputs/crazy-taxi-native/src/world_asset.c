#include "world_asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t u32(const uint8_t *b) {return b[0]|((uint32_t)b[1]<<8)|((uint32_t)b[2]<<16)|((uint32_t)b[3]<<24);}
static uint32_t u16(const uint8_t *b) {return b[0]|((uint32_t)b[1]<<8);}
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
void taxi_world_asset_free(TaxiWorldAsset *a) {if(a) {free(a->special_storage);free(a->cells);free(a->storage);memset(a,0,sizeof *a);}}
int taxi_world_asset_load_special(TaxiWorldAsset *a,const char *path){
    if(!a||!a->storage||!a->materials||!path)return -1;
    FILE *f=fopen(path,"rb");if(!f)return -1;
    uint8_t header[8];
    if(fread(header,1,8,f)!=8||memcmp(header,"CTS1",4)){fclose(f);return -1;}
    unsigned count=u32(header+4);if(count>16384){fclose(f);return -1;}
    size_t size=(size_t)count*32;uint8_t *records=malloc(size?size:1);
    if(!records){fclose(f);return -1;}
    int valid=fread(records,1,size,f)==size&&fgetc(f)==EOF;fclose(f);
    for(unsigned i=0;valid&&i<count;i++)if(records[32*i]>=a->nm)valid=0;
    if(!valid){free(records);return -1;}
    free(a->special_storage);a->special_storage=records;a->special_count=count;return 0;
}
int taxi_world_asset_load(TaxiWorldAsset *out,const char *path) {
    if(!out||out->storage||out->cells||out->special_storage||!path) return -1;
    FILE *f=fopen(path,"rb");if(!f) return -1;
    if(fseek(f,0,SEEK_END)) {fclose(f);return -1;}
    long size=ftell(f);if(size<56||size>32*1024*1024||fseek(f,0,SEEK_SET)) {fclose(f);return -1;}
    TaxiWorldAsset a={0};a.size=(size_t)size;a.storage=malloc(a.size);
    if(!a.storage) {fclose(f);return -1;}
    int valid=fread(a.storage,1,a.size,f)==a.size&&fgetc(f)==EOF;fclose(f);
    if(!valid||memcmp(a.storage,"CTW1",4)) goto bad;
    a.nv=u32(a.storage+4);a.np=u32(a.storage+8);a.nm=u32(a.storage+12);
    if(!a.nv||a.nv>65535||!a.np||a.np>32768||!a.nm||a.nm>256) goto bad;
    size_t offset=20+(size_t)a.nv*16+(size_t)a.np*24+(size_t)a.nm*28;
    if(offset>a.size||u32(a.storage+16)!=a.size-offset||a.size-offset<36) goto bad;
    a.vertices=a.storage+20;a.polygons=a.vertices+a.nv*16;a.materials=a.polygons+a.np*24;
    const uint8_t *grid=a.storage+offset;if(memcmp(grid,"CTC1",4)) goto bad;
    a.nx=u32(grid+4);a.nz=u32(grid+8);a.sx=bits(u32(grid+12));a.sz=bits(u32(grid+16));
    a.ox=bits(u32(grid+20));a.oz=bits(u32(grid+28));
    if(!a.nx||!a.nz||(uint64_t)a.nx*a.nz>1000000||a.sx<=0||a.sz<=0) goto bad;
    size_t count=(size_t)a.nx*a.nz;
    a.cells=calloc(count,sizeof *a.cells);if(!a.cells) goto bad;
    offset+=32; /* CTC1 header: four-byte magic plus seven words. */
    for(size_t i=0;i<count;i++) {
        if(a.size-offset<4) goto bad;
        uint32_t refs=u32(a.storage+offset);offset+=4;
        if(refs==UINT32_MAX) continue;
        if(refs>65535||a.size-offset<24+(size_t)refs*2) goto bad;
        a.cells[i]=a.storage+offset-4;offset+=24;
        for(uint32_t j=0;j<refs;j++) {uint32_t ref=u16(a.storage+offset+2*j);if(!(ref&32768)&&ref>=a.np) goto bad;}
        offset+=(size_t)refs*2;
    }
    if(offset!=a.size) goto bad;
    *out=a;return 0;
bad:taxi_world_asset_free(&a);return -1;
}
static const uint8_t *cell(TaxiWorldAsset *a,TaxiPoint p) {
    int32_t x=bits((uint32_t)p.x-(uint32_t)a->ox)/a->sx,z=bits((uint32_t)p.z-(uint32_t)a->oz)/a->sz;
    if(x<0||z<0||(uint32_t)x>=a->nx||(uint32_t)z>=a->nz) return NULL;
    return a->cells[(size_t)z*a->nx+(uint32_t)x];
}
int taxi_world_asset_ground(void *data,TaxiPoint p,TaxiGroundHit *hit) {
    TaxiWorldAsset *a=data;if(!a||!a->storage||!hit) return -1;
    const uint8_t *c=cell(a,p);*hit=(TaxiGroundHit){.height=INT32_MIN,.polygon=65535};
    if(!c) return 0;
    return taxi_ground_candidates(a->vertices,a->nv,a->polygons,a->np,c+28,u32(c),p,hit);
}
int taxi_world_asset_contact(void *data,TaxiPoint p,TaxiWorldContact *out) {
    TaxiWorldAsset *a=data;if(!a||!a->storage||!out) return -1;
    const uint8_t *c=cell(a,p);
    if(!c) return 0;
    TaxiGroundHit hit;
    int status=taxi_ground_candidates(a->vertices,a->nv,a->polygons,a->np,
        c+28,u32(c),p,&hit);
    if(status<0) return -1;
    /* Original29664 writes these outputs before2ABA0 tests the height. */
    out->material=hit.material;
    if(hit.contact_valid) out->contact_value=hit.contact_value;
    return taxi_world_contact(out,&hit);
}
int taxi_world_asset_obstacle(void *data,TaxiPoint p,TaxiObstacleHit *hit) {
    TaxiWorldAsset *a=data;if(!a||!a->storage||!hit) return -1;
    const uint8_t *c=cell(a,p);*hit=(TaxiObstacleHit){.height=INT32_MIN,.polygon=65535};
    if(!c) return 0;
    return taxi_obstacle_candidates(a->vertices,a->nv,a->polygons,a->np,a->materials,a->nm,c+28,u32(c),p,hit);
}
int taxi_world_asset_height(void *data,TaxiPoint p,int32_t *height) {
    TaxiWorldAsset *a=data;if(!a||!a->storage||!height) return -1;
    const uint8_t *c=cell(a,p);
    if(!c) {*height=INT32_MIN;return 0;}
    return taxi_ground_height_candidates(a->vertices,a->nv,a->polygons,a->np,
        c+28,u32(c),p,height);
}
