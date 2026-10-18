#ifndef TAXI_RANK_DISPLAY_OWNED_H
#define TAXI_RANK_DISPLAY_OWNED_H
#include "rank_display.h"
/* Views of the existing HUD-start owner's arrays. Storage outlives this view. */
typedef struct {uint8_t *scalar_16c0,*cache_5300;} TaxiRankDisplayOwner;
int taxi_rank_display_owner_valid(const TaxiRankDisplayOwner *);
int taxi_rank_display_owned_step(TaxiRankDisplayOwner *,uint8_t settings,
                                uint8_t rank,uint8_t trigger,int32_t delta);
#endif
