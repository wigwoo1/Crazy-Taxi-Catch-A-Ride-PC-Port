#ifndef TAXI_FARE_H
#define TAXI_FARE_H
#include <stdint.h>
#include <stdio.h>
void taxi_cash_glyphs_trace(FILE *sink);
typedef struct {
    int32_t total_whole_1698,total_fraction_169c;
    int32_t pending_whole_16a0,pending_fraction_16a4;
    uint8_t rating_1693,qualified_1691;
} TaxiFareState;
/* Numeric projections of 0800c540 modes1(start),2(normalize),0(settle).
   Omits glyph animation, high-score rank/persistence and presentation. */
void taxi_fare_start(TaxiFareState *fare,int32_t route_length);
void taxi_fare_normalize_pending(TaxiFareState *fare);
void taxi_fare_settle(TaxiFareState *fare);
/* Scalar part of0802939c over ten original56-byte tip records. Active byte2
   nonzero: clear it and add word4/signed byte1 to pending money. Geometry/UI
   snapping from the rest of that function is not included here. */
void taxi_fare_collect_tips(TaxiFareState *fare,uint8_t tips[10][56]);
typedef struct {uint16_t total[10],total_target[10],pending[9],pending_target[9];} TaxiCashGlyphs;
/* Original801ee78 reset and800c8de..c9fc: increment each changing row by1,
   wrap total60/pending80; blank target hides immediately. No invented easing. */
void taxi_cash_glyphs_init(TaxiCashGlyphs *glyphs);
void taxi_cash_glyphs_targets(TaxiCashGlyphs *glyphs,const TaxiFareState *fare);
void taxi_cash_glyphs_step(TaxiCashGlyphs *glyphs);
/* Complete original2939C, including active count and immediate pending glyph sync. */
void taxi_fare_collect_tips_display(TaxiFareState *,uint8_t tips[10][56],uint8_t *,TaxiCashGlyphs *);
void taxi_tip_collection_trace(FILE *sink);
#endif
