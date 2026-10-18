#ifndef TAXI_INDEXED_PREVIEW_SCALE_H
#define TAXI_INDEXED_PREVIEW_SCALE_H
#include <stddef.h>
#include <stdint.h>
/* Original2C918 indexed scaling. Truncated16.16 increments are accumulated
   per pixel/row; direct multiply-and-divide gives different samples. */
int taxi_indexed_preview_scale(const uint8_t *,size_t,unsigned,unsigned,uint8_t *,size_t,unsigned,unsigned);
#endif
