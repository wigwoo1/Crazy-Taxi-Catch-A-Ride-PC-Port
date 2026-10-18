#ifndef TAXI_GRAPHICS_PACKAGE_H
#define TAXI_GRAPHICS_PACKAGE_H
#include <stddef.h>
#include <stdint.h>
typedef struct {uint8_t *bytes;size_t size;} TaxiGraphicsPackage;
/* Immutable source-addressed graphics data only; never interpreted as code.
   Owner must start zeroed. Invalid package leaves prior owner unchanged. */
int taxi_graphics_package_load(TaxiGraphicsPackage *,const char *path);
void taxi_graphics_package_release(TaxiGraphicsPackage *);
#endif
