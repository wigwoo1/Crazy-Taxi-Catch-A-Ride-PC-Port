#ifndef TAXI_SPRITE_H
#define TAXI_SPRITE_H
#include <stdint.h>
/* Original08015586..080155b6: camera angle and vehicleheading1c select64
   views. Heading fixed-point conversion truncates towardzero, notASR12.
   Drift69c is not included by this original sprite selector. */
unsigned taxi_sprite_frame(int16_t camera_angle,int32_t heading_1c);
/* Native perspective adaptation of080156d8..0801599a. Original affine
   scale=(focal<<11)/distance has8 fractional bits, so one sprite pixel
   spans8 world units. Half centres are(-32,-32) and(31,-32) scaled pixels
   around the projected positionY-192. Returned bounds are screen-up local
   coordinates; second half overlaps first by one original pixel. */
typedef struct {float left,right,bottom,top;} TaxiSpriteBounds;
static inline TaxiSpriteBounds taxi_sprite_half_bounds(unsigned half) {
    return (TaxiSpriteBounds){half?-8.0f:-512.0f,half?504.0f:0.0f,0,512};
}
#endif
