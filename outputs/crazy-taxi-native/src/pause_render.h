#ifndef TAXI_PAUSE_RENDER_H
#define TAXI_PAUSE_RENDER_H
#include "pause_menu.h"
typedef struct {unsigned type,id;int32_t x,y;unsigned group_or_palette;} TaxiPauseRenderRequest;
/* Original221F8 ordinary pause menu caller layout. Sprite type0/group7;
   text type1/palette0or2, font0 alignment4 scale256. Palette request2.
   Debug dialog3 and original OAM/pixel composition remain separate. */
int taxi_pause_render_requests(const TaxiPauseMenu *,uint8_t fade,uint16_t width,TaxiPauseRenderRequest out[6],unsigned *count,uint8_t *palette_request);
#endif
