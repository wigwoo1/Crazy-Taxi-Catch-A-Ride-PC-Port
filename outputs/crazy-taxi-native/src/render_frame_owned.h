#ifndef TAXI_RENDER_FRAME_OWNED_H
#define TAXI_RENDER_FRAME_OWNED_H
#include "render_frame_init.h"
/* Original2B330 corner/stream initialization with original global03006940
   routed to native projection division. Unsupported callback fails early. */
int taxi_render_frame_owned(TaxiRenderFrameInit *,const TaxiRenderViewport *,uint32_t engine40,uint32_t engine44,uint32_t callback_global);
#endif
