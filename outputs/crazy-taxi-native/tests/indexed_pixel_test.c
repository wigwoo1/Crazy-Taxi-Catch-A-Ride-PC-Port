#include "../src/indexed_pixel.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    for(unsigned flags=0;flags<65536;flags++) {
        uint32_t expected=0;
        if(flags&8)expected=(flags&1)?0x0300691c:(flags&2)?0x030069b0:
            (flags&0x200)?0x03006a38:(flags&4)?0x030069a4:0x03006a28;
        assert(taxi_indexed_textured_billboard_callback((uint16_t)flags)==expected);
    }
    assert(taxi_indexed_billboard_packet_flags(0,0,0)==8);
    assert(taxi_indexed_billboard_packet_flags(0,8,1)==0x20c);
    assert(taxi_indexed_billboard_packet_flags(0,1,0)==0x20);
    assert(taxi_indexed_billboard_packet_flags(0,2,0)==0x20);
    assert(taxi_indexed_billboard_packet_flags(-1,8,1)==0x224);
    assert(taxi_indexed_billboard_packet_flags(0,0xfffffff4,0)==8);
    /* Flip is UV geometry, not dispatcher bit1/bit2: full-quad owner. */
    assert(!taxi_indexed_textured_billboard_callback(taxi_indexed_billboard_packet_flags(0,1,0)));
    assert(taxi_indexed_textured_billboard_callback(taxi_indexed_billboard_packet_flags(0,8,1))==0x03006a38);
    uint8_t lookup[2560];
    for(unsigned i=0;i<2560;i++)lookup[i]=(uint8_t)(i/256*19+i%256);
    for(int bank=-2;bank<12;bank++)for(int fraction=0;fraction<4096;fraction+=4095)
        for(unsigned texel=0;texel<256;texel++) {
            uint8_t pixels[4]={17,33,77,99};
            assert(taxi_indexed_keyed_pair(pixels+1,(uint8_t)texel,bank*4096+fraction,lookup)==(texel!=0));
            unsigned clamped=bank<0?0:bank>9?9:(unsigned)bank;
            if(texel)assert(pixels[1]==lookup[clamped*256+texel] && pixels[1]==pixels[2]);
            else assert(pixels[1]==33 && pixels[2]==77);
            assert(pixels[0]==17 && pixels[3]==99);
        }
    uint8_t texture[16], span[34];
    TaxiIndexedEdge rows[8][2];
    TaxiIndexedVertex polygon[4]={{2,0,0,0,100},{10,0,800,0,100},
        {10,6,800,600,100},{2,6,0,600,100}};
    int32_t first,last;
    assert(taxi_indexed_edges(rows,8,polygon,4,1,5,100,100,&first,&last));
    assert(first==1 && last==5);
    for (int y=1;y<5;++y) {
        assert(rows[y][0].x==2 && rows[y][1].x==10);
        assert(rows[y][0].u==0 && rows[y][1].u==800);
        assert(rows[y][0].v==(unsigned)y*100 && rows[y][1].v==(unsigned)y*100);
        assert(rows[y][0].shade==36864 && rows[y][1].shade==36864);
    }
    TaxiIndexedSampling sampling;
    for (unsigned lw=0;lw<=7;++lw) for (unsigned lh=0;lh<=7;++lh)
        for (unsigned level=0;level<=lw && level<=lh;++level) {
            assert(taxi_indexed_sampling(&sampling,1u<<lw,1u<<lh,lw,level));
            unsigned w=1u<<(lw-level),h=1u<<(lh-level);
            for (unsigned y=0;y<h;++y) for (unsigned x=0;x<w;++x) {
                uint32_t u=x<<(14+level),v=y<<(14+level);
                unsigned index=((u>>sampling.ushift)&sampling.umask)+
                    ((v>>sampling.vshift)&sampling.vmask);
                assert(index==y*w+x);
            }
        }
    assert(!taxi_indexed_sampling(&sampling,3,4,1,0));
    assert(!taxi_indexed_sampling(&sampling,4,4,2,3));
    uint8_t framebuffer[38400];
    for (unsigned i=0;i<38400;++i) framebuffer[i]=113;
    assert(taxi_indexed_sampling(&sampling,4,4,2,0));
    for (unsigned i=0;i<16;++i) texture[i]=(uint8_t)i;
    TaxiIndexedVertex textured[4]={{2,2,0,0,100},{10,2,4u<<14,0,100},
        {10,6,4u<<14,4u<<14,100},{2,6,0,4u<<14,100}};
    assert(taxi_indexed_polygon(framebuffer,textured,4,texture,16,sampling,
        lookup,0,0,240,160,100,100));
    for (unsigned y=0;y<160;++y) for (unsigned x=0;x<240;++x) {
        unsigned expected=113;
        if (y>=2 && y<6 && x>=2 && x<10) {
            unsigned texel=(y-2)*4+(x-2)/2;
            if (texel) expected=lookup[9*256+texel];
        }
        assert(framebuffer[y*240+x]==expected);
    }
    TaxiIndexedSpan prepared;
    for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
    TaxiIndexedVertex odd_rect[4]={{1,2,0,0,0},{9,2,9u<<14,0,0},
        {9,4,9u<<14,0,0},{1,4,0,0,0}};
    assert(taxi_indexed_sampling(&sampling,16,1,4,0));
    assert(taxi_indexed_unshaded_polygon(framebuffer,odd_rect,4,texture,16,sampling));
    for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
        unsigned expected=113;
        if(y>=2 && y<4 && x<10) { unsigned texel=(x/2)*2;if(texel)expected=texel; }
        assert(framebuffer[y*240+x]==expected);
    }
    for (unsigned i=0;i<38400;++i) framebuffer[i]=113;
    uint8_t identity[2560];
    for(unsigned i=0;i<2560;++i)identity[i]=(uint8_t)i;
    TaxiIndexedVertex clipped_rect[4]={{-3,-2,0,0,0},{9,-2,12u<<14,0,0},
        {9,4,12u<<14,0,0},{-3,4,0,0,0}};
    assert(taxi_indexed_polygon(framebuffer,clipped_rect,4,texture,16,sampling,
        identity,0,0,240,160,0,0));
    for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
        unsigned expected=113;
        if(y<4 && x<10)expected=3+(x/2)*2;
        assert(framebuffer[y*240+x]==expected);
    }
    for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
    const uint8_t solid[1]={1};
    TaxiIndexedVertex triangle[3]={{2,2,0,0,100},{10,2,0,0,100},{2,6,0,0,100}};
    assert(taxi_indexed_sampling(&sampling,1,1,0,0));
    assert(taxi_indexed_polygon(framebuffer,triangle,3,solid,1,sampling,
        lookup,0,0,240,160,100,100));
    for (unsigned y=0;y<160;++y) for (unsigned x=0;x<240;++x) {
        unsigned inside=y>=2 && y<6 && x>=2 && x<10-2*(y-2);
        assert(framebuffer[y*240+x]==(inside?lookup[2305]:113));
    }
    assert(taxi_indexed_span_prepare(&prepared,
        (TaxiIndexedEdge){1,100,500,800},(TaxiIndexedEdge){9,180,420,1600},0,8));
    assert(prepared.x==0 && prepared.pairs==4 && prepared.u==100 && prepared.v==500);
    assert(prepared.du==20 && prepared.dv==(uint32_t)-20 && prepared.dshade==200);
    assert(taxi_indexed_span_prepare(&prepared,
        (TaxiIndexedEdge){-2,100,500,800},(TaxiIndexedEdge){6,180,420,1600},1,6));
    assert(prepared.x==0 && prepared.pairs==3 && prepared.u==130 && prepared.v==470);
    prepared.x=99;
    assert(!taxi_indexed_span_prepare(&prepared,
        (TaxiIndexedEdge){6,0,0,0},(TaxiIndexedEdge){6,0,0,0},0,8));
    assert(prepared.x==99);
    for (unsigned i=0;i<16;++i) texture[i]=(uint8_t)i;
    for (unsigned start=0;start<16;++start) {
        for (unsigned i=0;i<34;++i) span[i]=113;
        assert(taxi_indexed_keyed_span(span+1,16,texture,16,lookup,
            start<<12,0,0,4096,0,4096,15,0,12,12));
        for (unsigned i=0;i<16;++i) {
            unsigned texel=(start+i)%16,bank=i>9?9:i;
            unsigned expected=texel?lookup[bank*256+texel]:113;
            assert(span[1+2*i]==expected && span[2+2*i]==expected);
        }
        assert(span[0]==113 && span[33]==113);
    }
    /* Negative coordinate wrap, negative shade and advancing across UINT32. */
    assert(taxi_indexed_keyed_span(span+1,2,texture,16,lookup,
        UINT32_MAX-4095,0,UINT32_MAX-4095,4096,0,4096,15,0,12,32));
    assert(span[1]==lookup[15] && span[2]==lookup[15]);
    span[1]=51; span[2]=52;
    assert(!taxi_indexed_keyed_span(span+1,2,texture,16,lookup,
        15,0,0,1,0,0,31,0,0,0));
    assert(span[1]==51 && span[2]==52);
    TaxiIndexedVertex flat_rect[4]={{3,1,0,0,0},{9,1,0,0,0},
        {9,4,0,0,0},{3,4,0,0,0}};
    for(unsigned mode=0;mode<2;++mode)for(unsigned byte=0;byte<256;++byte) {
        for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
        assert(taxi_indexed_flat_polygon(framebuffer,flat_rect,4,(uint16_t)(byte<<8),mode));
        for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
            unsigned expected=113;
            if(y>=1 && y<4 && x>=2 && x<10)expected=((x+y)&1)?byte:0;
            assert(framebuffer[y*240+x]==expected);
        }
    }
    for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
    assert(taxi_indexed_flat_polygon(framebuffer,clipped_rect,4,0x0703,1));
    for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
        unsigned expected=113;
        if(y<4 && x<10)expected=((x+y)&1)?7:3;
        assert(framebuffer[y*240+x]==expected);
    }
    TaxiIndexedVertex shade_rect[4]={{3,1,0,0,0},{11,1,0,0,100},
        {11,3,0,0,100},{3,3,0,0,0}};
    for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
    assert(taxi_indexed_shaded_flat_polygon(framebuffer,shade_rect,4,0x0700,
        lookup,100,100));
    /* Width9 after left alignment; truncation3640 precedes doubling.
       Both bytes including zero are shaded and row parity swaps them. */
    for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
        unsigned expected=113;
        if(y>=1 && y<3 && x>=2 && x<12) {
            unsigned bank=(4096+3640*2*((x-2)/2))/4096;
            unsigned color=((x+y)&1)?7:0;
            expected=lookup[bank*256+color];
        }
        assert(framebuffer[y*240+x]==expected);
    }
    uint8_t patterns[16];for(unsigned i=0;i<16;++i)patterns[i]=(uint8_t)(i*11);
    uint32_t attributes[4]={0xfe000000,0x0a000000,0x0a000000,0xfe000000};
    const unsigned indices[5]={0,0,3,5,7};
    for(unsigned shaded_mode=0;shaded_mode<2;++shaded_mode) {
        for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
        assert(taxi_indexed_attribute_flat_polygon(framebuffer,shade_rect,
            attributes,4,patterns,shaded_mode?lookup:0,100,100));
        for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
            unsigned expected=113;
            if(y>=1 && y<3 && x>=2 && x<12) {
                unsigned pair=(x-2)/2;
                unsigned color=patterns[indices[pair]*2+((x+y)&1)];
                unsigned bank=(4096+3640*2*pair)/4096;
                expected=shaded_mode?lookup[bank*256+color]:color;
            }
            assert(framebuffer[y*240+x]==expected);
        }
    }
    TaxiIndexedVertex six_rect[6]={{2,1,0,0,0},{6,1,0,0,0},{10,1,0,0,0},
        {10,4,0,0,0},{6,4,0,0,0},{2,4,0,0,0}};
    for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
    assert(taxi_indexed_flat_polygon(framebuffer,six_rect,6,0x0703,1));
    for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
        unsigned expected=113;
        if(y>=1 && y<4 && x>=2 && x<10)expected=((x+y)&1)?7:3;
        assert(framebuffer[y*240+x]==expected);
    }
    TaxiIndexedVertex perspective_rect[4]={{3,1,0,0,1024},{11,1,8u<<14,0,2048},
        {11,3,8u<<14,0,2048},{3,3,0,0,1024}};
    TaxiIndexedSpan perspective_spans[160];
    memset(perspective_spans,0xa5,sizeof perspective_spans);
    assert(taxi_indexed_perspective_rows_spans(perspective_spans,perspective_rect,4));
    for(unsigned y=0;y<160;y++) {
        const TaxiIndexedSpan *span=perspective_spans+y;
        if(y==1||y==2) {
            assert(span->x==2 && span->pairs==4 && span->u==0 && span->v==0);
            assert(span->du==2u*16384 && span->dv==0 && span->shade==0 && span->dshade==0);
        }else assert(span->pairs==0);
    }
    TaxiIndexedSpan retained_spans[160];memcpy(retained_spans,perspective_spans,sizeof retained_spans);
    TaxiIndexedVertex invalid_depth[4];memcpy(invalid_depth,perspective_rect,sizeof invalid_depth);
    invalid_depth[2].depth=0;
    assert(!taxi_indexed_perspective_rows_spans(perspective_spans,invalid_depth,4));
    assert(!memcmp(retained_spans,perspective_spans,sizeof retained_spans));
    assert(taxi_indexed_sampling(&sampling,16,1,4,0));
    for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
    assert(taxi_indexed_perspective_rows_polygon(framebuffer,perspective_rect,
        4,texture,16,sampling));
    for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
        unsigned expected=113;
        if(y>=1 && y<3 && x>=2 && x<10) {
            unsigned texel=((x-2)/2)*2;if(texel)expected=texel;
        }
        assert(framebuffer[y*240+x]==expected);
    }
    TaxiIndexedVertex columns_rect[4]={{3,1,0,0,1024},{11,1,0,0,1024},
        {11,3,8u<<14,0,2048},{3,3,8u<<14,0,2048}};
    TaxiIndexedSpan column_spans[120];
    assert(taxi_indexed_perspective_columns_spans(column_spans,columns_rect,4));
    for(unsigned x=0;x<120;x++) {
        const TaxiIndexedSpan *span=column_spans+x;
        if(x>=1&&x<6) {
            assert(span->x==1&&span->pairs==4&&span->u==0&&span->v==0);
            assert(span->du==32768&&span->dv==0&&span->shade==0&&span->dshade==0);
        }else assert(span->pairs==0);
    }
    TaxiIndexedSpan retained_columns[120];memcpy(retained_columns,column_spans,sizeof retained_columns);
    memcpy(invalid_depth,columns_rect,sizeof invalid_depth);invalid_depth[1].depth=0;
    assert(!taxi_indexed_perspective_columns_spans(column_spans,invalid_depth,4));
    assert(!memcmp(retained_columns,column_spans,sizeof retained_columns));
    for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
    assert(taxi_indexed_perspective_columns_polygon(framebuffer,columns_rect,
        4,texture,16,sampling));
    /* Source rounds column interval outward and extends bottom by2 rows. */
    for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
        unsigned expected=113;
        if(y>=1 && y<5 && x>=2 && x<12) {
            unsigned texel=(y-1)*2;if(texel)expected=texel;
        }
        assert(framebuffer[y*240+x]==expected);
    }
    /* Endpoint depths1024/2048, far2048 and shade1024 yield source banks1/9.
       Clipping -3 to0 advances shade three pixels; alignment has no extra step. */
    TaxiIndexedVertex shaded_rectangle[4]={{-3,1,0,0,1024},{5,1,8u<<14,0,2048},
        {5,3,8u<<14,0,2048},{-3,3,0,0,1024}};
    TaxiIndexedSpan shade_spans[160];
    assert(taxi_indexed_shaded_rows_bounds(shade_spans,shaded_rectangle,4,0,240,2048,1024));
    for(unsigned y=0;y<160;y++) {
        const TaxiIndexedSpan *span=shade_spans+y;
        if(y==1||y==2) {
            assert(span->x==0&&span->pairs==3&&span->u==3u*16384&&span->du==32768);
            assert(span->shade==4u*4096&&span->dshade==2u*4096);
        }else assert(span->pairs==0);
    }
    uint8_t planned_shaded[38400];memset(planned_shaded,113,sizeof planned_shaded);
    assert(taxi_indexed_sampling(&sampling,16,1,4,0));
    for(unsigned y=0;y<160;y++)if(shade_spans[y].pairs) {
        const TaxiIndexedSpan *span=shade_spans+y;
        assert(taxi_indexed_keyed_span(planned_shaded+y*240+span->x,span->pairs,texture,16,lookup,
            span->u,span->v,span->shade,span->du,span->dv,span->dshade,
            sampling.umask,sampling.vmask,sampling.ushift,sampling.vshift));
    }
    memset(framebuffer,113,sizeof framebuffer);
    assert(taxi_indexed_polygon(framebuffer,shaded_rectangle,4,texture,16,sampling,lookup,
        0,0,240,160,2048,1024));
    assert(!memcmp(framebuffer,planned_shaded,sizeof framebuffer));
    assert(taxi_indexed_shaded_rows_bounds(shade_spans,shaded_rectangle,4,-24,264,2048,1024));
    assert(shade_spans[1].x==-4&&shade_spans[1].pairs==5&&shade_spans[1].shade==4096);
    assert(taxi_indexed_shaded_rows_bounds(shade_spans,shaded_rectangle,4,0,240,2048,0));
    /* Source divide-by-zero retains dividend, rather than silently resetting shade. */
    assert(shade_spans[1].shade!=0);
    assert(taxi_indexed_shaded_flat_rows_bounds(shade_spans,shaded_rectangle,4,0,240,2048,1024));
    assert(shade_spans[1].x==0&&shade_spans[1].pairs==3&&shade_spans[1].shade==18656&&shade_spans[1].dshade==7280);
    memset(planned_shaded,113,sizeof planned_shaded);
    for(unsigned y=0;y<160;y++) {
        const TaxiIndexedSpan *span=shade_spans+y;uint32_t shade=span->shade;
        for(size_t k=0;k<span->pairs;k++) {
            unsigned bank=shade/4096;if(bank>9)bank=9;
            planned_shaded[y*240+span->x+k*2]=lookup[bank*256+((y&1)?255:0)];
            planned_shaded[y*240+span->x+k*2+1]=lookup[bank*256+((y&1)?0:255)];
            shade+=span->dshade;
        }
    }
    memset(framebuffer,113,sizeof framebuffer);
    assert(taxi_indexed_shaded_flat_polygon(framebuffer,shaded_rectangle,4,0xff00,lookup,2048,1024));
    assert(!memcmp(framebuffer,planned_shaded,sizeof framebuffer));
    TaxiIndexedSpan affine_clipped[160],affine_unclipped[160];
    assert(taxi_indexed_affine_rows_bounds(affine_clipped,perspective_rect,4,0,240,1));
    assert(taxi_indexed_affine_rows_bounds(affine_unclipped,perspective_rect,4,0,240,0));
    for(unsigned y=0;y<160;y++) {
        if(y==1||y==2) {
            assert(affine_clipped[y].x==2&&affine_clipped[y].pairs==5&&affine_clipped[y].du==32768);
            assert(affine_unclipped[y].x==2&&affine_unclipped[y].pairs==5&&affine_unclipped[y].du==29126);
            assert(affine_clipped[y].u==0&&affine_unclipped[y].u==0);
        }else assert(!affine_clipped[y].pairs&&!affine_unclipped[y].pairs);
    }
    TaxiIndexedVertex clipped_left[4];memcpy(clipped_left,perspective_rect,sizeof clipped_left);
    for(unsigned i=0;i<4;i++)clipped_left[i].x-=6;
    assert(taxi_indexed_affine_rows_bounds(affine_clipped,clipped_left,4,0,240,1));
    assert(affine_clipped[1].x==0&&affine_clipped[1].pairs==3&&affine_clipped[1].u==3u*16384);
    TaxiIndexedSpan retained_affine[160];memcpy(retained_affine,affine_unclipped,sizeof retained_affine);
    assert(!taxi_indexed_affine_rows_bounds(affine_unclipped,clipped_left,4,0,240,0));
    assert(!memcmp(retained_affine,affine_unclipped,sizeof retained_affine));
    unsigned special_bank=99;
    assert(taxi_indexed_billboard_bank(&special_bank,(uint32_t)-2,8,8)&&special_bank==0);
    assert(taxi_indexed_billboard_bank(&special_bank,7,8,8)&&special_bank==9);
    assert(!taxi_indexed_billboard_bank(&special_bank,8,8,8)&&special_bank==9);
    assert(!taxi_indexed_billboard_bank(&special_bank,(uint32_t)-3,8,8)&&special_bank==9);
    assert(taxi_indexed_billboard_bank(&special_bank,8,8,0)&&special_bank==2);
    TaxiIndexedSpan special_spans[160];
    assert(taxi_indexed_billboard_rows_bounds(special_spans,-3,-1,5,3,16,4,0,240,1,3));
    assert(special_spans[0].x==0&&special_spans[0].pairs==2&&special_spans[0].u==98304&&special_spans[0].v==16384&&special_spans[0].du==65536&&special_spans[0].shade==12288);
    uint8_t special_texture[64];for(unsigned i=0;i<64;i++)special_texture[i]=(uint8_t)i;
    memset(planned_shaded,113,sizeof planned_shaded);
    for(unsigned y=0;y<160;y++) {
        const TaxiIndexedSpan *span=special_spans+y;uint32_t u=span->u;
        for(size_t k=0;k<span->pairs;k++) {
            unsigned texel=special_texture[((span->v>>14)&3)*16+((u>>14)&15)];
            if(texel){unsigned next=(texel&~3u)|((texel+1)&3u);
                planned_shaded[y*240+span->x+k*2]=lookup[3*256+texel];
                planned_shaded[y*240+span->x+k*2+1]=(uint8_t)(lookup[3*256+texel]|lookup[3*256+next]);}
            u+=span->du;
        }
    }
    memset(framebuffer,113,sizeof framebuffer);
    assert(taxi_indexed_shaded_billboard(framebuffer,-3,-1,5,3,special_texture,64,16,4,4,lookup,1,8,8));
    assert(!memcmp(framebuffer,planned_shaded,sizeof framebuffer));
    /* Same source UV/depth geometry translated into the widescreen left margin. */
    TaxiIndexedVertex wide_rows[4],wide_columns[4];
    memcpy(wide_rows,perspective_rect,sizeof wide_rows);memcpy(wide_columns,columns_rect,sizeof wide_columns);
    for(unsigned i=0;i<4;i++){wide_rows[i].x-=24;wide_columns[i].x-=24;}
    TaxiIndexedSpan wide_row_spans[160],wide_column_spans[145];
    assert(taxi_indexed_perspective_rows_bounds(wide_row_spans,wide_rows,4,-24,264));
    for(unsigned y=0;y<160;y++) {
        const TaxiIndexedSpan *span=wide_row_spans+y;
        if(y==1||y==2)assert(span->x==-22&&span->pairs==4&&span->du==32768&&span->u==0);
        else assert(span->pairs==0);
    }
    memset(wide_column_spans,0xa5,sizeof wide_column_spans);
    TaxiIndexedSpan tail_guard=wide_column_spans[144];
    assert(taxi_indexed_perspective_columns_bounds(wide_column_spans,144,wide_columns,4,-24,264));
    assert(!memcmp(&tail_guard,wide_column_spans+144,sizeof tail_guard));
    for(unsigned x=0;x<144;x++) {
        const TaxiIndexedSpan *span=wide_column_spans+x;
        if(x>=1&&x<6)assert(span->x==1&&span->pairs==4&&span->du==32768&&span->u==0);
        else assert(span->pairs==0);
    }
    TaxiIndexedSpan wide_retained[145];memcpy(wide_retained,wide_column_spans,sizeof wide_retained);
    assert(!taxi_indexed_perspective_columns_bounds(wide_column_spans,143,wide_columns,4,-24,264));
    assert(!taxi_indexed_perspective_columns_bounds(wide_column_spans,144,wide_columns,4,-23,264));
    assert(!memcmp(wide_retained,wide_column_spans,sizeof wide_retained));
    TaxiIndexedSpan wide_rows_retained[160];memcpy(wide_rows_retained,wide_row_spans,sizeof wide_rows_retained);
    assert(!taxi_indexed_perspective_rows_bounds(wide_row_spans,wide_rows,4,-24,1026));
    assert(!memcmp(wide_rows_retained,wide_row_spans,sizeof wide_rows_retained));
    for(unsigned mode=0;mode<2;++mode) {
        for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
        assert(taxi_indexed_billboard(framebuffer,3,2,11,4,texture,16,16,1,4,mode));
        for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
            unsigned expected=113;
            if(y>=2 && y<4 && x>=2 && x<10) {
                unsigned texel=((x-2)/2)*4;if(texel)expected=texel;
            }
            assert(framebuffer[y*240+x]==expected);
        }
    }
    for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
    assert(taxi_indexed_billboard(framebuffer,-2,-1,6,1,texture,16,16,1,4,1));
    for(unsigned y=0;y<160;++y)for(unsigned x=0;x<240;++x) {
        unsigned expected=113;if(y==0 && x<6)expected=4+(x/2)*4;
        assert(framebuffer[y*240+x]==expected);
    }
    for(unsigned texel=0;texel<256;++texel) {
        uint8_t single=(uint8_t)texel;
        for(unsigned i=0;i<38400;++i)framebuffer[i]=113;
        assert(taxi_indexed_shaded_billboard(framebuffer,0,0,2,1,&single,
            1,1,1,0,lookup,40,100,100));
        unsigned next=(texel/4)*4+(texel+1)%4;
        assert(framebuffer[0]==(texel?lookup[5*256+texel]:113));
        assert(framebuffer[1]==(texel?(uint8_t)(lookup[5*256+texel]|lookup[5*256+next]):113));
        for(unsigned i=2;i<38400;++i)assert(framebuffer[i]==113);
    }
    /* A keyed source zero retains the previous BG index. A nonzero source
       may WRITE mapped zero, and the source OR pair can independently retain
       a nonzero second pixel. Coverage must follow these resolved bytes. */
    {
        uint8_t coverage_lookup[2560]={0};
        coverage_lookup[5*256+2]=7;
        uint8_t raw=1;
        memset(framebuffer,113,sizeof framebuffer);
        assert(taxi_indexed_shaded_billboard(framebuffer,0,0,2,1,&raw,
            1,1,1,0,coverage_lookup,40,100,100));
        assert(framebuffer[0]==0 && framebuffer[1]==7);
        raw=0;
        assert(taxi_indexed_shaded_billboard(framebuffer,0,0,2,1,&raw,
            1,1,1,0,coverage_lookup,40,100,100));
        assert(framebuffer[0]==0 && framebuffer[1]==7);
        raw=3; /* Rotation wraps within aligned four-byte word to index0. */
        coverage_lookup[5*256]=9;
        assert(taxi_indexed_shaded_billboard(framebuffer,0,0,2,1,&raw,
            1,1,1,0,coverage_lookup,40,100,100));
        assert(framebuffer[0]==0 && framebuffer[1]==9);
        for(unsigned i=2;i<38400;i++)assert(framebuffer[i]==113);
    }
    /* Source1304..1410 do-while edge: polygon lower endpoint +2 equals top. */
    TaxiIndexedVertex top_edge[4]={{0,-10,0,0,4096},{8,-10,0,0,4096},{8,-2,0,0,4096},{0,-2,0,0,4096}};
    TaxiIndexedSpan edge_spans[120];
    assert(taxi_indexed_perspective_columns_spans(edge_spans,top_edge,4));
    for(unsigned i=0;i<4;i++)assert(edge_spans[i].x==0&&edge_spans[i].pairs==1);
    uint8_t edge_texture[4096];memset(edge_texture,77,sizeof edge_texture);
    memset(framebuffer,113,sizeof framebuffer);
    TaxiIndexedSampling edge_sampling={63,4032,14,8};
    assert(taxi_indexed_perspective_columns_polygon(framebuffer,top_edge,4,edge_texture,sizeof edge_texture,edge_sampling));
    for(unsigned i=0;i<38400;i++)assert(framebuffer[i]==(i<8?77:113));
    TaxiPairColumnWork work={99,99};memset(framebuffer,113,sizeof framebuffer);
    assert(taxi_indexed_perspective_columns_polygon_work(framebuffer,top_edge,4,edge_texture,sizeof edge_texture,edge_sampling,&work));
    assert(work.texture_byte_reads==4&&work.framebuffer_halfword_writes==4);
    memset(edge_texture,0,sizeof edge_texture);memset(framebuffer,113,sizeof framebuffer);
    assert(taxi_indexed_perspective_columns_polygon_work(framebuffer,top_edge,4,edge_texture,sizeof edge_texture,edge_sampling,&work));
    assert(work.texture_byte_reads==4&&!work.framebuffer_halfword_writes);
    for(unsigned i=0;i<38400;i++)assert(framebuffer[i]==113);
    work=(TaxiPairColumnWork){99,99};assert(!taxi_indexed_perspective_columns_polygon_work(framebuffer,top_edge,4,edge_texture,0,edge_sampling,&work));
    assert(work.texture_byte_reads==99&&work.framebuffer_halfword_writes==99);
    for(unsigned i=0;i<38400;i++)assert(framebuffer[i]==113);
    TaxiIndexedVertex touch_left[4]={{-8,0,0,0,4096},{0,0,0,0,4096},{0,2,0,0,4096},{-8,2,0,0,4096}};
    memset(edge_texture,77,sizeof edge_texture);memset(framebuffer,113,sizeof framebuffer);
    assert(taxi_indexed_unshaded_clipped_polygon_work(framebuffer,touch_left,4,edge_texture,sizeof edge_texture,edge_sampling,&work));
    assert(work.texture_byte_reads==2&&work.framebuffer_halfword_writes==2);
    for(unsigned i=0;i<38400;i++)assert(framebuffer[i]==((i<2||i==240||i==241)?77:113));
    TaxiIndexedSpan clipped_spans[160];assert(taxi_indexed_unshaded_clipped_rows_bounds(clipped_spans,touch_left,4,0,240));
    assert(clipped_spans[0].x==0&&clipped_spans[0].pairs==1&&clipped_spans[1].pairs==1);
    TaxiIndexedVertex wide_touch[4]={{-32,0,0,0,4096},{-24,0,0,0,4096},{-24,2,0,0,4096},{-32,2,0,0,4096}};
    assert(taxi_indexed_unshaded_clipped_rows_bounds(clipped_spans,wide_touch,4,-24,264));
    assert(clipped_spans[0].x==-24&&clipped_spans[0].pairs==1&&clipped_spans[1].pairs==1);
    puts("PASS: original indexed pair/span and flat patterns, keyed retention, coordinate wrap, shade bounds and guards.");
}
