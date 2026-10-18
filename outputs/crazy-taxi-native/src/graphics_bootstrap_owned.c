#include "graphics_bootstrap_owned.h"
int taxi_graphics_bootstrap_owned(TaxiGraphicsBootstrapPrefix *prefix,TaxiGraphicsBootstrapTail *tail,TaxiRestartRenderContext *render,TaxiSceneRegistration *registration,const TaxiGraphicsBootstrapAssets *assets,const int32_t bounds[4],int32_t near_plane,int32_t far_plane){
 if(!prefix||!tail||!render||!registration||!assets||!bounds)return -1;
 if(taxi_graphics_bootstrap_prefix(prefix,render))return -1;
 render->engine_present=1;
 return taxi_graphics_bootstrap_tail(tail,render,registration,assets,bounds,near_plane,far_plane);
}
