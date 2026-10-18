#ifndef TAXI_SCENE_REGISTRATION_RENDER_H
#define TAXI_SCENE_REGISTRATION_RENDER_H
#include "scene_registration.h"
#include "restart_render_context.h"
#include "indexed_asset.h"
/* Valid-scene native2B44C composition: publish copied scene -> black clear ->
   source sky pattern -> retained shade parameters. Nativeinvalidasset rejection
   replaces originalfatal diagnostics; no full2AC6C registration/relocation. */
int taxi_scene_registration_render(TaxiSceneRegistration *,const uint8_t *,size_t,
    const TaxiIndexedAsset *,const uint16_t palette[256],TaxiRestartRenderContext *);
#endif
