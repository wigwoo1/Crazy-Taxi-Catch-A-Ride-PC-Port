#ifndef TAXI_GRAPHICS_BOOTSTRAP_OWNED_H
#define TAXI_GRAPHICS_BOOTSTRAP_OWNED_H
#include "graphics_bootstrap_prefix.h"
#include "graphics_bootstrap_tail.h"
/* Native2AC6C composition: binding/allocation semantics and existing typed
   graphics tail. Logical engine presence follows successful native binding.
   This never executes relocated ROM instructions; callback bodies and their
   supported domains retain the existing native helper contracts. */
int taxi_graphics_bootstrap_owned(TaxiGraphicsBootstrapPrefix *,TaxiGraphicsBootstrapTail *,TaxiRestartRenderContext *,TaxiSceneRegistration *,const TaxiGraphicsBootstrapAssets *,const int32_t bounds[4],int32_t near_plane,int32_t far_plane);
#endif
