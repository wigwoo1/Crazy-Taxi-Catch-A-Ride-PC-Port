#include "graphics_bootstrap_prefix.h"
#include "engine_callback_identity.h"
int taxi_graphics_bootstrap_prefix(TaxiGraphicsBootstrapPrefix *out,TaxiRestartRenderContext *render){
 if(!out||!render||render->state_2c4ec>676||(render->state_2c4ec&3))return -1;
 static const uint32_t owners[42]={
 0x03006908u, 0x0300690cu, 0x03006914u, 0x03006918u,
 0x0300691cu, 0x0300692cu, 0x03006930u, 0x03006934u,
 0x03006938u, 0x0300693cu, 0x03006940u, 0x03006944u,
 0x03006948u, 0x0300694cu, 0x03006950u, 0x030069a0u,
 0x030069a4u, 0x030069a8u, 0x030069acu, 0x030069b0u,
 0x030069b4u, 0x030069b8u, 0x03006a28u, 0x03006a2cu,
 0x03006a30u, 0x03006a34u, 0x03006a38u, 0x03006a3cu,
 0x03006a40u, 0x03006a90u, 0x03006a94u, 0x03006a98u,
 0x03006a9cu, 0x03006ab0u, 0x03006ab4u, 0x03006ab8u,
 0x03006abcu, 0x03006ac0u, 0x03006ac4u, 0x03006ac8u,
 0x03006accu, 0x03006ad0u,
 };
 TaxiGraphicsBootstrapPrefix next={.image_source=0x08000700,.image_identity=0x03000008+render->state_2c4ec,.image_bytes=0x495c};
 for(unsigned i=0;i<42;i++){
  uint32_t entry=taxi_engine_callback_identity(owners[i]);if(!entry)return -1;
  next.callbacks[i]=(TaxiGraphicsCallbackBinding){owners[i],entry};
 }
 next.internal[0]=(TaxiGraphicsInternalBinding){0x080049e8,0x08001db0};
 next.internal[1]=(TaxiGraphicsInternalBinding){0x080049ec,0x08001dc4};
 next.internal[2]=(TaxiGraphicsInternalBinding){0x08002d0c,0x08001dc4};
 next.initialized=1;*out=next;render->state_2c4ec+=0x495c;return 0;
}
uint32_t taxi_graphics_bootstrap_binding(const TaxiGraphicsBootstrapPrefix *s,uint32_t owner){
 if(!s||!s->initialized)return 0;
 for(unsigned i=0;i<42;i++)if(s->callbacks[i].owner==owner)return s->callbacks[i].entry;
 return 0;
}
