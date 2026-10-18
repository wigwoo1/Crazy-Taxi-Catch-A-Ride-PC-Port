#include "render_frame_owned.h"
#include "engine_callback_identity.h"
#include "engine_projection_call.h"
#include <string.h>
static int divide(void *context,uint32_t a,uint32_t b,uint32_t identity,int32_t *out){
 uint32_t global=*(const uint32_t *)context;if(identity!=taxi_engine_callback_identity(global))return -1;uint32_t result;
 if(taxi_engine_projection_call(global,a,b,&result))return -1;memcpy(out,&result,4);return 0;
}
int taxi_render_frame_owned(TaxiRenderFrameInit *s,const TaxiRenderViewport *v,uint32_t engine40,uint32_t engine44,uint32_t global){
 uint32_t identity=taxi_engine_callback_identity(global);if(identity!=0x080029fc)return -1;
 return taxi_render_frame_init(s,v,engine40,engine44,identity,divide,&global);
}
