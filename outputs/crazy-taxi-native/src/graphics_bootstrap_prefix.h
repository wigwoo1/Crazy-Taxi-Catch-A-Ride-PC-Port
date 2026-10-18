#ifndef TAXI_GRAPHICS_BOOTSTRAP_PREFIX_H
#define TAXI_GRAPHICS_BOOTSTRAP_PREFIX_H
#include "restart_render_context.h"
typedef struct {uint32_t owner,entry;} TaxiGraphicsCallbackBinding;
typedef struct {uint32_t source_site,entry;} TaxiGraphicsInternalBinding;
typedef struct {
 uint32_t image_source,image_identity,image_bytes;
 TaxiGraphicsCallbackBinding callbacks[42];TaxiGraphicsInternalBinding internal[3];
 uint8_t initialized;
} TaxiGraphicsBootstrapPrefix;
/* Native semantic2AC6C..2AE58: source2C8C4 allocation bookkeeping plus42
   callback bindings and3internal links. Source addresses are data identities,
   never host pointers or machine code. No ARM image is copied or executed.
   state_2c4ec is the source allocation cursor (consumed then reset by tail).
   Qualified aligned cursor0..676 only; failures leave prefix/cursor unchanged.
   Initialized bindings do not claim every callback BODY has been ported. */
int taxi_graphics_bootstrap_prefix(TaxiGraphicsBootstrapPrefix *,TaxiRestartRenderContext *);
uint32_t taxi_graphics_bootstrap_binding(const TaxiGraphicsBootstrapPrefix *,uint32_t owner);
#endif
