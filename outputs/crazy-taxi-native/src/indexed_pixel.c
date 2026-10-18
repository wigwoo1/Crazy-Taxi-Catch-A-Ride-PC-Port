#include "indexed_pixel.h"
#include "pair_column_setup.h"
#include "pair_row_raster.h"
#include <string.h>
int taxi_indexed_sampling(TaxiIndexedSampling *out, unsigned width,
    unsigned height, unsigned log_width, unsigned level) {
    if (!out || log_width>7 || width!=(1u<<log_width) || !height ||
        height>128 || (height&(height-1)) || level>log_width || !(height>>level)) return 0;
    unsigned row_shift=log_width-level;
    *out=(TaxiIndexedSampling){(width>>level)-1,
        ((height>>level)-1)<<row_shift,14+level,14+level-row_shift};
    return 1;
}
static int32_t signed_bits(uint32_t value) {
    int32_t result; memcpy(&result,&value,4); return result;
}
static uint32_t quotient(uint32_t difference, int32_t width) {
    return (uint32_t)((int64_t)signed_bits(difference)/width);
}
int taxi_indexed_span_prepare(TaxiIndexedSpan *out, TaxiIndexedEdge left,
    TaxiIndexedEdge right, int32_t clip_left, int32_t clip_right) {
    int32_t width=signed_bits((uint32_t)right.x-(uint32_t)left.x);
    if (!out || width<=0 || left.x>=clip_right || right.x<clip_left) return 0;
    uint32_t du=quotient(right.u-left.u,width);
    uint32_t dv=quotient(right.v-left.v,width);
    uint32_t ds=quotient(right.shade-left.shade,width);
    int32_t advance=signed_bits((uint32_t)clip_left-(uint32_t)left.x);
    if (advance>0) {
        left.u+=du*(uint32_t)advance;
        left.v+=dv*(uint32_t)advance;
        left.shade+=ds*(uint32_t)advance;
        left.x=clip_left;
    }
    if (right.x>clip_right) right.x=clip_right;
    /* ARM BIC rounds the output address down without adjusting attributes. */
    left.x=signed_bits((uint32_t)left.x&~1u);
    int32_t pixels=signed_bits((uint32_t)right.x-(uint32_t)left.x);
    /* Host guard excludes malformed limits; valid source spans are positive. */
    if (pixels<=0) return 0;
    *out=(TaxiIndexedSpan){left.x,((size_t)pixels+1)/2,
        left.u,left.v,left.shade,du*2,dv*2,ds*2};
    return 1;
}
static uint32_t arm_asr(uint32_t value, unsigned shift) {
    shift &= 255;
    if (!shift) return value;
    if (shift >= 32) return value & 0x80000000u ? UINT32_MAX : 0;
    return (value >> shift) | (value & 0x80000000u ? UINT32_MAX << (32-shift) : 0);
}
static uint32_t source_divide(uint32_t value, uint32_t divisor) {
    return divisor ? (uint32_t)((int64_t)signed_bits(value)/signed_bits(divisor)) : value;
}
static int indexed_edges(TaxiIndexedEdge (*rows)[2], size_t row_count,
    const TaxiIndexedVertex *vertices, size_t count, int32_t top, int32_t bottom,
    uint32_t far_depth, uint32_t shade_depth, int32_t *first, int32_t *last,int raw_depth) {
    if (!rows || !vertices || !first || !last || count<3 || count>8 ||
        top<0 || bottom<=top || (size_t)bottom>row_count) return 0;
    int32_t min_y=bottom,max_y=top;
    for (size_t i=0;i<count;++i) {
        TaxiIndexedVertex a=vertices[i],b=vertices[(i+count-1)%count];
        unsigned side=0;
        if (a.y==b.y) continue;
        if (a.y>b.y) { TaxiIndexedVertex temp=a;a=b;b=temp;side=1; }
        if (a.y>=bottom || b.y<top) continue;
        int32_t height=signed_bits((uint32_t)b.y-(uint32_t)a.y);
        if (height<=0) return 0;
        uint32_t shade_a=raw_depth?a.depth:4096+source_divide((a.depth-(far_depth-shade_depth))<<15,shade_depth);
        uint32_t shade_b=raw_depth?b.depth:4096+source_divide((b.depth-(far_depth-shade_depth))<<15,shade_depth);
        uint32_t x=(uint32_t)a.x<<12;
        uint32_t dx=quotient(((uint32_t)b.x<<12)-x,height);
        uint32_t du=quotient(b.u-a.u,height),dv=quotient(b.v-a.v,height);
        uint32_t ds=quotient(shade_b-shade_a,height);
        int32_t y=a.y,end=b.y;
        if (y<top) {
            uint32_t advance=(uint32_t)top-(uint32_t)y;
            x+=dx*advance;a.u+=du*advance;a.v+=dv*advance;shade_a+=ds*advance;y=top;
        }
        if (end>bottom) end=bottom;
        if (y<min_y) min_y=y;
        if (end>max_y) max_y=end;
        /* Original loop executes once even when clipping leaves zero rows. */
        int32_t iterations=end>y?end-y:1;
        for (int32_t n=0;n<iterations;++n) {
            if ((size_t)y>=row_count) return 0;
            rows[y++][side]=(TaxiIndexedEdge){signed_bits(arm_asr(x,12)),a.u,a.v,shade_a};
            x+=dx;a.u+=du;a.v+=dv;shade_a+=ds;
        }
    }
    *first=min_y;*last=max_y;
    return 1;
}
int taxi_indexed_edges(TaxiIndexedEdge (*rows)[2], size_t row_count,
    const TaxiIndexedVertex *vertices,size_t count,int32_t top,int32_t bottom,
    uint32_t far_depth,uint32_t shade_depth,int32_t *first,int32_t *last) {
    return indexed_edges(rows,row_count,vertices,count,top,bottom,far_depth,
        shade_depth,first,last,0);
}
int taxi_indexed_perspective_rows_bounds(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right) {
    if(!spans || !vertices || count<3 || count>8 || left>=right ||
       left < -1024 || right>1024 || (left&1) || (right&1))return 0;
    TaxiIndexedSpan prepared[160];memset(prepared,0,sizeof prepared);
    TaxiIndexedVertex input[8];
    for(size_t i=0;i<count;++i) {
        input[i]=vertices[i];
        if(!input[i].depth)return 0;
        input[i].u=source_divide(input[i].u<<8,input[i].depth);
        input[i].v=source_divide(input[i].v<<8,input[i].depth);
        input[i].depth=source_divide(0x40000000,vertices[i].depth)<<6;
    }
    TaxiIndexedEdge rows[160][2];memset(rows,0,sizeof rows);
    int32_t first,last;
    if(!indexed_edges(rows,160,input,count,0,160,0,0,&first,&last,1))return 0;
    for(int32_t y=first;y<last;++y) {
        TaxiIndexedEdge a=rows[y][0],b=rows[y][1];
        uint32_t descriptor[8]={(uint32_t)a.x,a.u,a.v,a.shade,(uint32_t)b.x,b.u,b.v,b.shade};
        TaxiPairRowPrepared row;
        int status=taxi_pair_row_prepare_perspective(&row,descriptor,left,right);
        if(status<0)return 0;if(!status)continue;
        prepared[y]=(TaxiIndexedSpan){signed_bits(row.column),row.pairs,row.x,row.y,0,row.dx,row.dy,0};
    }
    memcpy(spans,prepared,sizeof prepared);
    return 1;
}

int taxi_indexed_perspective_rows_spans(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count) {
    return taxi_indexed_perspective_rows_bounds(spans,vertices,count,0,240);
}
int taxi_indexed_perspective_rows_polygon_work(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling,TaxiPairColumnWork *work) {
    if(!framebuffer || !texture)return 0;
    TaxiIndexedSpan spans[160];
    if(!taxi_indexed_perspective_rows_spans(spans,vertices,count))return 0;
    uint8_t staged[38400];memcpy(staged,framebuffer,sizeof staged);TaxiPairColumnWork total={0};
    for(unsigned y=0;y<160;y++) {
        const TaxiIndexedSpan *span=spans+y;if(!span->pairs)continue;
        uint32_t u=span->u,v=span->v;TaxiPairColumnWork part;
        if(taxi_pair_row_raster(staged,y*240+(unsigned)span->x,texture,texture_size,
            sampling.umask,sampling.vmask,sampling.ushift,sampling.vshift,&u,&v,span->du,span->dv,(unsigned)span->pairs,&part))return 0;
        total.texture_byte_reads+=part.texture_byte_reads;total.framebuffer_halfword_writes+=part.framebuffer_halfword_writes;
    }
    memcpy(framebuffer,staged,sizeof staged);if(work)*work=total;return 1;
}
int taxi_indexed_perspective_rows_polygon(uint8_t framebuffer[38400],const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,size_t texture_size,TaxiIndexedSampling sampling){return taxi_indexed_perspective_rows_polygon_work(framebuffer,vertices,count,texture,texture_size,sampling,0);}
int taxi_indexed_keyed_span(uint8_t *output, size_t pairs,
    const uint8_t *texture, size_t texture_size, const uint8_t lookup[2560],
    uint32_t u, uint32_t v, uint32_t shade, uint32_t du, uint32_t dv,
    uint32_t dshade, uint32_t umask, uint32_t vmask,
    unsigned ushift, unsigned vshift) {
    /* Validate all addresses first, preserving output for invalid host input. */
    if (!output || !texture || !lookup || pairs > SIZE_MAX/2) return 0;
    uint32_t check_u=u,check_v=v;
    for (size_t i=0;i<pairs;++i) {
        uint32_t index=(arm_asr(check_u,ushift)&umask)+(arm_asr(check_v,vshift)&vmask);
        if (index>=texture_size) return 0;
        check_u+=du; check_v+=dv;
    }
    for (size_t i=0;i<pairs;++i) {
        uint32_t index=(arm_asr(u,ushift)&umask)+(arm_asr(v,vshift)&vmask);
        int32_t signed_shade;
        memcpy(&signed_shade,&shade,sizeof signed_shade);
        taxi_indexed_keyed_pair(output+i*2,texture[index],signed_shade,lookup);
        u+=du; v+=dv; shade+=dshade;
    }
    return 1;
}
static int affine_rows_bounds(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right,int clipped,
    uint32_t far_depth,uint32_t shade_depth,int shaded) {
    if(!spans||!vertices||count<3||count>8||left>=right||left < -1024||right>1024||
       (left&1)||(right&1))return 0;
    if(!clipped)for(size_t i=0;i<count;i++)
        if(vertices[i].x<left||vertices[i].x>right||vertices[i].y<0||vertices[i].y>=160)return 0;
    TaxiIndexedSpan prepared[160];memset(prepared,0,sizeof prepared);
    TaxiIndexedEdge rows[160][2];memset(rows,0,sizeof rows);int32_t first,last;
    if(!taxi_indexed_edges(rows,160,vertices,count,0,160,far_depth,shade_depth,&first,&last))return 0;
    for(int32_t y=first;y<last;y++) {
        TaxiIndexedEdge a=rows[y][0],b=rows[y][1];TaxiIndexedSpan span;
        if(clipped) {
            if(!taxi_indexed_span_prepare(&span,a,b,left,right))continue;
        }else {
            a.x=signed_bits((uint32_t)a.x&~1u);
            int32_t width=b.x-a.x;if(width<=0)continue;
            span=(TaxiIndexedSpan){a.x,((size_t)width+1)/2,a.u,a.v,0,
                quotient(b.u-a.u,width)*2,quotient(b.v-a.v,width)*2,0};
        }
        if(span.x<left||(int64_t)span.x+(int64_t)span.pairs*2>right)return 0;
        if(!shaded){span.shade=0;span.dshade=0;}prepared[y]=span;
    }
    memcpy(spans,prepared,sizeof prepared);return 1;
}
int taxi_indexed_affine_rows_bounds(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right,int clipped) {
    return affine_rows_bounds(spans,vertices,count,left,right,clipped,0,0,0);
}
int taxi_indexed_shaded_rows_bounds(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right,
    uint32_t far_depth,uint32_t shade_depth) {
    return affine_rows_bounds(spans,vertices,count,left,right,1,far_depth,shade_depth,1);
}
int taxi_indexed_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices, size_t count,
    const uint8_t *texture, size_t texture_size, TaxiIndexedSampling sampling,
    const uint8_t lookup[2560], int32_t left, int32_t top,
    int32_t right, int32_t bottom, uint32_t far_depth, uint32_t shade_depth) {
    if (!framebuffer || !texture || !lookup || left<0 || right>240 ||
        left>=right || top<0 || bottom>160 || top>=bottom) return 0;
    TaxiIndexedEdge rows[160][2];
    memset(rows,0,sizeof rows);
    int32_t first,last;
    if (!taxi_indexed_edges(rows,160,vertices,count,top,bottom,
        far_depth,shade_depth,&first,&last)) return 0;
    for (int32_t y=first;y<last;++y) {
        TaxiIndexedSpan span;
        if (!taxi_indexed_span_prepare(&span,rows[y][0],rows[y][1],left,right)) continue;
        if (span.x<0 || (size_t)span.x+span.pairs*2>240) return 0;
        if (!taxi_indexed_keyed_span(framebuffer+(size_t)y*240+span.x,span.pairs,
            texture,texture_size,lookup,span.u,span.v,span.shade,
            span.du,span.dv,span.dshade,sampling.umask,sampling.vmask,
            sampling.ushift,sampling.vshift)) return 0;
    }
    return 1;
}
int taxi_indexed_unshaded_clipped_rows_bounds(TaxiIndexedSpan spans[160],const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right){
 if(!spans||!vertices||left>=right||left < -1024||right>1024||(left&1)||(right&1))return 0;
 TaxiIndexedEdge rows[160][2]={0};TaxiIndexedSpan prepared[160]={0};int32_t first,last;
 if(!taxi_indexed_edges(rows,160,vertices,count,0,160,0,0,&first,&last))return 0;
 for(int32_t row=first;row<last;row++){TaxiIndexedEdge a=rows[row][0],b=rows[row][1];uint32_t d[8]={(uint32_t)a.x,a.u,a.v,0,(uint32_t)b.x,b.u,b.v,0};TaxiPairRowPrepared p;int status=taxi_pair_row_prepare_bounds(&p,d,left,right);if(status<0)return 0;if(!status)continue;prepared[row]=(TaxiIndexedSpan){signed_bits(p.column),p.pairs,p.x,p.y,0,p.dx,p.dy,0};}
 memcpy(spans,prepared,sizeof prepared);return 1;
}
int taxi_indexed_unshaded_clipped_polygon_work(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling,TaxiPairColumnWork *work){
    if(!framebuffer||!vertices||!texture)return 0;
    TaxiIndexedEdge rows[160][2];memset(rows,0,sizeof rows);int32_t first,last;
    if(!taxi_indexed_edges(rows,160,vertices,count,0,160,0,0,&first,&last))return 0;
    if(first>=last){if(work)*work=(TaxiPairColumnWork){0};return 1;}
    uint32_t descriptors[160*8]={0};
    for(int32_t row=first;row<last;row++){TaxiIndexedEdge a=rows[row][0],b=rows[row][1];uint32_t *d=descriptors+(row-first)*8;d[0]=(uint32_t)a.x;d[1]=a.u;d[2]=a.v;d[4]=(uint32_t)b.x;d[5]=b.u;d[6]=b.v;}
    return taxi_pair_row_batch_clipped(framebuffer,(unsigned)first,(unsigned)(last-first),descriptors,0,240,
        texture,texture_size,sampling.umask,sampling.vmask,sampling.ushift,sampling.vshift,work)?0:1;
}
int taxi_indexed_unshaded_polygon_work(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling,TaxiPairColumnWork *work) {
    if(!framebuffer || !vertices || !texture || count<3 || count>8)return 0;
    for(size_t i=0;i<count;++i)
        if(vertices[i].x<0 || vertices[i].x>240 || vertices[i].y<0 || vertices[i].y>=160)return 0;
    TaxiIndexedEdge rows[160][2];memset(rows,0,sizeof rows);
    int32_t first,last;
    if(!taxi_indexed_edges(rows,160,vertices,count,0,160,0,0,&first,&last))return 0;
    if(first>=last){if(work)*work=(TaxiPairColumnWork){0};return 1;}
    uint32_t descriptors[160*8]={0};
    for(int32_t row=first;row<last;row++){
        TaxiIndexedEdge left=rows[row][0],right=rows[row][1];uint32_t *d=descriptors+(row-first)*8;
        d[0]=(uint32_t)left.x;d[1]=left.u;d[2]=left.v;d[4]=(uint32_t)right.x;d[5]=right.u;d[6]=right.v;
    }
    return taxi_pair_row_batch(framebuffer,(unsigned)first,(unsigned)(last-first),descriptors,
        texture,texture_size,sampling.umask,sampling.vmask,sampling.ushift,sampling.vshift,work)?0:1;
}
int taxi_indexed_unshaded_polygon(uint8_t framebuffer[38400],const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,size_t texture_size,TaxiIndexedSampling sampling){return taxi_indexed_unshaded_polygon_work(framebuffer,vertices,count,texture,texture_size,sampling,0);}
int taxi_indexed_flat_rows_bounds(TaxiIndexedSpan spans[160],const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right){
    if(!spans||!vertices||count<3||count>8||left>=right||left < -1024||right>1024||(left&1)||(right&1))return 0;
    TaxiIndexedSpan prepared[160];memset(prepared,0,sizeof prepared);TaxiIndexedEdge rows[160][2];memset(rows,0,sizeof rows);int32_t first,last;
    if(!taxi_indexed_edges(rows,160,vertices,count,0,160,0,0,&first,&last))return 0;
    for(int32_t y=first;y<last;y++){int32_t a=rows[y][0].x,b=rows[y][1].x;
        if(a<left)a=left;if(b>right)b=right;a=signed_bits((uint32_t)a&~1u);
        int32_t width=signed_bits((uint32_t)b-(uint32_t)a);if(width<=0)continue;
        unsigned pairs=((unsigned)width+1)/2;if(a<left||(int64_t)a+pairs*2>right)return 0;
        prepared[y]=(TaxiIndexedSpan){a,pairs,0,0,0,0,0,0};
    }
    memcpy(spans,prepared,sizeof prepared);return 1;
}
int taxi_indexed_clipped_flat_polygon_work(uint8_t framebuffer[38400],const TaxiIndexedVertex *vertices,size_t count,uint16_t pattern,TaxiFlatRowWork *work){
    if(!framebuffer||!vertices||count<3||count>8)return 0;
    TaxiIndexedEdge rows[160][2];memset(rows,0,sizeof rows);int32_t first,last;
    if(!taxi_indexed_edges(rows,160,vertices,count,0,160,0,0,&first,&last))return 0;
    if(first>=last){if(work)*work=(TaxiFlatRowWork){0};return 1;}
    uint32_t descriptors[160*8]={0};
    for(int32_t y=first;y<last;y++){uint32_t *d=descriptors+(y-first)*8;d[0]=(uint32_t)rows[y][0].x;d[4]=(uint32_t)rows[y][1].x;}
    return taxi_flat_row_batch(framebuffer,(unsigned)first,(unsigned)(last-first),descriptors,0,240,pattern,work)?0:1;
}
int taxi_indexed_flat_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,uint16_t pattern,int clipped) {
    if(clipped)return taxi_indexed_clipped_flat_polygon_work(framebuffer,vertices,count,pattern,0);
    if(!framebuffer || !vertices || count<3 || count>8)return 0;
    if(!clipped)for(size_t i=0;i<count;++i)
        if(vertices[i].x<0 || vertices[i].x>240 || vertices[i].y<0 || vertices[i].y>=160)return 0;
    TaxiIndexedEdge rows[160][2];memset(rows,0,sizeof rows);
    int32_t first,last;
    if(!taxi_indexed_edges(rows,160,vertices,count,0,160,0,0,&first,&last))return 0;
    for(int32_t y=first;y<last;++y) {
        int32_t left=rows[y][0].x,right=rows[y][1].x;
        if(clipped) {if(left<0)left=0;if(right>240)right=240;}
        left=signed_bits((uint32_t)left&~1u);
        if(right<=left)continue;
        size_t pixels=((size_t)(right-left)+1)/2*2;
        if(left<0 || (size_t)left+pixels>240)return 0;
        /* Original repeats the halfword and ROR8 after every row;
           zero color bytes are opaque, unlike the texture families. */
        for(size_t x=0;x<pixels;++x)
            framebuffer[(size_t)y*240+(size_t)left+x]=(uint8_t)(pattern>>((((unsigned)y+(unsigned)x)&1)*8));
    }
    return 1;
}
int taxi_indexed_shaded_flat_rows_bounds(TaxiIndexedSpan spans[160],
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right,
    uint32_t far_depth,uint32_t shade_depth) {
    if(!spans||!vertices||left>=right||left < -1024||right>1024||(left&1)||(right&1))return 0;
    TaxiIndexedSpan prepared[160];memset(prepared,0,sizeof prepared);
    TaxiIndexedEdge rows[160][2];memset(rows,0,sizeof rows);int32_t first,last;
    if(!taxi_indexed_edges(rows,160,vertices,count,0,160,far_depth,shade_depth,&first,&last))return 0;
    for(int32_t y=first;y<last;y++) {
        TaxiIndexedEdge a=rows[y][0],b=rows[y][1];a.x=signed_bits((uint32_t)a.x&~1u);
        int32_t width=signed_bits((uint32_t)b.x-(uint32_t)a.x);if(width<=0)continue;
        uint32_t step=quotient(b.shade-a.shade,width);
        if(b.x<left||a.x>=right)continue;
        if(a.x<left){a.shade+=step*((uint32_t)left-(uint32_t)a.x);a.x=left;}
        if(b.x>right)b.x=right;
        size_t pairs=((size_t)(b.x-a.x)+1)/2;
        if((int64_t)a.x+(int64_t)pairs*2>right)return 0;
        prepared[y]=(TaxiIndexedSpan){a.x,pairs,0,0,a.shade,0,0,step*2};
    }
    memcpy(spans,prepared,sizeof prepared);return 1;
}
int taxi_indexed_shaded_flat_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,uint16_t pattern,
    const uint8_t lookup[2560],uint32_t far_depth,uint32_t shade_depth) {
    if(!framebuffer || !lookup)return 0;
    TaxiIndexedEdge rows[160][2];memset(rows,0,sizeof rows);
    int32_t first,last;
    if(!taxi_indexed_edges(rows,160,vertices,count,0,160,far_depth,
        shade_depth,&first,&last))return 0;
    for(int32_t y=first;y<last;++y) {
        TaxiIndexedEdge a=rows[y][0],b=rows[y][1];
        a.x=signed_bits((uint32_t)a.x&~1u);
        int32_t width=signed_bits((uint32_t)b.x-(uint32_t)a.x);
        if(width<=0)continue;
        uint32_t step=quotient(b.shade-a.shade,width);
        if(b.x<0 || a.x>=240)continue;
        if(a.x<0) {a.shade+=step*(0u-(uint32_t)a.x);a.x=0;}
        if(b.x>240)b.x=240;
        size_t pairs=((size_t)(b.x-a.x)+1)/2;
        if((size_t)a.x+pairs*2>240)return 0;
        uint8_t low=(uint8_t)pattern,high=(uint8_t)(pattern>>8);
        if(y&1) {uint8_t temp=low;low=high;high=temp;}
        for(size_t i=0;i<pairs;++i) {
            int32_t shade=signed_bits(a.shade);
            unsigned bank=shade<0?0:(unsigned)shade/4096;
            if(bank>9)bank=9;
            uint8_t *pixel=framebuffer+(size_t)y*240+(size_t)a.x+i*2;
            pixel[0]=lookup[bank*256+low];pixel[1]=lookup[bank*256+high];
            a.shade+=step*2;
        }
    }
    return 1;
}
int taxi_indexed_attribute_flat_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,const uint32_t *attributes,size_t count,
    const uint8_t patterns[16],const uint8_t *lookup,
    uint32_t far_depth,uint32_t shade_depth) {
    if(!framebuffer || !vertices || !attributes || !patterns || count<3 || count>8)return 0;
    TaxiIndexedVertex input[8];
    for(size_t i=0;i<count;++i) {input[i]=vertices[i];input[i].u=attributes[i];}
    TaxiIndexedEdge rows[160][2];memset(rows,0,sizeof rows);
    int32_t first,last;
    if(!taxi_indexed_edges(rows,160,input,count,0,160,far_depth,
        shade_depth,&first,&last))return 0;
    for(int32_t y=first;y<last;++y) {
        TaxiIndexedEdge a=rows[y][0],b=rows[y][1];
        a.x=signed_bits((uint32_t)a.x&~1u);
        int32_t width=signed_bits((uint32_t)b.x-(uint32_t)a.x);
        if(width<=0)continue;
        uint32_t da=quotient(b.u-a.u,width),ds=quotient(b.shade-a.shade,width);
        if(b.x<0 || a.x>=240)continue;
        if(a.x<0) {uint32_t advance=0u-(uint32_t)a.x;a.u+=da*advance;a.shade+=ds*advance;a.x=0;}
        if(b.x>240)b.x=240;
        size_t pairs=((size_t)(b.x-a.x)+1)/2;
        if((size_t)a.x+pairs*2>240)return 0;
        for(size_t i=0;i<pairs;++i) {
            int32_t attribute=signed_bits(arm_asr(a.u,24));
            unsigned index=attribute<0?0:attribute>7?7:(unsigned)attribute;
            uint8_t low=patterns[index*2],high=patterns[index*2+1];
            if(y&1) {uint8_t temp=low;low=high;high=temp;}
            if(lookup) {
                int32_t shade=signed_bits(a.shade);
                unsigned bank=shade<0?0:(unsigned)shade/4096;if(bank>9)bank=9;
                low=lookup[bank*256+low];high=lookup[bank*256+high];
            }
            uint8_t *pixel=framebuffer+(size_t)y*240+(size_t)a.x+i*2;
            pixel[0]=low;pixel[1]=high;a.u+=da*2;a.shade+=ds*2;
        }
    }
    return 1;
}
int taxi_indexed_perspective_columns_bounds(TaxiIndexedSpan *spans,size_t capacity,
    const TaxiIndexedVertex *vertices,size_t count,int32_t left,int32_t right) {
    if(!spans || !vertices || count<3 || count>8 || left>=right ||
       left < -1024 || right>1024 || (left&1) || (right&1))return 0;
    int32_t columns_count=(right-left)/2;
    if(capacity<(size_t)columns_count)return 0;
    TaxiIndexedVertex shifted[8];
    for(size_t i=0;i<count;i++) {
        shifted[i]=vertices[i];
        int64_t x=(int64_t)vertices[i].x-left;
        if(x<INT32_MIN||x>INT32_MAX)return 0;
        shifted[i].x=(int32_t)x;
    }
    vertices=shifted;int32_t viewport_width=right-left;
    TaxiIndexedSpan prepared[1024];memset(prepared,0,sizeof prepared);
    TaxiIndexedEdge columns[1024][2];memset(columns,0,sizeof columns);
    int32_t first=columns_count,last=0;
    for(size_t i=0;i<count;++i) {
        TaxiIndexedVertex a=vertices[i],b=vertices[(i+count-1)%count];
        if(!a.depth || !b.depth)return 0;
        if(a.x==b.x)continue;
        unsigned side=1;
        if(a.x>b.x) {TaxiIndexedVertex temp=a;a=b;b=temp;side=0;}
        if(a.x>=viewport_width || b.x<0)continue;
        int32_t width=signed_bits((uint32_t)b.x-(uint32_t)a.x);
        if(width<=0)return 0;
        uint32_t y=(uint32_t)a.y<<12;
        uint32_t dy=quotient(((uint32_t)b.y<<12)-y,width);
        uint32_t u=source_divide(a.u<<8,a.depth),v=source_divide(a.v<<8,a.depth);
        uint32_t z=source_divide(0x40000000,a.depth)<<6;
        uint32_t du=quotient(source_divide(b.u<<8,b.depth)-u,width);
        uint32_t dv=quotient(source_divide(b.v<<8,b.depth)-v,width);
        uint32_t dz=quotient((source_divide(0x40000000,b.depth)<<6)-z,width);
        int32_t start=a.x,end=b.x;
        if(start<0) {uint32_t advance=0u-(uint32_t)start;
            y+=dy*advance;u+=du*advance;v+=dv*advance;z+=dz*advance;start=0;}
        if(end>viewport_width)end=viewport_width;
        start/=2;end=(end+1)/2;
        if(start<first)first=start;if(end>last)last=end;
        int32_t iterations=end>start?end-start:1;
        for(int32_t n=0;n<iterations;++n) {
            if(start<0 || start>=columns_count)return 0;
            columns[start++][side]=(TaxiIndexedEdge){signed_bits(arm_asr(y,12)),u,v,z};
            y+=dy*2;u+=du*2;v+=dv*2;z+=dz*2;
        }
    }
    for(int32_t column=first;column<last;++column) {
        TaxiIndexedEdge a=columns[column][0],b=columns[column][1];
        const uint32_t descriptor[8]={(uint32_t)a.x,a.u,a.v,a.shade,(uint32_t)b.x,b.u,b.v,b.shade};
        TaxiPairColumnSetup setup;
        int status=taxi_pair_column_setup(descriptor,0,160,&setup);
        if(status<0)return 0;
        if(!status)continue;
        /* Source do-while draws once even at the accepted zero-row edge. */
        prepared[column]=(TaxiIndexedSpan){(int32_t)setup.row,setup.rows?setup.rows:1u,setup.x,setup.y,0,setup.dx,setup.dy,0};
    }
    memcpy(spans,prepared,(size_t)columns_count*sizeof *spans);
    return 1;
}
int taxi_indexed_perspective_columns_spans(TaxiIndexedSpan spans[120],
    const TaxiIndexedVertex *vertices,size_t count) {
    return taxi_indexed_perspective_columns_bounds(spans,120,vertices,count,0,240);
}
int taxi_indexed_perspective_columns_polygon_work(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling,TaxiPairColumnWork *work) {
    if(!framebuffer || !texture)return 0;
    TaxiIndexedSpan spans[120];
    if(!taxi_indexed_perspective_columns_spans(spans,vertices,count))return 0;
    uint8_t staged[38400];memcpy(staged,framebuffer,sizeof staged);
    TaxiPairColumnWork total={0};
    for(unsigned column=0;column<120;column++) {
        const TaxiIndexedSpan *span=spans+column;if(!span->pairs)continue;
        uint32_t u=span->u,v=span->v;TaxiPairColumnWork part;
        if(taxi_pair_column_raster_work(staged,(size_t)span->x*240+column*2,texture,texture_size,
            sampling.umask,sampling.vmask,sampling.ushift,sampling.vshift,&u,&v,span->du,span->dv,(unsigned)span->pairs,&part))return 0;
        total.texture_byte_reads+=part.texture_byte_reads;total.framebuffer_halfword_writes+=part.framebuffer_halfword_writes;
    }
    memcpy(framebuffer,staged,sizeof staged);if(work)*work=total;return 1;
}
int taxi_indexed_perspective_columns_polygon(uint8_t framebuffer[38400],
    const TaxiIndexedVertex *vertices,size_t count,const uint8_t *texture,
    size_t texture_size,TaxiIndexedSampling sampling) {
    return taxi_indexed_perspective_columns_polygon_work(framebuffer,vertices,count,texture,texture_size,sampling,0);
}
int taxi_indexed_billboard_bank(unsigned *out,uint32_t depth,uint32_t far_depth,uint32_t shade_depth) {
    if(!out)return 0;
    int32_t bank=signed_bits(2+source_divide((depth-(far_depth-shade_depth))<<3,shade_depth));
    if(bank<0||bank>9)return 0;*out=(unsigned)bank;return 1;
}
int taxi_indexed_billboard_rows_bounds(TaxiIndexedSpan spans[160],int32_t left,int32_t top,
    int32_t right,int32_t bottom,unsigned width,unsigned height,int32_t clip_left,int32_t clip_right,
    int clipped,unsigned bank) {
    if(!spans||!width||!height||width>128||height>128||bank>9||clip_left>=clip_right||
       clip_left < -1024||clip_right>1024||(clip_left&1)||(clip_right&1))return 0;
    int32_t dx=signed_bits((uint32_t)right-(uint32_t)left),dy=signed_bits((uint32_t)bottom-(uint32_t)top);
    if(dx<=0||dy<=0)return 0;
    uint32_t du=quotient(width<<14,dx),dv=quotient(height<<14,dy),u=0,v=0;
    /* ARM4A94 retains divider R2 as U; divisor1 preserves prior R2.
       A 1x1 extent retains the descriptor address and is not qualified here. */
    if(!clipped)u=dy>1?dv:(dx>1?du:0);
    if(clipped) {
        if(left<=clip_left){u=du*((uint32_t)clip_left-(uint32_t)left);left=clip_left;}
        if(top<=0){v=dv*(0u-(uint32_t)top);top=0;}
        if(right>clip_right)right=clip_right;if(bottom>160)bottom=160;
    }
    left=signed_bits((uint32_t)left&~1u);right=signed_bits((uint32_t)right&~1u);
    if(left<clip_left||right>clip_right||top<0||bottom>160||right<=left||bottom<=top)return 0;
    TaxiIndexedSpan prepared[160];memset(prepared,0,sizeof prepared);
    for(int32_t y=top;y<bottom;y++){prepared[y]=(TaxiIndexedSpan){left,(size_t)(right-left)/2,u,v,bank*4096,du*2,0,0};v+=dv;}
    memcpy(spans,prepared,sizeof prepared);return 1;
}
static int indexed_billboard(uint8_t framebuffer[38400],int32_t left,
    int32_t top,int32_t right,int32_t bottom,const uint8_t *texture,
    size_t texture_size,unsigned width,unsigned height,unsigned log_width,int clipped,
    const uint8_t *bank) {
    TaxiIndexedSampling sampling;
    if(!framebuffer || !texture || !taxi_indexed_sampling(&sampling,width,height,log_width,0))return 0;
    int32_t dx=signed_bits((uint32_t)right-(uint32_t)left);
    int32_t dy=signed_bits((uint32_t)bottom-(uint32_t)top);
    if(dx<=0 || dy<=0)return 0;
    uint32_t du=quotient(width<<14,dx),dv=quotient(height<<14,dy),u=0,v=0;
    /* ARM4A94 retains divider R2 as U; divisor1 preserves prior R2.
       A 1x1 extent retains the descriptor address and is not qualified here. */
    if(!clipped)u=dy>1?dv:(dx>1?du:0);
    if(clipped) {
        if(left<=0) {u=du*(0u-(uint32_t)left);left=0;}
        if(top<=0) {v=dv*(0u-(uint32_t)top);top=0;}
        if(right>240)right=240;if(bottom>160)bottom=160;
    }
    left=signed_bits((uint32_t)left&~1u);right=signed_bits((uint32_t)right&~1u);
    if(left<0 || right>240 || top<0 || bottom>160 || right<=left || bottom<=top)return 0;
    uint8_t identity[2560];for(unsigned i=0;i<2560;++i)identity[i]=(uint8_t)i;
    for(int32_t y=top;y<bottom;++y) {
        if(bank) {
            uint32_t row_u=u;
            for(int32_t x=left;x<right;x+=2) {
                uint32_t index=(arm_asr(row_u,sampling.ushift)&sampling.umask)+
                    (arm_asr(v,sampling.vshift)&sampling.vmask);
                if(index>=texture_size)return 0;
                uint8_t texel=texture[index];
                if(texel) {
                    /* ARM4d1c aligned-word ROR load, then OR(value,value<<8).
                       Only the low halfword is stored. */
                    unsigned next=(texel&~3u)|((texel+1u)&3u);
                    uint8_t *pixel=framebuffer+(size_t)y*240+(size_t)x;
                    pixel[0]=bank[texel];pixel[1]=(uint8_t)(bank[texel]|bank[next]);
                }
                row_u+=du*2;
            }
            v+=dv;continue;
        }
        if(!taxi_indexed_keyed_span(framebuffer+(size_t)y*240+(size_t)left,
            (size_t)(right-left)/2,texture,texture_size,identity,u,v,0,du*2,0,0,
            sampling.umask,sampling.vmask,sampling.ushift,sampling.vshift))return 0;
        v+=dv;
    }
    return 1;
}
int taxi_indexed_billboard_work(uint8_t framebuffer[38400],int32_t left,int32_t top,int32_t right,int32_t bottom,const uint8_t *texture,size_t texture_size,unsigned width,unsigned height,unsigned log_width,int clipped,TaxiPairColumnWork *work){
    TaxiIndexedSampling sampling;TaxiIndexedSpan spans[160];
    if(!framebuffer||!texture||!taxi_indexed_sampling(&sampling,width,height,log_width,0)||!taxi_indexed_billboard_rows_bounds(spans,left,top,right,bottom,width,height,0,240,clipped,0))return 0;
    uint8_t staged[38400];memcpy(staged,framebuffer,sizeof staged);TaxiPairColumnWork total={0};
    for(unsigned y=0;y<160;y++){const TaxiIndexedSpan *span=spans+y;if(!span->pairs)continue;uint32_t u=span->u,v=span->v;TaxiPairColumnWork part;
        if(taxi_pair_row_raster(staged,y*240+(unsigned)span->x,texture,texture_size,sampling.umask,sampling.vmask,sampling.ushift,sampling.vshift,&u,&v,span->du,span->dv,(unsigned)span->pairs,&part))return 0;
        total.texture_byte_reads+=part.texture_byte_reads;total.framebuffer_halfword_writes+=part.framebuffer_halfword_writes;
    }
    memcpy(framebuffer,staged,sizeof staged);if(work)*work=total;return 1;
}
int taxi_indexed_billboard(uint8_t framebuffer[38400],int32_t left,
    int32_t top,int32_t right,int32_t bottom,const uint8_t *texture,
    size_t texture_size,unsigned width,unsigned height,unsigned log_width,int clipped) {
    return taxi_indexed_billboard_work(framebuffer,left,top,right,bottom,texture,
        texture_size,width,height,log_width,clipped,0);
}
int taxi_indexed_shaded_billboard(uint8_t framebuffer[38400],int32_t left,
    int32_t top,int32_t right,int32_t bottom,const uint8_t *texture,
    size_t texture_size,unsigned width,unsigned height,unsigned log_width,
    const uint8_t lookup[2560],uint32_t depth,uint32_t far_depth,uint32_t shade_depth) {
    if(!lookup)return 0;
    uint32_t value=2+source_divide((depth-(far_depth-shade_depth))<<3,shade_depth);
    int32_t bank=signed_bits(value);
    /* Source has no clamp; reject missing lookup memory on the host. */
    if(bank<0 || bank>9)return 0;
    return indexed_billboard(framebuffer,left,top,right,bottom,texture,
        texture_size,width,height,log_width,1,lookup+(unsigned)bank*256);
}
int taxi_indexed_keyed_pair(uint8_t output[2],uint8_t texel,int32_t shade,
    const uint8_t lookup[2560]) {
    if(!texel)return 0;
    /* Negative ASR values clamp0; positive division equals originalASR12. */
    unsigned bank=shade<0?0:(unsigned)shade/4096;
    if(bank>9)bank=9;
    uint8_t color=lookup[bank*256+texel];
    output[0]=color;output[1]=color;return 1;
}

uint32_t taxi_indexed_textured_billboard_callback(uint16_t flags) {
    if(!(flags&8))return 0;
    if(flags&1)return 0x0300691c;
    if(flags&2)return 0x030069b0;
    if(flags&0x200)return 0x03006a38;
    return (flags&4)?0x030069a4:0x03006a28;
}

uint16_t taxi_indexed_billboard_packet_flags(int32_t angle,uint32_t caller_flags,int side_clipped) {
    uint16_t flags=(angle || (caller_flags&3))?0x20:8;
    if(caller_flags&8)flags|=0x200;
    if(side_clipped)flags|=4;
    return flags;
}
