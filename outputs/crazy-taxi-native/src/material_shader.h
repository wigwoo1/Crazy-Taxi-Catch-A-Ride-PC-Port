#ifndef TAXI_MATERIAL_SHADER_H
#define TAXI_MATERIAL_SHADER_H
#include <stdint.h>
/* OpenGL compatibility shader: original Q24 attribute selects eight palette
   halfwords; source screen coordinates select pair and odd-row parity.
   GPU triangle interpolation is a high-resolution adaptation, not ARM edge math. */
int taxi_material_shader_begin(const uint8_t material[28],const uint16_t palette[256],int16_t cap);
/* Active attribute material: index0=backdrop alpha0, othersBG2alpha1.
   Begin resets disabled to preserve original raster opaque writes.
   Alpha-test/blending must be disabled when retaining index0 coverage. */
int taxi_material_shader_bitmap_coverage(int enabled);
void taxi_material_shader_end(void);
void taxi_material_shader_free(void);
#endif
