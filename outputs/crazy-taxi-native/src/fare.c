#include "fare.h"
#include <string.h>
#include <stdio.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t asr12(uint32_t u) {return bits((u>>12)|(u&0x80000000u?0xfff00000u:0));}
static int32_t word(const uint8_t *p) {return bits(p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24));}
static void normalize(int32_t *whole,int32_t *fraction) {
    /* Original loop carries only positive fractions>99; never borrows. */
    if(*fraction>99) {*whole=add(*whole,*fraction/100);*fraction%=100;}
}
void taxi_fare_start(TaxiFareState *s,int32_t length) {
    int32_t whole=length/256;
    int32_t residual=bits((uint32_t)length-((uint32_t)whole<<8));
    int32_t fraction=bits((uint32_t)residual<<12)/0x28f5;
    if(fraction>99) fraction=99;
    s->pending_whole_16a0=asr12((uint32_t)whole<<11);
    s->pending_fraction_16a4=asr12((uint32_t)fraction<<11);
    taxi_fare_normalize_pending(s);
}
void taxi_fare_normalize_pending(TaxiFareState *s) {
    normalize(&s->pending_whole_16a0,&s->pending_fraction_16a4);
}
void taxi_fare_settle(TaxiFareState *s) {
    s->total_whole_1698=add(s->total_whole_1698,s->pending_whole_16a0);
    s->total_fraction_169c=add(s->total_fraction_169c,s->pending_fraction_16a4);
    s->pending_whole_16a0=s->pending_fraction_16a4=0;
    normalize(&s->total_whole_1698,&s->total_fraction_169c);
    int32_t w=s->total_whole_1698;
    s->rating_1693=(uint8_t)(w==0&&s->total_fraction_169c==0?8:
        w>20000?0:w>10000?1:w>5000?2:w>4000?3:w>3000?4:w>2000?5:w>1000?6:7);
    s->qualified_1691=(uint8_t)(s->rating_1693<=2);
}
void taxi_fare_collect_tips(TaxiFareState *s,uint8_t tips[10][56]) {
    for(unsigned i=0;i<10;i++) if(tips[i][2]) {
        tips[i][2]=0;
        const unsigned offsets[4]={32,36,8,12};
        const uint32_t positions[4]={192u*256,148u*256,112u*256,16u*256};
        for(unsigned k=0;k<4;k++)for(unsigned j=0;j<4;j++)tips[i][offsets[k]+j]=(uint8_t)(positions[k]>>(j*8));
        s->pending_whole_16a0=add(s->pending_whole_16a0,word(tips[i]+4));
        int32_t fraction=tips[i][1]<128?tips[i][1]:(int32_t)tips[i][1]-256;
        s->pending_fraction_16a4=add(s->pending_fraction_16a4,fraction);
    }
    taxi_fare_normalize_pending(s); /* Original tail calls0800c540(2,-1). */
}

void taxi_cash_glyphs_init(TaxiCashGlyphs *g) {memset(g,255,sizeof *g);}
static void glyph_targets(uint16_t *target,unsigned slots,unsigned step,int32_t whole,int32_t fraction) {
    char number[32],grouped[40],padded[11];snprintf(number,sizeof number,"%d",whole);
    unsigned length=(unsigned)strlen(number),n=0;
    for(unsigned i=0;i<length;i++) {
        if(i&&((length-i)%3)==0)grouped[n++]=',';
        grouped[n++]=number[i];
    }
    /* Original10830 writes two byte digits from UNSIGNED divide/remainder,
       even for signed negative fractions; it does not printf the fraction. */
    grouped[n++]='.';
    grouped[n++]=(char)(uint8_t)((uint32_t)fraction/10u+48u);
    grouped[n++]=(char)(uint8_t)((uint32_t)fraction%10u+48u);
    grouped[n]=0;
    length=(unsigned)strlen(grouped);memset(padded,' ',slots);padded[slots]=0;
    if(length<=slots)memcpy(padded+slots-length,grouped,length);
    for(unsigned i=0;i<slots;i++) {
        if(padded[i]>='0'&&padded[i]<='9')target[i]=(uint16_t)((padded[i]-'0')*step);
        else if(padded[i]==' '&&i!=(slots==10?3u:2u)&&i!=(slots==10?7u:6u))target[i]=UINT16_MAX;
        /* Original punctuation/non-digit path preserves the prior halfword. */
    }
}
static FILE *glyph_trace;static unsigned glyph_trace_count;
void taxi_cash_glyphs_trace(FILE *sink){glyph_trace=sink;glyph_trace_count=0;}
void taxi_cash_glyphs_targets(TaxiCashGlyphs *g,const TaxiFareState *f) {
    int record=glyph_trace&&glyph_trace_count<16384;
    if(record){
        fprintf(glyph_trace,"%ld %ld %ld %ld %u %u",(long)f->total_whole_1698,(long)f->total_fraction_169c,
            (long)f->pending_whole_16a0,(long)f->pending_fraction_16a4,(unsigned)f->rating_1693,(unsigned)f->qualified_1691);
        for(unsigned i=0;i<10;i++)fprintf(glyph_trace," %u",(unsigned)g->total_target[i]);
        for(unsigned i=0;i<9;i++)fprintf(glyph_trace," %u",(unsigned)g->pending_target[i]);
    }
    glyph_targets(g->total_target,10,6,f->total_whole_1698,f->total_fraction_169c);
    glyph_targets(g->pending_target,9,8,f->pending_whole_16a0,f->pending_fraction_16a4);
    if(record){
        for(unsigned i=0;i<10;i++)fprintf(glyph_trace," %u",(unsigned)g->total_target[i]);
        for(unsigned i=0;i<9;i++)fprintf(glyph_trace," %u",(unsigned)g->pending_target[i]);
        fputc('\n',glyph_trace);++glyph_trace_count;
    }
}
static void glyph_step(uint16_t *current,const uint16_t *target,unsigned count,unsigned wrap) {
    for(unsigned i=0;i<count;i++)if(current[i]!=target[i]) {
        if(target[i]==UINT16_MAX)current[i]=UINT16_MAX;
        else {current[i]=(uint16_t)(current[i]+1);if(current[i]==wrap)current[i]=0;}
    }
}
void taxi_cash_glyphs_step(TaxiCashGlyphs *g) {
    glyph_step(g->total,g->total_target,10,60);
    glyph_step(g->pending,g->pending_target,9,80);
}

static FILE *collection_trace;
void taxi_tip_collection_trace(FILE *sink){collection_trace=sink;}
static void collection_record(const TaxiFareState *f,const uint8_t tips[10][56],uint8_t count,const TaxiCashGlyphs *g){
 fprintf(collection_trace," %ld %ld %u",(long)f->pending_whole_16a0,(long)f->pending_fraction_16a4,count);
 for(unsigned i=0;i<9;i++)fprintf(collection_trace," %u",g->pending_target[i]);
 for(unsigned i=0;i<9;i++)fprintf(collection_trace," %u",g->pending[i]);
 for(unsigned i=0;i<10;i++)for(unsigned j=0;j<56;j++)fprintf(collection_trace," %u",tips[i][j]);
}
void taxi_fare_collect_tips_display(TaxiFareState *f,uint8_t tips[10][56],uint8_t *active_count,TaxiCashGlyphs *g){
    if(collection_trace)collection_record(f,tips,*active_count,g);
    *active_count=0;
    taxi_fare_collect_tips(f,tips);
    uint16_t total[10];memcpy(total,g->total_target,sizeof total);
    taxi_cash_glyphs_targets(g,f);memcpy(g->total_target,total,sizeof total);
    memcpy(g->pending,g->pending_target,sizeof g->pending);
    if(collection_trace){collection_record(f,tips,*active_count,g);fputc('\n',collection_trace);}
}
