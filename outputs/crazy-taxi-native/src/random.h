#ifndef TAXI_RANDOM_H
#define TAXI_RANDOM_H
#include <stdint.h>
#include <stdio.h>
void taxi_random_trace(FILE *sink);
void taxi_random_trace_label(const char *label);
/* Original state02003b04; 0801e72c inclusive signed range. */
int32_t taxi_random_range(uint32_t *state,int32_t minimum,int32_t maximum);
int32_t taxi_random_range_tagged(uint32_t *state,int32_t minimum,int32_t maximum,const char *label);
void taxi_random_fold(uint32_t *state);
#endif
