#ifndef TAXI_SCENE_REGISTRATION_H
#define TAXI_SCENE_REGISTRATION_H
#include <stdint.h>
#include <stddef.h>
/* Native copy/valid-scene checks from2B44C. Original pointer fields remain
   source identities; they must never be dereferenced as host addresses.
   Clear-color, background pattern and shade helpers are separate adapters. */
typedef struct {uint8_t descriptor[104];uint16_t materials,textures,vertices,polygons,special,groups;int16_t background;uint8_t registered;} TaxiSceneRegistration;
int taxi_scene_registration_set(TaxiSceneRegistration *,const uint8_t *,size_t);
int taxi_scene_registration_load(TaxiSceneRegistration *,const char *path);
#endif
