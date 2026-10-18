#include "world_render.h"
#include "world_polygon_stamp.h"
#include <string.h>
static uint32_t u16(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8);}
static uint32_t u32(const uint8_t *p){return u16(p)|(u16(p+2)<<16);}
static int visit_cells(const TaxiWorldAsset *w,int32_t min_x,int32_t min_z,
    int32_t max_x,int32_t max_z,uint8_t *seen,size_t seen_size,
    TaxiWorldCellVisible visible,TaxiWorldReferenceVisit visit,void *context,uint32_t *stamps,size_t stamp_count,uint32_t generation,uint32_t *special_stamps,size_t special_stamp_count){
    if(!w||!w->cells||!w->nx||!w->nz||(!stamps&&(!seen||seen_size<w->np))||(stamps&&stamp_count<w->np)||!visit)return -1;
    if(special_stamps&&special_stamp_count<w->special_count)return -1;
    if(!stamps)memset(seen,0,w->np);
    if(min_x<0)min_x=0;if(min_z<0)min_z=0;
    if(max_x>=(int32_t)w->nx)max_x=(int32_t)w->nx-1;
    if(max_z>=(int32_t)w->nz)max_z=(int32_t)w->nz-1;
    if(min_x>max_x||min_z>max_z)return 0;
    for(int32_t z=min_z;z<=max_z;z++)for(int32_t x=min_x;x<=max_x;x++){
        const uint8_t *cell=w->cells[(size_t)z*w->nx+(unsigned)x];
        if(!cell)continue;
        if(visible){int rc=visible(context,(unsigned)x,(unsigned)z);if(rc<0)return -1;if(!rc)continue;}
        unsigned count=u32(cell);
        for(unsigned i=0;i<count;i++){
            unsigned reference=u16(cell+28+2*i);
            if(!(reference&0x8000u)){
                if(reference>=w->np)return -1;
                if(stamps){int gate=taxi_world_polygon_stamp(stamps,stamp_count,generation,reference);if(gate<0)return -1;if(!gate)continue;}
                else {if(seen[reference])continue;seen[reference]=1;}
            }
            else if(special_stamps){
                int gate=taxi_world_special_stamp(special_stamps,special_stamp_count,generation,reference);
                if(gate<0)return -1;if(!gate)continue;
            }
            if(visit(context,reference)<0)return -1;
        }
    }
    return 0;
}
int taxi_world_render_visit(const TaxiWorldAsset *w,int32_t x,int32_t z,int32_t xx,int32_t zz,uint8_t *seen,size_t n,TaxiWorldCellVisible visible,TaxiWorldReferenceVisit visit,void *context){return visit_cells(w,x,z,xx,zz,seen,n,visible,visit,context,NULL,0,0,NULL,0);}
int taxi_world_render_visit_stamped(const TaxiWorldAsset *w,int32_t x,int32_t z,int32_t xx,int32_t zz,uint32_t *stamps,size_t n,uint32_t generation,TaxiWorldCellVisible visible,TaxiWorldReferenceVisit visit,void *context){if(!stamps)return -1;return visit_cells(w,x,z,xx,zz,NULL,0,visible,visit,context,stamps,n,generation,NULL,0);}
int taxi_world_render_visit_cached(const TaxiWorldAsset *w,int32_t x,int32_t z,int32_t xx,int32_t zz,uint32_t *normal,size_t n,uint32_t *special,size_t sn,uint32_t generation,TaxiWorldCellVisible visible,TaxiWorldReferenceVisit visit,void *context){
 if(!normal||!special)return -1;
 return visit_cells(w,x,z,xx,zz,NULL,0,visible,visit,context,normal,n,generation,special,sn);
}
