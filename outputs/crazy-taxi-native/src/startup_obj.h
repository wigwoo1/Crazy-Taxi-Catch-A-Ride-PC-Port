#ifndef TAXI_STARTUP_OBJ_H
#define TAXI_STARTUP_OBJ_H
#include "obj_order.h"
typedef struct {TaxiObjField field;int affine;int16_t matrix[4];} TaxiStartupObjPlacement;
/* Original28288 metric offsets vs28658 centered zero-angle field.
   Scale256 delegates to non-affine placement. Producer attributes, bucket
   ordering, clipping, palette/window behavior remain separate dependencies. */
int taxi_startup_obj_placement(const uint16_t metrics[9],int32_t x,int32_t y,int16_t scale,TaxiStartupObjPlacement *);
#endif
