#include "../src/fare.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(void) {
    TaxiFareState f={0};taxi_fare_start(&f,40960);
    assert(f.pending_whole_16a0==80&&f.pending_fraction_16a4==0);
    taxi_fare_start(&f,511);assert(f.pending_whole_16a0==0&&f.pending_fraction_16a4==49);
    taxi_fare_start(&f,-511);assert(f.pending_whole_16a0==-1&&f.pending_fraction_16a4==-50);
    taxi_fare_start(&f,INT32_MAX);assert(f.pending_whole_16a0==-1&&f.pending_fraction_16a4==49);
    taxi_fare_start(&f,40960);
    uint8_t tips[10][56]={{0}};
    tips[0][2]=64;tips[0][4]=3;tips[0][1]=75;
    tips[9][2]=255;tips[9][4]=2;tips[9][1]=50;
    tips[1][4]=99; /* Inactive record must not be paid. */
    taxi_fare_collect_tips(&f,tips);
    assert(f.pending_whole_16a0==86&&f.pending_fraction_16a4==25&&!tips[0][2]&&!tips[9][2]);
    taxi_fare_collect_tips(&f,tips);assert(f.pending_whole_16a0==86); /* Once only. */
    f.total_whole_1698=999;f.total_fraction_169c=90;taxi_fare_settle(&f);
    assert(f.total_whole_1698==1086&&f.total_fraction_169c==15&&f.rating_1693==6&&!f.qualified_1691);
    assert(!f.pending_whole_16a0&&!f.pending_fraction_16a4);
    const int32_t thresholds[]={1000,2000,3000,4000,5000,10000,20000};
    for(unsigned i=0;i<7;i++) {
        f=(TaxiFareState){.pending_whole_16a0=thresholds[i]};taxi_fare_settle(&f);assert(f.rating_1693==7-i);
        f=(TaxiFareState){.pending_whole_16a0=thresholds[i]+1};taxi_fare_settle(&f);assert(f.rating_1693==6-i);
    }
    f=(TaxiFareState){0};taxi_fare_settle(&f);assert(f.rating_1693==8);
    f=(TaxiFareState){.total_whole_1698=INT32_MAX,.pending_whole_16a0=1,.pending_fraction_16a4=-1};
    taxi_fare_settle(&f);assert(f.total_whole_1698==INT32_MIN&&f.total_fraction_169c==-1&&f.rating_1693==7);
    f=(TaxiFareState){0};tips[3][2]=1;tips[3][1]=255;
    taxi_fare_collect_tips(&f,tips);assert(f.pending_fraction_16a4==-1);
    TaxiCashGlyphs glyphs;taxi_cash_glyphs_init(&glyphs);
    f=(TaxiFareState){.pending_whole_16a0=1491};taxi_cash_glyphs_targets(&glyphs,&f);
    assert(glyphs.total_target[6]==0&&glyphs.total_target[8]==0&&glyphs.total_target[9]==0);
    assert(glyphs.pending_target[1]==8&&glyphs.pending_target[3]==32&&glyphs.pending_target[4]==72&&glyphs.pending_target[5]==8);
    taxi_cash_glyphs_step(&glyphs);assert(glyphs.pending[4]==0); /* FFFF increments to0. */
    for(unsigned i=0;i<72;i++)taxi_cash_glyphs_step(&glyphs);
    assert(glyphs.pending[4]==72&&glyphs.pending[3]==32&&glyphs.pending[0]==UINT16_MAX);
    glyphs.total[0]=59;glyphs.total_target[0]=0;glyphs.pending[0]=79;glyphs.pending_target[0]=0;
    taxi_cash_glyphs_step(&glyphs);assert(!glyphs.total[0]&&!glyphs.pending[0]);
    glyphs.total[0]=24;glyphs.total_target[0]=UINT16_MAX;taxi_cash_glyphs_step(&glyphs);assert(glyphs.total[0]==UINT16_MAX);
    puts("Fare scalar and original cash rolling rows: reset, grouping, blanking, wrap and target convergence passed; no playable fare claimed");return 0;
}
