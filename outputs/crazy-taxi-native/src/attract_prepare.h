#ifndef TAXI_ATTRACT_PREPARE_H
#define TAXI_ATTRACT_PREPARE_H
#include <stdint.h>
/* Original1708..171B recorder/override owner. Retain all unassigned bytes,
   including override mask170C and pointer1714. */
typedef struct {uint8_t recorder[20],flag_1ea8,flag_3895;} TaxiAttractPrepare;
void taxi_attract_prepare(TaxiAttractPrepare *);
#endif
