#include "settlement_display.h"
int taxi_settlement_display_allowed(const TaxiSettlementDisplayInput *i){
    return i&&!i->fade_177b&&!i->outer_1ea8&&!i->start_1696&&
        !i->entry_ended_1697&&!i->freeze_5f20&&(i->settings_0&224u)!=128u;
}
int taxi_settlement_display_step(TaxiTripEndState *end,TaxiCashGlyphs *cash,
    TaxiRankDisplayOwner *rank,uint8_t numeric,const TaxiSettlementDisplayInput *i){
    if(!end||!i||(rank&&!taxi_rank_display_owner_valid(rank)))return -1;
    if(!taxi_settlement_display_allowed(i))return 0;
    if(end->result_16d0&&end->result_timer_16d2){
        uint16_t next=(uint16_t)((uint32_t)end->result_timer_16d2-(uint32_t)i->delta_3b00);
        end->result_timer_16d2=next;
        if(!next||(next&0x8000u)){end->result_timer_16d2=0;end->result_16d0=0;end->phase_16d1=0;}
    }
    if(cash)taxi_cash_glyphs_step(cash);
    return rank?taxi_rank_display_owned_step(rank,i->settings_0,numeric,0,i->delta_3b00):0;
}
