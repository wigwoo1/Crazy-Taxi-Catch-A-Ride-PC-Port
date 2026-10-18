#ifndef TAXI_RENDER_TAIL_OWNED_H
#define TAXI_RENDER_TAIL_OWNED_H
#include "palette_bank.h"
#include <stddef.h>
/* Original15A12..15A86 AFTER the ordinary cache/placement/driver palette tail.
   shared1534[0..2] are the canonical session failure flag/phase/byte timer.
   Always publishes the first projected half to shared9AB0. Failure animation
   observes BEFORE-update signed phase and fade, optionally overwrites OBJ4
   and record4's source identity, then advances BYTE timer/phase using3B00.
   No frame scheduler, GPU, CPU emulation, or hardware register access.
   Return1 copied source flash palette,0 no palette copy,-1 invalid native
   owner/source bounds with every supplied owner unchanged. Palette owners
   and ROM are required only on the original flash-copy branch. */
int taxi_render_tail_owned(TaxiPaletteRegistration *,TaxiPaletteBanks *,
 TaxiPaletteTransfer *,const uint8_t *rom,size_t rom_size,uint8_t shared1534[3],
 uint8_t fade177b,uint32_t delta3b00,const uint32_t first_half[3],uint32_t anchor9ab0[3]);
#endif