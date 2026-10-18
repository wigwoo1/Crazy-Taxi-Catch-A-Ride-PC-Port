#ifndef TAXI_CREDITS_FLOW_H
#define TAXI_CREDITS_FLOW_H
#include <stdint.h>
typedef struct {int32_t y_q8,x_q8,line_q8;uint32_t angle,row,line;uint8_t timer,sound_started;} TaxiCreditsFlow;
/* fade_out is ordered identical fade-request count0..2, not a boolean. */
typedef struct {int32_t text_id;uint8_t sound29,fade_out,write_row;uint16_t scroll[3],tiles[30];} TaxiCreditsFrame;
void taxi_credits_flow_begin(TaxiCreditsFlow *);
/* Running-frame2446E..245FA only. Caller supplies original formatted31-byte
   line including palette prefix, sine table value at pre-step angle, and
   real delta/Confirm/Start. Setup, fade service, text/font assets and return
   belong to caller. Input gates4458..446C must already allow running frame. */
int taxi_credits_flow_step(TaxiCreditsFlow *,int32_t delta,int32_t sine,
 const uint8_t line[31],uint8_t effects,int confirm,int start,TaxiCreditsFrame *);
#endif
