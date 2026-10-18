#ifndef TAXI_RENDER_HISTORY_H
#define TAXI_RENDER_HISTORY_H
#include <stdint.h>
enum {TAXI_RENDER_HISTORY_CAPACITY=16};
typedef struct {uint64_t time_ns,tag;} TaxiRenderStamp;
typedef struct {TaxiRenderStamp stamps[TAXI_RENDER_HISTORY_CAPACITY];unsigned begin,count;} TaxiRenderHistory;
typedef struct {uint64_t before_tag,after_tag;double alpha;uint8_t held;} TaxiRenderBracket;
/* Presentation timestamps only; caller owns matching immutable pose snapshots.
   Clear on reset/focus rebase. No gameplay/audio state or extrapolation. */
void taxi_render_history_clear(TaxiRenderHistory *);
int taxi_render_history_push(TaxiRenderHistory *,uint64_t time_ns,uint64_t tag);
int taxi_render_history_bracket(const TaxiRenderHistory *,uint64_t target_ns,TaxiRenderBracket *);
#endif
