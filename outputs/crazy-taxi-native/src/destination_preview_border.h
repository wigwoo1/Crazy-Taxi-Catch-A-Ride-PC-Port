#ifndef TAXI_DESTINATION_PREVIEW_BORDER_H
#define TAXI_DESTINATION_PREVIEW_BORDER_H
#include <stddef.h>
#include <stdint.h>
/* Original31860 suffix31902..31A26. Indexed4096-byte owner. Source
   horizontal addressing uses width rounded down to even, including odd widths.
   Scene rendering/state snapshot is external; not complete31860. */
int taxi_destination_preview_border(uint8_t *,size_t,unsigned,unsigned,uint8_t);
#endif
