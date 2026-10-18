#ifndef TAXI_BOOT_BITMAP_TRANSFORM_H
#define TAXI_BOOT_BITMAP_TRANSFORM_H
#include <stdint.h>
typedef struct {int16_t pa,pb,pc,pd;int32_t reference_x,reference_y;} TaxiBitmapTransform;
/* Original1E38A..1E3CA writes BG2 and BG3 matrices and references.
   Called by original boot after setting retained r4=256 at1E34A..4C.
   Native typed state only, no hardware register execution. */
int taxi_boot_bitmap_transforms(TaxiBitmapTransform state[2]);
#endif
