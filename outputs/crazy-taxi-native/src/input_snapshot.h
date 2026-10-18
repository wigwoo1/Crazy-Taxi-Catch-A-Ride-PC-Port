#ifndef TAXI_INPUT_SNAPSHOT_H
#define TAXI_INPUT_SNAPSHOT_H
#include "input.h"
#include <stddef.h>
/* Diagnostic original input-state import. No hardware reads or CPU execution.
   Input bytes begin at original34DC; all owners through351D are required. */
int taxi_input_snapshot_read(TaxiInput *,const uint8_t *,size_t);
#endif
