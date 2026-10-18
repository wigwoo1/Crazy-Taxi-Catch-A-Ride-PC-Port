#ifndef TAXI_RENDER_FRAME_INIT_H
#define TAXI_RENDER_FRAME_INIT_H
#include "render_viewport.h"
typedef int (*TaxiFrameDivide)(void *,uint32_t numerator,uint32_t focal,uint32_t division_owner,int32_t *result);
/* Original2B330 caller: eight near/far corners, untouched fourth words,
   and two engine-backed render stream initializations. 08032454 dispatches BX r2 to the caller-selected arithmetic callback;
   ordinary bootstrap installs relocated ARM 080029FC. This API retains
   that explicit callback boundary. */
typedef struct {int32_t corners[8][4];uint32_t streams[8];} TaxiRenderFrameInit;
int taxi_render_frame_init(TaxiRenderFrameInit *,const TaxiRenderViewport *,uint32_t engine_40,uint32_t engine_44,uint32_t division_owner,TaxiFrameDivide,void *);
#endif
